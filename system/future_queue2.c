#include <future_queue2.h>

queue * create_queue2() {
	queue * q = (queue*)getmem(sizeof(queue));
	if((int32)q == SYSERR){
		return NULL;
	}
	
	q->front = NULL;
	q->rear = NULL;
	
	return q;
}

void push2(queue * q, measurement value){
	node * new_node = (node *)getmem(sizeof(node));
	if ((int32)new_node == SYSERR) {
		fprintf(stderr, "Memory allocation failed\n");
		return;
	}

	new_node->value = value;
	new_node->next = NULL;
	
	if (q->front == NULL &&  q->rear == NULL) {
		q->front = q->rear = new_node;
		return;
	}
	
	q->rear->next = new_node;
	q->rear = new_node;
}

measurement pop2(queue * q) { 
	measurement value;
	//if (q->front == NULL) {
		//return NULL;
	//}
	value = q->front->value;
	node *n = q->front;
	
	q->front = q->front->next;
	
	if (q->front == NULL) {
		q->rear = NULL;
	}
	
	freemem((char*)n, sizeof(node));;
	
	return value;
}

uint32 size2(queue * q) {
	uint32 count =0;
	node *cur = q->front;
	while (cur != NULL) {
		count++;
	}
	return count;
}

syscall free_queue2(queue * q){
    if (q != NULL) {
		
		node* cur = q->front;
		while (cur != NULL) {
			if (freemem((char *)q, sizeof(node)) != OK) {
				return SYSERR;
			}
			cur = cur->next;
		}
		q->front = NULL;
		q->rear = NULL;
		return freemem((char*)q, sizeof(queue));
    }
    return SYSERR;
}
