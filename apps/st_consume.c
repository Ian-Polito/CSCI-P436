#include <xinu.h>
#include <stream_proc.h>

void stream_consumer(int stream_id) {
	char * m;
	int i;
	int stream_size = size(streams);
	for (i = 0; i <= 11; i++) {
		wait(produced);
		m = pop(streams);
		//print stuff about consumed measurement
		printf("Consumed Measurement: ID:%d %s\n",stream_id,m);
		signal(consumed);
	}
}

