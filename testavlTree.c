#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "avlTree.h"
#include "randomNumber.h"
#define LIMIT 31

int main()
{
	avlTree *tree = avlTreeInit(compareNumber, showNumber, destroyNumber);
	srand(time(NULL));
	int i, temp, *num;
	i = 0;
	while (i++ < LIMIT) {
		temp = getRandomNumber();
		if (avlTreeInsert(tree, createNumber(temp)))
			printf("inserted %d\n", temp);
		else
			printf("%d already exists!\n", temp);
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
		temp = getRandomNumber();
		num = createNumber(temp);
		if (avlTreeDelete(tree, num))
			printf("Deleting %d\n", temp);
		destroyNumber(num);
	}
	avlTreeShow(tree);
		
	avlTreeDestroy(tree);	
	return 0;
}