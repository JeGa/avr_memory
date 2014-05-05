#include "memory.h"

// Defined from linker script
extern char __heap_start;

char *heapStart = &__heap_start;
char *nextFreeMemory;
// TODO: char margin = 128;

struct memoryArea;

typedef struct memoryArea
{
	uint8_t size;
	struct memoryArea *next;
} memoryArea;

memoryArea *freeListStart;
memoryArea *freeListEnd;

// Prototypes
void *checkFreeList(uint8_t size);

/**
 * @param size Number of bytes to allocate (max is 256)
 */
void *allocateMemory(uint8_t size)
{
	memoryArea *newMemoryArea;
	void *memoryFromFreeList;
	
	// Size has to be at least the size of the pointer.
	if (size < sizeof(memoryArea*))
		return 0;
	
	if (nextFreeMemory == 0)
		nextFreeMemory = heapStart;
	
	// Check collision with stack.
	// TODO: Margin if ()
	
	memoryFromFreeList = checkFreeList(size);
	if (memoryFromFreeList)
		return memoryFromFreeList;
		
	// No memory from free list available, get new memory.
	newMemoryArea = (memoryArea *) nextFreeMemory;
	newMemoryArea->size = size;
	
	nextFreeMemory += size + sizeof(uint8_t);
	
	return &(newMemoryArea->next);
}

void freeMemory(void *mem)
{
	memoryArea *oldMemoryArea;
	
	oldMemoryArea = (memoryArea *) (mem - sizeof(uint8_t));
	
	if (freeListStart == 0) {
		freeListStart = oldMemoryArea;
		freeListEnd = oldMemoryArea;
	} else {
		freeListEnd->next = oldMemoryArea;
		freeListEnd = oldMemoryArea;
	}
	
	freeListEnd->next = 0;
}

memoryInfo dumpMemory()
{
	memoryInfo info;
	memoryArea *iterator;
	
	info.heapStart = heapStart;
	info.nextFreeMemory = nextFreeMemory;
	
	info.heapAllocated = (char *) (nextFreeMemory - heapStart);
	
	if (freeListStart == 0)
		info.freeListEntries = 0;
		
	for (iterator = freeListStart; iterator != 0; iterator = iterator->next) {
		info.heapAllocated -= (iterator->size + sizeof(uint8_t)); // + size variable
		++info.freeListEntries;
	}
	
	return info;
}

void *checkFreeList(uint8_t size)
{
	memoryArea *iterator, *i;
	memoryArea *bestFittingMemoryArea;
	
	// No entry in the free list.
	if (freeListStart == 0)
		return 0;
	
	bestFittingMemoryArea = 0;
		
	// Go through the free list and find a fitting free memory area.
	for (iterator = freeListStart; iterator != 0; iterator = iterator->next) {
		
		if (iterator->size < size)
			continue;
		
		// Find possible memory area.
		if (iterator->size == size) {
			// Same size, perfect.
			bestFittingMemoryArea = iterator;
			break;
		} else {
			// Save the next best fitting area.
			if (bestFittingMemoryArea == 0)
				bestFittingMemoryArea = iterator;
			else if (iterator->size < bestFittingMemoryArea->size)
				bestFittingMemoryArea = iterator;
		}
	}
	
	if (bestFittingMemoryArea == 0)
		return 0; // No area found.
	
	// Check if there is only one element in the list.
	if (freeListStart == freeListEnd) {
		freeListStart = 0;
		freeListEnd = 0;
		
		bestFittingMemoryArea->next = 0;
		
		return &(bestFittingMemoryArea->next);
	}
	
	// Check the first element in the list.
	if (freeListStart == bestFittingMemoryArea) {
		freeListStart = freeListStart->next;
		
		bestFittingMemoryArea->next = 0;
		
		return &(bestFittingMemoryArea->next);
	}
	
	// Go through the other entries in the list.
	for (iterator = freeListStart->next, i = freeListStart;
		 iterator != 0;
		 i = iterator, iterator = iterator->next) {
		
		if (iterator == bestFittingMemoryArea) {
			i->next = iterator->next;
			
			// if its the last element in the list, 
			if (iterator == freeListEnd)
				freeListEnd = i;
			
			break;
		}

	}
	
	bestFittingMemoryArea->next = 0;
	
	return &(bestFittingMemoryArea->next);
}