#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>

void *threadOne(void *p) {
  // Allocate memory for the string to be returned
  char *retval = malloc(20 * sizeof(char));
  if (retval == NULL) {
    perror("malloc failed");
    pthread_exit(NULL);
  }

  // Copy the string into the allocated memory
  strcpy(retval, "Hello from thread");
  
  // Exit the thread, returning the string
  pthread_exit(retval);
}


int main() {
  void *pretval1;
  pthread_t thrID1;

  // Create the thread
  pthread_create(&thrID1, NULL, threadOne, NULL);

  // Wait for the thread to finish and retrieve the return value
  pthread_join(thrID1, &pretval1);

  // Print the returned string
  printf("Returned string: %s\n", (char *)pretval1);

  // Free the allocated memory
  free(pretval1);

  return 0;
}