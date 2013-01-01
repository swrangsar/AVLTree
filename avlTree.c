#include <stdio.h>
#include "dbg.h"
#include "avlTree.h"
#define	BALBOUND	3

static int isLeaf(avlNode *node);
static int isRoot(avlNode *node);
static void setRoot(avlTree *tree, avlNode *node);
static void pairLeft(avlNode *parent, avlNode *child);
static void pairRight(avlNode *parent, avlNode *child);
static void orphan(avlNode *node);
static avlNode *createAvlNode(void *data);
static void destroyAvlNode(avlTree *tree, avlNode *node);
static avlNode *rotateLeft(avlNode *node);
static avlNode *rotateRight(avlNode *node);
static avlNode *rotateLeftAndBalance(avlNode *node);
static avlNode *rotateRightAndBalance(avlNode *node);
static avlNode *doubleRotateLeftAndBalance(avlNode *node);
static avlNode *doubleRotateRightAndBalance(avlNode *node);
static avlNode *balance(avlNode *node);
static void fixupInsert(avlTree *tree, avlNode *node);
static void fixupDelete(avlTree *tree, avlNode *node);
static avlNode *predecessor(avlNode *node);
static void replaceWithPredecessor(avlTree *tree, avlNode *node);
static void deleteLeaf(avlTree *tree, avlNode *node);
static void deleteSingleChildNode(avlTree *tree, avlNode *node);
static void delete(avlTree *tree, avlNode *node);
static void inorder(avlTree *tree, avlNode *node);
static void preorder(avlTree *tree, avlNode *node);


/*****************************/
/*** Basic functions       ***/
/*****************************/


static int isLeaf(avlNode *node)
{
	checkError(node, "Null node can't be checked for a leaf!");
	return (!node->left && !node->right)?1:0;
}

static int isRoot(avlNode *node)
{
	checkError(node, "Null node can't be checked for root!");
	return (!node->parent)?1:0;
}

static void setRoot(avlTree *tree, avlNode *node)
{
	checkError(tree, "cannot set root of a null tree!");
	checkError(node, "Null node cant be set as root!");
	tree->root = node;
	if (node->parent) (node->parent = NULL);
}

static void pairLeft(avlNode *parent, avlNode *child)
{
	checkError(parent, "while left pairing parent cannot be null!");
	checkError(parent != child, "parent and child cannot be same!");
	parent->left = child;
	if (child) child->parent = parent;
}

static void pairRight(avlNode *parent, avlNode *child)
{
	checkError(parent, "while right pairing parent cannot be null!");
	checkError(parent != child, "parent and child cannot be same!");
	parent->right = child;
	if (child) child->parent = parent;
}

static void orphan(avlNode *node)
{
	checkError(node, "Null node can't be orphaned!");
	node->parent = node->left = node->right = NULL;
}



/************************************/
/*** Constructors and destructors ***/
/************************************/


static avlNode *createAvlNode(void *data)
{
	avlNode *new = malloc(sizeof(avlNode));
	checkMemory(new);
	new->data = (void *)data;
	new->balance = 0;
	new->left = NULL;
	new->right = NULL;
	new->parent = NULL;
	return new;
}

static void destroyAvlNode(avlTree *tree, avlNode *node)
{
	checkError(tree, "cannot destroy node of a null tree!");
	if (!node) return;
	if (node->left) destroyAvlNode(tree, node->left);
	if (node->right) destroyAvlNode(tree, node->right);
	avlNode *p = node->parent;
	if (p) (node == p->left)?(p->left = NULL):(p->right = NULL);
	if (node->data) tree->destroyData(node->data);
	node->data = NULL;
	free(node);
}

avlTree *avlTreeInit(avlCompareFunc compareData, avlShowFunc showData, avlDestroyFunc destroyData)
{
	avlTree *new = malloc(sizeof(avlTree));
	checkMemory(new);
	new->root = NULL;
	new->compareData = compareData;
	new->showData = showData;
	new->destroyData = destroyData;
	return new;
}

void avlTreeDestroy(avlTree *tree)
{
	checkError(tree, "Cannot destroy NULL tree!");
	destroyAvlNode(tree, tree->root);
	free(tree);
}



/*****************/
/*** Rotations ***/
/*****************/


static avlNode *rotateLeft(avlNode *node)
{
	checkError(node, "cannot left rotate a null node!");
	avlNode *p, *n, *r;
	p = node->parent;
	n = node;
	r = node->right;
	checkError(r, "right child must exist for left rotation!");
	pairRight(n, r->left);
	pairLeft(r, n);
	(p)?((n == p->left)?pairLeft(p, r):pairRight(p, r)):(r->parent = NULL);
	return r;	
}

static avlNode *rotateRight(avlNode *node)
{
	checkError(node, "cannot right rotate a null node!");
	avlNode *p, *n, *l;
	p = node->parent;
	n = node;
	l = node->left;
	checkError(l, "left child must exist for right rotation!");
	pairLeft(n, l->right);
	pairRight(l, n);
	(p)?((n == p->left)?pairLeft(p, l):pairRight(p, l)):(l->parent = NULL);
	return l;	
}



/*********************************/
/*** Balance factor management ***/
/*********************************/


static avlNode *rotateLeftAndBalance(avlNode *node)
{
	checkError(node, "cannot left rotate and balance a null node!");
	avlNode *n, *r;
	n = node;
	r = node->right;
	checkError(r, "right child must exist for left rotation!");
	node->balance = 0;
	--r->balance;
	return rotateLeft(node);	
}

static avlNode *rotateRightAndBalance(avlNode *node)
{
	checkError(node, "cannot right rotate and balance a null node!");
	avlNode *n, *l;
	n = node;
	l = node->left;
	checkError(l, "left child must exist for right rotation!");
	node->balance = 0;
	++l->balance;
	return rotateRight(node);
}

static avlNode *doubleRotateLeftAndBalance(avlNode *node)
{
	checkError(node, "cannot left double rotate a null node!");
	avlNode *r;
	r = node->right;
	checkError(r, "right child must exist for left rotation!");
	checkError(r->left, "left grandchild must exist for double left rotation!");
	char gcbal = r->left->balance;
	checkError((abs(gcbal) < BALBOUND-1), "balance exceeded bound!");
	r->left->balance = 0;
	if (gcbal < 0) {
		r->balance = 1;
		node->balance = 0;
	} else if (gcbal > 0) {
		r->balance = 0;
		node->balance = -1;
	} else {
		r->balance = 0;
		node->balance = 0;
	}		
	rotateRight(r);
	return rotateLeft(node);
}

static avlNode *doubleRotateRightAndBalance(avlNode *node)
{
	checkError(node, "cannot right double rotate a null node!");
	avlNode *l;
	l = node->left;
	checkError(l, "left child must exist for right rotation!");
	checkError(l->right, "right grandchild must exist for double right rotation!");
	char gcbal = l->right->balance;
	checkError((abs(gcbal) < BALBOUND-1), "balance exceeded bound!");
	l->right->balance = 0;
	if (gcbal < 0) {
		l->balance = 0;
		node->balance = 1;
	} else if (gcbal > 0) {
		l->balance = -1;
		node->balance = 0;
	} else {
		l->balance = 0;
		node->balance = 0;
	}
	rotateLeft(l);
	return rotateRight(node);	
}

static avlNode *balance(avlNode *node)
{
	checkError(node, "cannot balance a null node!");
	avlNode *l, *r;
	l = node->left;
	r = node->right;
	checkError((abs(node->balance) == 2), "node balancing can be done only for balance of +/-2!");
	if (node->balance == -2) {
		checkError(l, "left child must exist!");
		checkError((abs(l->balance) < BALBOUND), "balance exceeded bound!");
		return (l->balance > 0)?doubleRotateRightAndBalance(node):rotateRightAndBalance(node);
	} else {
		checkError(r, "right child must exist!");
		checkError((abs(r->balance) < BALBOUND), "balance exceeded bound!");
		return (r->balance < 0)?doubleRotateLeftAndBalance(node):rotateLeftAndBalance(node);
	}
}



/**************/
/*** Insert ***/
/**************/


static void fixupInsert(avlTree *tree, avlNode *node)
{
	avlNode *p, *subroot;
	checkError(tree, "cannot update balance of a null tree!");
	checkError(node, "cannot update balance of a null node!");
	while ((p = node->parent)) {
		(node == p->left)?(--(p->balance)):(++(p->balance));
		checkError((abs(p->balance) < BALBOUND), "balance exceeded bound!");
		if (abs(p->balance) != 1) break;
		node = p;
	}
	if (p && (abs(p->balance) == 2)) {
		if (isRoot(subroot = balance(p))) (tree->root = subroot);
	}
}

avlNode *avlTreeInsert(avlTree *tree, void *data)
{
	checkError(tree, "cannot insert into a null tree!");
	checkError(data, "null data not allowed to insert!");
	if (!tree->root) return (tree->root = createAvlNode(data));
	int diff;
	avlNode *node, *conductor, *new;
	conductor = tree->root;
	
	while (conductor) {
		node = conductor;
		diff = tree->compareData(data, node->data);
		if (diff < 0)
			conductor = conductor->left;
		else if (diff > 0)
			conductor = conductor->right;
		else
			return NULL;
	}
	new = createAvlNode(data);
	(diff < 0)?pairLeft(node, new):pairRight(node, new);
	fixupInsert(tree, new);
	return new;
}



/**************/
/*** Search ***/
/**************/


avlNode *avlTreeSearch(avlTree *tree, void *data)
{
	checkError(tree, "cannot search a null tree!");
	avlNode *node = tree->root;
	int diff;
	while (node) {
		diff = tree->compareData(data, node->data);
		if (diff < 0)
			node = node->left;
		else if (diff > 0)
			node = node->right;
		else
			return node;
	}
	return NULL;
}



/**************/
/*** Delete ***/
/**************/


static void fixupDelete(avlTree *tree, avlNode *node)
{
	checkError(tree, "cannot reduce balance of a null tree!");
	checkError(node, "cannot reduce the balance of the parent of a null node!");
	avlNode *p;
	int bal;	
	while (node) {
		p = node->parent;
		bal = node->balance;
		checkError((abs(bal) < BALBOUND), "balance exceeded bound!");
		if (bal == 0) {
			if (p) {
				(node == p->left)?(++p->balance):(--p->balance);				
				node = p;
			} else {
				return;
			}
		} else if (abs(bal) == 2) {
			node = balance(node);
			if (!node->parent) {
				setRoot(tree, node);
				return;
			}
		} else {
			return;
		}
	}
}

static avlNode *predecessor(avlNode *node)
{
	checkError(node, "cannot find predecessor of a null node!");
	avlNode *l = node->left;
	checkError(l, "cannot find predecessor if left child don't exist!");
	avlNode *conductor = l->right;
	avlNode *predecessor = l;
	while (conductor) {
		predecessor = conductor;
		conductor = conductor->right;
	};
	return predecessor;	
}

static void replaceWithPredecessor(avlTree *tree, avlNode *node)
{
	checkError(tree, "cannot replace with predecessor if null tree!");
	checkError(node, "cannot replace a null node with predecessor!");
	avlNode *l, *r, *p, *pred, *pl, *pr, *pp;
	char bal, pbal;
	l = node->left;
	r = node->right;
	p = node->parent;
	bal = node->balance;
	checkError(l && r, "cannot replace with predecessor if left n right child don't exist!");
	pred = predecessor(node);
	pl = pred->left;
	pr = pred->right;
	pp = pred->parent;
	pbal = pred->balance;
	
	pred->balance = bal;
	node->balance = pbal;
	(isRoot(node))?(setRoot(tree, pred)):((node == p->left)?pairLeft(p, pred):pairRight(p, pred));
	pairRight(pred, r);
	pairLeft(node, pl);
	pairRight(node, pr);
	if (pred == l) {
		pairLeft(pred, node);
	} else {
		pairLeft(pred, l);
		(pred == pp->left)?pairLeft(pp, node):pairRight(pp, node);
	}
}

static void deleteLeaf(avlTree *tree, avlNode *node)
{
	checkError(tree, "cannot delete from null tree!");
	checkError(node, "cannot delete null node!");
	checkError(isLeaf(node), "can delete a leaf only!");
	if (isRoot(node)) {
		destroyAvlNode(tree, node);
		tree->root = NULL;
		return;
	}
	avlNode *p = node->parent;
	(node == p->left)?(++(p->balance)):(--(p->balance));
	destroyAvlNode(tree, node);
	fixupDelete(tree, p);
}

static void deleteSingleChildNode(avlTree *tree, avlNode *node)
{
	checkError(tree, "cannot delete from null tree!");
	checkError(node, "cannot delete null node!");
	avlNode *child = (node->left)?(node->left):(node->right);
	avlNode *otherChild;
	otherChild = (child == node->left)?(node->right):(node->left);
	checkError(child, "single child can't be null!");
	checkError(!otherChild, "other child must be null!");
	if (isRoot(node)) {
		setRoot(tree, child);
		orphan(node);
		destroyAvlNode(tree, node);
		return;
	}
	avlNode *p = node->parent;
	(node == p->left)?(++(p->balance)):(--(p->balance));
	checkError((abs(p->balance) < BALBOUND), "balance exceeded bound!");
	(node == p->left)?(pairLeft(p, child)):(pairRight(p, child));
	orphan(node);
	destroyAvlNode(tree, node);
	fixupDelete(tree, p);
}

static void delete(avlTree *tree, avlNode *node)
{
	checkError(tree, "cannot delete from null tree!");
	checkError(node, "cannot delete null node!");
	avlNode *l, *r, *p;
	l = node->left;
	r = node->right;
	p = node->parent;
	if (isLeaf(node)) {
		deleteLeaf(tree, node);
	} else if (!l || !r) {
		deleteSingleChildNode(tree, node);
	} else {
		replaceWithPredecessor(tree, node);
		delete(tree, node);
	}
}

int avlTreeDelete(avlTree *tree, void *data)
{
	/* returns 1 if data is found and deleted */
	checkError(tree, "cannot delete from null tree!");
	avlNode* node = avlTreeSearch(tree, data);
	if (!node) return 0;
	delete(tree, node);
	return 1;
}



/*************************/
/*** Printing the tree ***/
/*************************/


static void inorder(avlTree *tree, avlNode *node)
{
	checkError(tree, "cannot inorder traverse a null tree!");
	if (node) {
		inorder(tree, node->left);
		tree->showData(node->data);
		printf("\n");
		inorder(tree, node->right);
	}
}

static void preorder(avlTree *tree, avlNode *node)
{
	checkError(tree, "cannot preorder traverse a null tree!");
	printf("{");
	if (node) {
		tree->showData(node->data);
		preorder(tree, node->left);
		preorder(tree, node->right);
	} else {
		printf("NULL");
	}
	printf("}");
	
}

void avlTreeShow(avlTree *tree)
{
	checkError(tree, "cannot show a null tree!");
	if (!tree->root) {
		printf("TreeShow: This tree has no elements!\n");
		return;
	}
	printf("\n--- showing tree inorder ---\n\n");
	inorder(tree, tree->root);
	printf("\n--- done showing tree inorder! ---\n");
	printf("\n--- showing tree preorder ---\n\n");	
	preorder(tree, tree->root);
	printf("\n--- done showing tree preorder! ---\n");
}


