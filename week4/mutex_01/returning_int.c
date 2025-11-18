#include <pthread.h>
#include <stdio.h>


int gretval=0;

void *threadOne(void *p){
  gretval = 42;
  pthread_exit(&gretval);
}

void main(){
  void *pretval1;
  int retval;
  pthread_t thrID1;
  pthread_create(&thrID1, NULL, threadOne, NULL);
  pthread_join(thrID1, &pretval1);
  retval=*(int *)pretval1;
  printf("*pretval1 = %d\n", retval);
}
