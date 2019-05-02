#include <xinu.h>
#include <stream_proc.h>
#include "tscdf-input.h"

void stream_producer(int stream_id) {
	//measurement * m;
	//m = (measurement*)getmem(sizeof(measurement));
	char m[80];
	int i;
	char * a;
	int st;
	int ts;
	int v;
	for (i = 0; i < 11; i++) {
		wait(consumed);
		a = (char*)stream_input[i];
		st = atoi(a);
		while (*a++ != '\t');
		ts = atoi(a);
		while (*a++ != '\t');
		v = atoi(a);
		if (st == stream_id) {
			sprintf(m,"Time: %d, Val: %d",ts,v);
			//m->timestamp = ts;
			//m->value = v;
			push(streams,m);
		}
		signal(produced);
	}
}
