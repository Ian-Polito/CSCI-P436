#ifndef _FUTURE_PRODCONS_H
#define FUTURE_PRODCONS_H

#include <xinu.h>
#include <future.h>

//future producer consumer function prototypes
uint future_prod(future * fut, char n);
uint future_cons(future * fut);


#endif
