#include <xinu.h>
#include <prodcons.h>

void consumer(int count) {
	// reads the value of the global variable 'n'
	// 'count' times
	// print consumed value e.g. consumed : 8
	int i;
	for (i = 0; i <= count; i++) {
		wait(produced);
		printf("consumed : %d\n",n);
		signal(consumed);
	}
}

