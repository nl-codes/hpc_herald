/*
🧠 1️⃣ Syntax of pthread_create()
int pthread_create(pthread_t *thread,
                   const pthread_attr_t *attr,
                   void *(*start_routine)(void *),
                   void *arg)

⚙️ 2️⃣ Argument Details
pthread_t *thread ---->             Pointer to a pthread_t variable where the function will store the thread ID of the newly created thread.
const pthread_attr_t *attr ---->	Pointer to thread attributes (like stack size, scheduling, etc.). Usually set to NULL for default attributes.
void *(*start_routine)(void *) --->	Pointer to the function that the new thread will execute.
                                    👉 The function must take a void* argument and return a void*.
void *arg --->                  	Argument passed to the thread function. It can be NULL or a pointer to any data the thread needs.            

⚙️ 3️⃣ Return Value    
Returns 0 on success, or an positive error number on failure on failure. 

*/


#include <pthread.h>
#include <stdio.h>
#include <unistd.h> // for sleep()

void* foo(void* arg) {
    printf("Thread is running...\n");
    sleep(2);
    printf("Thread finished!\n");
    return "Done";   // thread returns this value
 }
int main() {
    
    // Create a pthread_t variable to store
    // thread ID
    pthread_t thread1;
    void* status;
    
    // Creating a new thread. 
    pthread_create(&thread1, NULL, foo, NULL);

    // Waiting for the created thread to terminate

    pthread_join(thread1, &status); // Waits for 'thread1' to finish and gets its status

    // pthread_join(thread1, NULL);    // Waits for 'thread1' to finish but ignores its status
    printf("Thread returned: %s\n", (char*)status);

    return 0;
}


/*
pthread_join(pthread_t thread, void **retval);
Arguments:
thread: This is the identifier (pthread_t) of the thread to wait for. The thread ID is returned by pthread_create() when 
        the thread is created.​
retval: This is a pointer to a location where the exit status (the return value from the thread function or the argument 
        passed to pthread_exit()) of the joined thread will be stored. This argument can be set to NULL if you do not 
        need the exit value.

Return Value:
On success, pthread_join() returns 0. On failure, it returns an positive error number indicating the type of error that occurred.
*/


