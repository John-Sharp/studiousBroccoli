#include <stddef.h>

#if 1
void * myMalloc(size_t x);
#define malloc(x) myMalloc(x)
void myFree(void * ptr);
#define free(x) myFree(x)
#endif

