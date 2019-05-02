#include <xinu.h>
#include <prodcons.h>

//global variable n
int n;
//semaphores
sid32 produced, consumed;

shellcmd xsh_prodcons(int nargs, char *args[]) {
	int count = 2000;
	produced = semcreate(1);
	consumed = semcreate(0);
	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
                printf("Usage: %s <integer>\n\n",args[0]);
		printf("Usage: %s\n\n",args[0]);
                printf("Description:\n");
                printf("Takes either an integer or nothing and starts a producer consumer.\n");
                printf("Options (one per invocation):\n");
                printf("\t--help\tdisplay this help and exit\n");
                return (0);
        }
	//check args[1], if present assign value to count
	if (nargs > 2) {
		//incorrect nargs
		fprintf(stderr, "%s: too many arguments\n", args[0]);
                fprintf(stderr, "Try '%s --help' for more information\n",
                        args[0]);
                return (1);
	}
	if (nargs == 2) {
		count = atoi(args[1]);
	}
	printf("count: %d\n",count);
	resume( create(producer, 1024, 20, "producer", 1, count));
	resume( create(consumer, 1024, 20, "consumer", 1, count));
	return(0);
}
