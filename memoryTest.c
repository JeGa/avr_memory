#include "memory.h"

int main(void)
{
	volatile char *mem1, *mem2;
	volatile memoryInfo info1, info2, info3;
	
	mem1 = (char *) allocateMemory(9);
	info1 = dumpMemory();
	
	mem2 = (char *) allocateMemory(10);
	info2 = dumpMemory();
	
	freeMemory(mem1);
	freeMemory(mem2);
	info3 = dumpMemory();
	
	mem1 = (char *) allocateMemory(10);
	info1 = dumpMemory();
	
    while (1) {}
}