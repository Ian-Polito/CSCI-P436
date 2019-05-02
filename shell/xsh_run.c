/* xsh_run.c - xsh_run */

#include <xinu.h>
#include <string.h>
#include <stdio.h>

//function prototypes
shellcmd xsh_hello(int nargs, char *args[]);
shellcmd xsh_prodcons(int nargs, char *args[]);
void stream_proc(int nargs, char *args[]);
uint32 future_test(int nargs, char *args[]);

//xsh_run - take a string and run corresponding function

shellcmd xsh_run(int nargs, char *args[]) {
	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
                printf("Usage: %s <string> <arguments>\n\n",args[0]);
                printf("Description:\n");
                printf("Takes a string and runs corresponding function with given arguments\n");
                printf("Options (one per invocation):\n");
                printf("\t--help\tdisplay this help and exit\n");
		printf("\tlist\tprint all functions supported and exit\n");
                return (0);
        }
	if ((nargs == 2) && (strncmp(args[1], "list", 5) == 0)) {
      		printf("prodcons\n");
      		printf("hello\n");
		printf("stream_proc\n");
		printf("future_test\n");
		printf("stream_proc_future\n");
      		return OK;
    	}
	
	//this will go past "run" and pass the function/process name and its arguments
	args++;
	nargs--;
	
	if (strncmp(args[0], "prodcons", 8) == 0) {
		//xsh_prodcons(nargs, args);
		resume (create((void *)xsh_prodcons, 4096, 20, "prodcons", 2, nargs, args));
	}
	else if (strncmp(args[0], "hello", 5) == 0) {
                //xsh_hello(nargs, args);
                resume (create((void *)xsh_hello, 4096, 20, "hello", 2, nargs, args));
        }
	else if (strncmp(args[0], "stream_proc", 11) == 0) {
		//stream_proc(nargs, args);
		resume (create((void *)stream_proc, 4096, 20, "stream_proc", 2, nargs, args));
	}
	else if (strncmp(args[0], "future_test", 11) == 0) {
		//start futures
		future_test(nargs, args);
	}
	else if (strncmp(args[0], "stream_proc_future", 11) == 0) {
                //start futures
                stream_proc_future(nargs, args);
        } else {
		//unknown function
		printf("Unknown function: %s\n", args[0]);
	}
}
