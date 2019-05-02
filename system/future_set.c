//future_set.c
#include <xinu.h>
#include <future.h>
#include <stdio.h>

syscall future_set(future *f, char * value) {
	uint16 flags = f->flags;
	switch(flags) {
		case FUTURE_EXCLUSIVE: {
			if(f->state == FUTURE_EMPTY){
				f->value = *value;
				//printf("*value %d, f->value: %d\n", *value, f->value);
				f->state = FUTURE_READY;
				return OK;             
			} else if (f->state == FUTURE_WAITING) {
                		pid32 pid;
                		f->value = *value;
				//printf("*value %d, f->value: %d\n", *value, f->value);
				f->state = FUTURE_READY;
                		pid = f->pid;
				resume(pid);
                		return OK;
			} else if (f->state == FUTURE_READY) {
				fprintf(stderr, "Error: Old value not read\n");
                		return SYSERR;
			}
			break;
        	}
        	case FUTURE_SHARED: {
			if (f->state == FUTURE_EMPTY) {
				f->value = *value;
				//printf("*value %d, f->value: %d\n", *value, f->value);
				f->state = FUTURE_READY;
				return OK;
			} else if (f->state == FUTURE_WAITING) {
				pid32 pid;
				f->value = *value;
				//printf("*value %d, f->value: %d\n", *value, f->value);
				f->state= FUTURE_READY;  
				while ((pid = pop(f->get_queue)) != -1) {
					resume(pid);
				}
			} else if (f->state == FUTURE_READY) {
				fprintf(stderr, "Error: Multiple set not allowed\n");
				return SYSERR;
			}
			break;
		}
		case FUTURE_QUEUE : {
			if (f->state == FUTURE_EMPTY || f->state == FUTURE_READY) {
				push(f->set_queue, currpid);
				f->state = FUTURE_READY;
				suspend(currpid);
			}
			if (f->state == FUTURE_WAITING) {
				f->value = *value;
				//printf("*value %d, f->value: %d\n", *value, f->value);
				f->state = FUTURE_READY;
				pid32 pid = pop(f->get_queue);
				resume(pid);
				return OK;
			}
			break;
        	}
        	default: {
			return SYSERR;
		}
		return SYSERR;
	}
	return SYSERR;
}
