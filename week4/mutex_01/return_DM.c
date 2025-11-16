#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *threadOne(void *p){
  int *retval;
  retval= malloc(sizeof(int));
  *retval = 42;
  pthread_exit(retval);
}

void main(){
  void *pretval;
  int retval;
  pthread_t thrID1;
  pthread_create(&thrID1, NULL, threadOne, NULL);
  pthread_join(thrID1, &pretval);
  retval=*(int *)pretval;
  printf("retval = %d\n", retval);
  free(pretval);
}
