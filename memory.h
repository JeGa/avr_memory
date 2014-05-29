/**
 * @file memory.h
 *
 * This file provides an own malloc implementation. The allocated memory is
 * located on the heap.
 *
 * When memory is freed, it is put to the free list. New memory allocations
 * look at first in the free list. If theres a suitable memory block in the
 * free list, it is returned. Otherwise, a new memory block is allocated.
 */

#pragma once

#include <stdint.h>

typedef struct
{
	char *heapStart;
	char *nextFreeMemory;
	int heapAllocated;
	int freeListEntries;
} memoryInfo;

/**
 * @param size Number of bytes to allocate (max is 256)
 *
 * @retval 0 Error allocating memory
 */
void *allocateMemory(uint8_t size);

void freeMemory(void *mem);

/**
 * For debugging and memory info.
 */
memoryInfo dumpMemory();
