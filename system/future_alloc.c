//future_alloc.c
#include <xinu.h>
#include <future.h>
#include <stdio.h>

future* future_alloc(int future_flags, uint size) {
	future * f = (future*)getmem(sizeof(future));
	if ((int32)f == SYSERR) {
		fprintf(stderr, "SYSERR: Allocating memory to future.\n");
		return NULL;
	}
	f->flags = future_flags;
	f->size = size;
	f->pid = -1;
	f->state = FUTURE_EMPTY;
	f->value = (char*)getmem(sizeof(char));
	f->set_queue = create_queue();
	f->get_queue = create_queue();
	return f;
}
