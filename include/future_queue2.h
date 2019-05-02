#ifndef _FQUEUE_H
#define _FQUEUE_H
#include <xinu.h>

//Struct definitions
struct measurement {
        int timestamp;
        int value;
};
typedef struct measurement measurement;

//queue node struct
typedef struct qnode {
	measurement value;
	struct qnode * next;
} node;

//queue struct
typedef struct queue {
	node* front;
	node* rear;
} queue;

//queue functions
queue * create_queue2();
void push2(queue * q, measurement value);
measurement pop2(queue * q);
uint32 size2(queue * q);
syscall free_queue2(queue * q);
#endif
