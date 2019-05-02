#ifndef _FQUEUE_H
#define _FQUEUE_H
#include <xinu.h>

//queue node struct
typedef struct qnode {
	int32 value;
	struct qnode * next;
} node;

//queue struct
typedef struct queue {
	node* front;
	node* rear;
} queue;

//queue functions
queue * create_queue();
void push(queue * q, int32 value);
int32 pop(queue * q);
uint32 size(queue * q);
syscall free_queue(queue * q);
#endif
