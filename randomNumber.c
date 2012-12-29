#include <stdio.h>
#include "dbg.h"
#include "randomNumber.h"
#define MAX 1000

int getRandomNumber(void)
{
	return (int)((((double)rand())/RAND_MAX)*MAX);
}

int *createNumber(int num)
{
	int *new = malloc(sizeof(int));
	checkMemory(new);
	*new = num;
	return new;
}

int compareNumber(void *a, void *b)
{
	return (*(int *)a - *(int *)b);
}

void destroyNumber(void *num)
{
	if (num) free(num);
	num = NULL;
}

void showNumber(void *num)
{
	if (num)
		printf("%d\n", *(int *)num);
	else
		printf("NULL\n");
}