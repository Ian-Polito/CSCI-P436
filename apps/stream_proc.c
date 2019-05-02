#include <xinu.h>
#include <stream_proc.h>
#include <stdlib.h>

//extern vars
sid32 produced, consumed;
int num_streams;
int work_queue;
int time_window;
int output_time;
queue * streams;

void stream_proc(int nargs, char *args[]) {
	produced = semcreate(1);
	consumed = semcreate(0);
	char usage[] = "Usage: -s num_streams -w work_queue -t time_window -o output_time\n";
	int i;
	if (!(nargs % 2)) {
		printf("%s",usage);
		return (-1);
	} else {
		i = nargs - 1;
		while (i > 0) {
			char * str = args[i-1];
			char c = *(++str);
			switch (c) {
				case 's':
					num_streams = atoi(args[i]);
					streams = create_queue();
					break;
				case 'w':
					work_queue = atoi(args[i]);
					break;
				case 't':
					time_window = atoi(args[i]);
					break;
				case 'o':
					output_time = atoi(args[i]);
					break;
				default:
					printf("%s",usage);
					return (-1);
			}
			i -= 2;
		}
	}
	i = 0;
	while (i < num_streams) {
		resume( create(stream_producer, 1024, 20, "st_producer", 1, i));
        	resume( create(stream_consumer, 1024, 20, "st_consumer", 1, i));
		i++;
	}
}

