#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "avlTree.h"
#include "randomNumber.h"
#define LIMIT 300

int main()
{
	avlTree *tree = createAvlTree(compareNumber, showNumber, destroyNumber);
	srand(time(NULL));
	int i, temp, *num;
	i = 0;
	while (i++ < LIMIT) {
		printf("Inserting %d\n", temp = getRandomNumber());
		avlTreeInsert(tree, createNumber(temp));
	}
	
	avlTreeShow(tree);
	
	printf("\n--- search test started ---\n\n");
	i = 0;
	while (i++ < LIMIT) {
		temp = getRandomNumber();
		num = createNumber(temp);
		if (avlTreeSearch(tree, num)) {
			printf("found %d\n", temp);
		} else {
			printf("!found %d\n", temp);			
		}
		destroyNumber(num);
	}
	printf("\n\n--- search test over ---\n");
	
	i = 0;
	while (i++ < LIMIT) {
		printf("Deleting %d\n", temp = getRandomNumber());
		num = createNumber(temp);
		avlTreeDelete(tree, num);
		destroyNumber(num);
	}
	avlTreeShow(tree);
		
	destroyAvlTree(tree);
	return 0;
}