//future_free.c
#include <xinu.h>
#include <future.h>
#include <stdio.h>

syscall future_free(future * f) {
	if (f != NULL) {
                free_queue(f->get_queue);
                free_queue(f->set_queue);
                return freemem((char*)f, sizeof(future));
        } else {
                return SYSERR;
        }
}
