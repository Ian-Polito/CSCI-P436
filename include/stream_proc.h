#include <future_queue2.h>

//semaphore declaration
extern sid32 produced, consumed;

//function prototypes
void stream_consumer(int stream_id);
void stream_producer(int stream_id);
void stream_proc(int nargs, char *args[]);

//global variables
extern int num_streams;
extern int work_queue;
extern int time_window;
extern int output_time;
extern queue * streams;
