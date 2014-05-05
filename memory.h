#include <stdint.h>

typedef struct
{
	char *heapStart;
	char *heapAllocated;
	char *nextFreeMemory;
	int freeListEntries;
} memoryInfo;

void *allocateMemory(uint8_t size);

void freeMemory(void *mem);

memoryInfo dumpMemory();
