#include <xinu.h>
#include <stdlib.h>
#include <string.h>
#include <future.h>
#include <future_prodcons.h>
#include <stream_proc.h>

uint32 future_ring(future *in, future *out);
int ffib(int n);
future **fibfut;
char zero = 0;
char one = 1;

//assignment 5 function
uint32 stream_proc_future(int nargs, char *args[]) {
	
}

/**
 * Test Futures
 */
uint32 future_test(int nargs, char *args[])
{
  int ring = 0;
  int future_flags = 2;
  int ring_count = 10;
  int fib = -1;
  int final_val;
  int i;

#ifndef NFUTURE
  printf("no support for futures (NFUTURES not defined.)\n");
  return OK;
#endif
  if (nargs > 3) {
	printf("Incorrect arguments!\n");
	return SYSERR;
  }
  if (nargs == 3 && strncmp(args[1], "-r", 2) == 0) {
    ring = 1;
    printf("Producer/consumer process ring\n");
  } else if (nargs == 2 && strncmp(args[1], "-pc", 3) == 0) {
    future * f_exclusive;
    future * f_shared;
    future * f_queue;

    f_exclusive = future_alloc(FUTURE_EXCLUSIVE, sizeof(int));
    f_shared    = future_alloc(FUTURE_SHARED, sizeof(int));
    f_queue     = future_alloc(FUTURE_QUEUE, sizeof(int));

    // Test FUTURE_EXCLUSIVE
    resume( create(future_cons, 1024, 20, "fcons1", 1, f_exclusive) );
    resume( create(future_prod, 1024, 20, "fprod1", 2, f_exclusive, 1) );

    // Test FUTURE_SHARED
    resume( create(future_cons, 1024, 20, "fcons2", 1, f_shared) );
    resume( create(future_cons, 1024, 20, "fcons3", 1, f_shared) );
    resume( create(future_cons, 1024, 20, "fcons4", 1, f_shared) ); 
    resume( create(future_cons, 1024, 20, "fcons5", 1, f_shared) );
    resume( create(future_prod, 1024, 20, "fprod2", 2, f_shared, 2) );

    // Test FUTURE_QUEUE
    resume( create(future_cons, 1024, 20, "fcons6", 1, f_queue) );
    resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue) );
    resume( create(future_cons, 1024, 20, "fcons8", 1, f_queue) );
    resume( create(future_cons, 1024, 20, "fcons9", 1, f_queue) );
    resume( create(future_prod, 1024, 20, "fprod3", 2, f_queue, 3) );
    resume( create(future_prod, 1024, 20, "fprod4", 2, f_queue, 4) );
    resume( create(future_prod, 1024, 20, "fprod5", 2, f_queue, 5) );
    resume( create(future_prod, 1024, 20, "fprod6", 2, f_queue, 6) );
  } else if (nargs == 3 && strncmp(args[1], "-f", 2) == 0) {
    //FIBONACCI NUMBER
    fib = atoi(args[2]);
  } else {
    printf("No valid options\n");
    return(OK);
  }

  if (ring == 1) {
    future *first, *in, *out = NULL;
    first = future_alloc(future_flags, sizeof(int));
    in = first;
    for (i=0; i < ring_count; i++) {
      out = future_alloc(future_flags, sizeof(int));
      resume( create(future_ring, 1024, 20, "", 2, in, out) );
      in = out;
    }
    printf("master sets %d\n", ring_count);
    future_set(first, (char *)&ring_count);
    future_get(out, (char *)&final_val);
    printf("master gets %d\n", final_val);
    return(OK);
  }
  
  if (fib > -1) {
    char final_fib;

    /* create the array of future pointers */
    if ((fibfut = (future **)getmem(sizeof(future *) * (fib + 1))) == (future **)SYSERR) {
      printf("getmem failed\n");
      return(SYSERR);
    }
    
    /* get futures for the future array */
    for (i=0; i <= fib; i++) {
      if((fibfut[i] = future_alloc(future_flags, sizeof(int))) == (future *)SYSERR) {
        printf("future_alloc failed\n");
        return(SYSERR);
      }
    }
    
    //spawn fib threads and get final value 
    //TODO - you need to add your code here
    
    int N = atoi(args[2]);
    int z;
    char snum[5];
    char base[10];
    printf("Futures Fibonacci for N = %d\n", N);
    for (z = 0; z <= N; z++) {
      strcpy(base, "ffib");
      sprintf(snum, "%d", z);
      strncat(base, snum, 3);
      resume( create(ffib, 1024, 20, base, 1, z) );
    }
    
    future_get(fibfut[fib], &final_fib);
    for (i=0; i <= fib; i++) {
      future_free(fibfut[i]);
    }
    freemem((char *)fibfut, sizeof(future *) * (fib + 1));    
    printf("Nth Fibonacci value for N=%d is %d\n", fib, final_fib);
    return(OK);
  }
  return(OK);
}

uint future_ring(future *in, future *out) {
  int val;
  future_get(in, (char *)&val);
  printf("Process %d gets %d, puts %d\n", getpid(), val, val-1);
  val--;
  future_free(in);
  future_set(out, (char *)&val);
  return OK;
}

int ffib(int n) {
  char minus1 = 0;
  char minus2 = 0;
  char this = 0;
  if (n == 0) {
    future_set(fibfut[0], &zero);
    return OK;
  }
  if (n == 1) {
    future_set(fibfut[1], &one);
    return OK;
  }
  future_get(fibfut[n-2], &minus2);
  future_get(fibfut[n-1], &minus1);
  this = minus1 + minus2;
  //printf("minus2: %d, minus1: %d, this: %d\n",minus2,minus1,this);
  future_set(fibfut[n], &this);
  return(0);
}
