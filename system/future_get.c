//future_get.c
#include <xinu.h>
#include <future.h>
#include <stdio.h>

syscall future_get(future *f, char* value) {
	uint16 flags = f->flags;
	
	switch(flags) {
		case FUTURE_EXCLUSIVE: {
			if (f->state == FUTURE_EMPTY) {
				f->state = FUTURE_WAITING;
                		f->pid = currpid;
                		suspend(currpid); 
			}
			if (f->state == FUTURE_WAITING) {
				fprintf(stderr, "Future already in waiting state\n");
				return SYSERR;
			}
			if(f->state == FUTURE_READY){
				*value = f->value;
				f->state = FUTURE_EMPTY;
                		return OK;
			}
			break;
		}
		case FUTURE_SHARED: {
			if(f->state == FUTURE_EMPTY || f->state == FUTURE_WAITING) {
				f->state = FUTURE_WAITING;
                		push(f->get_queue, currpid);
				suspend(currpid);
			}
			if(f->state == FUTURE_READY){
				*value = f->value;
				return OK;
			}
			break;
        	}
        	case FUTURE_QUEUE : {
			if (f->state == FUTURE_EMPTY || f->state == FUTURE_WAITING) {
				f->state = FUTURE_WAITING;
				push(f->get_queue, currpid);
				suspend(currpid);
            		}
            		if(f->state == FUTURE_READY){
				if(size(f->set_queue) == 0){
					*value = f->value;
					f->state = FUTURE_WAITING;
				} else {
                    			pid32 pid;
                    			push(f->get_queue, currpid);
                    			f->state = FUTURE_WAITING;
                    			pid = pop(f->set_queue);
                    			resume(pid);
					suspend(currpid);
					*value = f->value;
                    			if (size(f->set_queue) == 0 && size(f->get_queue) == 0) {
						f->state = FUTURE_EMPTY;
                    			}
                		}
                		return OK;      
            		}
        	}
		default: {
			return SYSERR;
		}
		return SYSERR;
	}
	return SYSERR;
}
