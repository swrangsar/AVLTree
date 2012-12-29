CFLAGS = -Wall -g -DNDEBUG

all: testavlTree
avlTree.o: avlTree.h
randomNumber.o: randomNumber.h
testavlTree: avlTree.o randomNumber.o testavlTree.o
clean:
	rm -f testavlTree avlTree.o randomNumber.o testavlTree.o