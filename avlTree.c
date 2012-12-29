#include <stdio.h>
#include "dbg.h"
#include "avlTree.h"
#define	BALBOUND	3

static int isLeaf(avlNode *node);
static int isRoot(avlNode *node);
static void makeRoot(avlTree *tree, avlNode *node);
static void pairLeft(avlNode *parent, avlNode *child);
static void pairRight(avlNode *parent, avlNode *child);
static void orphan(avlNode *node);
static avlNode *createAvlNode(void *data);
static void destroyAvlNode(avlTree *tree, avlNode *node);
static avlNode *rotateLeft(avlNode *node);
static avlNode *rotateRight(avlNode *node);
static avlNode *doubleRotateLeft(avlNode *node);
static avlNode *doubleRotateRight(avlNode *node);
static avlNode *balance(avlNode *node);
static void updateBalance(avlTree *tree, avlNode *node);
static avlNode *insert(avlTree *tree, avlNode *node, void *data);
static avlNode *search(avlTree *tree, avlNode *node, void *data);
static void reduceBalance(avlTree *tree, avlNode *node);
static avlNode *predecessor(avlNode *node);
static void replaceWithPredecessor(avlTree *tree, avlNode *node);
static void deleteLeaf(avlTree *tree, avlNode *node);
static void deleteSingleChildNode(avlTree *tree, avlNode *node);
static void delete(avlTree *tree, avlNode *node);
static void inorder(avlTree *tree, avlNode *node, int height);
static void preorder(avlTree *tree, avlNode *node, int height);


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

static void makeRoot(avlTree *tree, avlNode *node)
{
	checkError(tree, "cannot make root of a null tree!");
	checkError(node, "Null node is not manually made a root!");
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

static avlNode *doubleRotateLeft(avlNode *node)
{
	checkError(node, "cannot left double rotate a null node!");
	avlNode *n, *r;
	n = node;
	r = node->right;
	checkError(r, "right child must exist for left rotation!");
	checkError(r->left, "left grandchild must exist for double left rotation!");
	rotateRight(r);
	return rotateLeft(n);
}

static avlNode *doubleRotateRight(avlNode *node)
{
	checkError(node, "cannot right double rotate a null node!");
	avlNode *n, *l;
	n = node;
	l = node->left;
	checkError(l, "left child must exist for right rotation!");
	checkError(l->right, "right grandchild must exist for double right rotation!");
	rotateLeft(l);
	return rotateRight(n);	
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
		if (l->balance > 0) {
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
			return doubleRotateRight(node);
		} else {
			node->balance = 0;
			++l->balance;
			return rotateRight(node);
		} 
	} else {
		checkError(r, "right child must exist!");
		checkError((abs(r->balance) < BALBOUND), "balance exceeded bound!");
		if (r->balance < 0) {
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
			return doubleRotateLeft(node);
		} else {
			node->balance = 0;
			--r->balance;
			return rotateLeft(node);	
		}
	}
}

static void updateBalance(avlTree *tree, avlNode *node)
{
	avlNode *p, *subroot;
	checkError(tree, "cannot update balance of a null tree!");
	checkError(node, "cannot update balance of a null node!");
	if (isRoot(node)) return;
	p = node->parent;
	(node == p->left)?(--(p->balance)):(++(p->balance));
	checkError((abs(p->balance) < BALBOUND), "balance exceeded bound!");
	if (abs(p->balance) < 2) {
		if (p->balance) updateBalance(tree, p);
	} else {
		subroot = balance(p);
		if (isRoot(subroot)) {
			tree->root = subroot;
			return;
		}
		checkError(!subroot->balance, "insertion: balancing should produce a zero balance!");
	}
}

static avlNode *insert(avlTree *tree, avlNode *node, void *data)
{
	checkError(tree, "cannot insert into a null tree!");
	checkError(node, "cannot insert next to a null node!");
	checkError(data, "null data not allowed to insert!");
	int diff = tree->compareData(data, node->data);
	if (diff < 0) {
		if (node->left) {
			return insert(tree, node->left, data);
		} else {
			/* do the insertion and balancing */
			avlNode *new = createAvlNode(data);
			pairLeft(node, new);
			updateBalance(tree, new);
			return new;
		}
	} else if (diff > 0) {
		if (node->right) {
			return insert(tree, node->right, data);
		} else {
			/* do the insertion and balancing */
			avlNode *new = createAvlNode(data);
			pairRight(node, new);
			updateBalance(tree, new);
			return new;
		}
	} else {
		tree->destroyData(data);
		printf("Data already exists and was not inserted!\n");
		return node;
	}
}

static avlNode *search(avlTree *tree, avlNode *node, void *data)
{
	if (!node) return NULL;
	int diff = tree->compareData(data, node->data);
	if (diff < 0) {
		return search(tree, node->left, data);
	} else if (diff > 0) {
		return search(tree, node->right, data);
	} else {
		return node;
	}
}

static void reduceBalance(avlTree *tree, avlNode *node)
{
	checkError(tree, "cannot reduce balance of a null tree!");
	checkError(node, "cannot reduce the balance of the parent of a null node!");
	avlNode *p = node->parent;
	int bal = node->balance;
	checkError((abs(bal) < BALBOUND), "balance exceeded bound!");
	if (isRoot(node)) {
		if (abs(bal) == 2) (tree->root = balance(node));
		return;
	}
	
	if (bal == 0) {
		(node == p->left)?(++p->balance):(--p->balance);
		checkError((abs(p->balance) < BALBOUND), "balance exceeded bound!");
		reduceBalance(tree, p);
	} else if (abs(bal) == 2) {
		reduceBalance(tree, balance(node));
	} else {
		return;
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
	(isRoot(node))?(makeRoot(tree, pred)):((node == p->left)?pairLeft(p, pred):pairRight(p, pred));
	if (pred == l) {
		pairLeft(pred, node);
		pairRight(pred, r);
		pairLeft(node, pl);
		pairRight(node, pr);
		return;
	} else {
		pairLeft(pred, l);
		pairRight(pred, r);
		pairLeft(node, pl);
		pairRight(node, pr);
		(pred == pp->left)?pairLeft(pp, node):pairRight(pp, node);
		return;	
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
	reduceBalance(tree, p);
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
		makeRoot(tree, child);
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
	reduceBalance(tree, p);
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

static void inorder(avlTree *tree, avlNode *node, int height)
{
	checkError(tree, "cannot inorder traverse a null tree!");
	if (node) {
		if (node->left) inorder(tree, node->left, height+1);
		printf("%3d: ", height);
		tree->showData(node->data);
		if (node->right) inorder(tree, node->right, height+1);
	}
}

static void preorder(avlTree *tree, avlNode *node, int height)
{
	checkError(tree, "cannot preorder traverse a null tree!");
	if (node) {
		printf("%3d: ", height);
		tree->showData(node->data);
		if (node->left) preorder(tree, node->left, height+1);
		if (node->right) preorder(tree, node->right, height+1);
	}
}

avlTree *createAvlTree(avlCompareFunc compareData, avlShowFunc showData, avlDestroyFunc destroyData)
{
	avlTree *new = malloc(sizeof(avlTree));
	checkMemory(new);
	new->root = NULL;
	new->compareData = compareData;
	new->showData = showData;
	new->destroyData = destroyData;
	return new;
}

void destroyAvlTree(avlTree *tree)
{
	checkError(tree, "Cannot destroy NULL tree!");
	destroyAvlNode(tree, tree->root);
	free(tree);
}

avlNode *avlTreeInsert(avlTree *tree, void *data)
{
	checkError(tree, "cannot insert into a null tree!");
	checkError(data, "null data not allowed to insert!");
	if (!tree->root) {
		tree->root = createAvlNode(data);
		return tree->root;
	} else {
		return insert(tree, tree->root, data);
	}
}

avlNode *avlTreeSearch(avlTree *tree, void *data)
{
	checkError(tree, "cannot search a null tree!");
	if (!tree->root) return NULL;
	return search(tree, tree->root, data);
}

void avlTreeDelete(avlTree *tree, void *data)
{
	checkError(tree, "cannot delete from null tree!");
	if (!tree->root) {
		printf("Deletion: tree has no elements!\n");
		return;
	}
	avlNode* node = search(tree, tree->root, data);
	if (!node) {
		return;
	}
	delete(tree, node);
	printf("deleted successfully!\n");
}

void avlTreeShow(avlTree *tree)
{
	checkError(tree, "cannot show a null tree!");
	if (!tree->root) {
		printf("TreeShow: This tree has no elements!\n");
		return;
	}
	printf("\n--- showing tree inorder ---\n\n");
	inorder(tree, tree->root, 0);
	printf("\n--- done showing tree inorder! ---\n");
	printf("\n--- showing tree preorder ---\n\n");	
	preorder(tree, tree->root, 0);
	printf("\n--- done showing tree preorder! ---\n");
}