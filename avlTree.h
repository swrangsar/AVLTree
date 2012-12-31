#ifndef AVLTREE_H
#define AVLTREE_H

typedef struct avlNode {
	void *data;
	char balance;
	struct avlNode *left;
	struct avlNode *right;
	struct avlNode *parent;
} avlNode;

typedef int (*avlCompareFunc)(void *a, void *b);
typedef void (*avlShowFunc) (void *data);
typedef void (*avlDestroyFunc) (void *data);

typedef struct avlTree {
	avlNode *root;
	avlCompareFunc compareData;
	avlShowFunc showData;
	avlDestroyFunc destroyData;
} avlTree;

avlTree *avlTreeInit(avlCompareFunc compareData, avlShowFunc showData, avlDestroyFunc destroyData);
avlNode *avlTreeSearch(avlTree *tree, void *data);
avlNode *avlTreeInsert(avlTree *tree, void *data);
void avlTreeDestroy(avlTree *tree);
int avlTreeDelete(avlTree *tree, void *data);
void avlTreeShow(avlTree *tree);

#endif