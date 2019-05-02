#include <future_queue.h>

#ifndef _FUTURE_H_
#define _FUTURE_H_
 
/* define states */
#define FUTURE_FREE	  0x00
#define FUTURE_EMPTY 	  0x01
#define FUTURE_FULL 	  0x03
#define FUTURE_READY      0x04
#define FUTURE_WAITING    0x05

/* modes of operation for future*/
#define FUTURE_EXCLUSIVE  1
#define FUTURE_SHARED     2
#define FUTURE_QUEUE      3	

typedef struct futent
{
  uint16 state;
  uint16 flags;
  uint32 size;
  char * value; /* alloc must memget size (in chars) space */
  pid32 pid; /* for single waiter case */
  queue * set_queue;
  queue * get_queue;
} future;

/* Interface for system call */
future* future_alloc(int future_flags, uint size);
syscall future_free(future*);
syscall future_get(future*, char*);
syscall future_set(future*, char*);
 
#endif /* _FUTURE_H_ */
