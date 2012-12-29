#ifndef DBG_H
#define DBG_H

#include <stdlib.h>

#define checkError(A, M, ...)  if (!(A)) {printf("DEBUG %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__); exit(EXIT_FAILURE);}
#define checkMemory(A)  if (!(A)) {printf("DEBUG %s:%d: memory could not be allocated!\n", __FILE__, __LINE__); exit(EXIT_FAILURE);}

#endif