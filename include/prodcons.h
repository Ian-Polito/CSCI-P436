//global variable for prod/cons
extern int n;
//semaphore declaration
extern sid32 produced, consumed;

//function prototypes
void consumer(int count);
void producer(int count);
