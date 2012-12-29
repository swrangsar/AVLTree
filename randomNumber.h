#ifndef RANDOMNUMBER_H
#define RANDOMNUMBER_H

int getRandomNumber(void);
int *createNumber(int num);
int compareNumber(void *a, void *b);
void destroyNumber(void *num);
void showNumber(void *num);

#endif