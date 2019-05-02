#include <future_prodcons.h>

uint future_prod(future * fut, char n) {
  printf("Produced %d\n",n);
  future_set(fut, &n);
  return OK;
}

uint future_cons(future * fut) {
  char i;
  int status;
  status = (int)future_get(fut, &i);
  if (status < 1) {
    printf("future_get failed\n");
    return -1;
  }
  printf("Consumed %d\n", i);
  return OK;
}
