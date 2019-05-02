#include "myqueue.h"

myqueue * createQueue() {
        //myqueue * queue = (myqueue*)malloc(sizeof(myqueue));
        queue->head = 0;
        queue->size = 0;
        return queue;
}

void myqueueinsert(myqueue * queue, measurement m) {
	//node * e = (node*)malloc(sizeof(node));
	e->m = m;
	e->next = queue->head;
	queue->head = e;
	queue->size = queue->size + 1;
}

measurement myqueueremove(myqueue * queue) {
	node * e = queue->head;
	if (e == 0) {
		//empty queue
		return;
	}
	if (e->next == 0) {
		measurement result = e->m;
		//free(e);
		queue->head = 0;
		queue->size = queue->size - 1;
		return result;
	} else {
		while (e->next->next != 0) {
			e = e->next;
		}
		node * n = e->next;
		measurement result = n->m;
		//free(n);
		e->next = 0;
		queue->size = queue->size - 1;
		return result;
	}
}
