/* xsh_hello.c - xsh_hello */

#include <xinu.h>
#include <string.h>
#include <stdio.h>

//xsh_hello - take a string a print hello <string> welcome to xinu

shellcmd xsh_hello(int nargs, char *args[]) {
	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Usage: %s <string>\n\n",args[0]);
		printf("Description:\n");
		printf("Takes a string and prints:\n");
		printf("Hello <string>, Welcome to the world of Xinu!!\n");
		printf("Options (one per invocation):\n");
		printf("\t--help\tdisplay this help and exit\n");
		return (0);
	}
	//check argument count
	if (nargs > 2) {
		//incorrect num of args
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
			args[0]);
		return (1);
	}
	if (nargs < 2) {
		//incorrect num of args
		fprintf(stderr, "%s: too few arguments\n", args[0]);
		fprintf(stderr, "try '%s --help' for more information\n",
			args[0]);
		return (1);
	}
	printf("Hello %s, Welcome to the world of Xinu!!\n", args[1]);
	return (0);
}
