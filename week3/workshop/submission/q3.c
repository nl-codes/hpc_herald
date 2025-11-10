#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

// Upper limit: find primes in [1..MAX]
#define MAX  10000

// Structure describing a range [start, stop] for a thread to process
typedef struct {
	int start, stop;
} start_stop;

// Check if n is prime; Returns 1 if prime, 0 otherwise.
int is_prime(int n) {
	if (n < 2) return 0;
	int limit = (int)sqrt(n);
	for (int i = 2; i <= limit; i++) {
		if (n % i == 0) {
			return 0;
		}
	}
	return 1;
}

/*
	Thread routine: count primes in the assigned range.
	Allocates an int on the heap, stores the count there and returns
	the pointer via pthread_exit.
*/
void* prime_finder(void* arg) {
	int* count = (int*)malloc(sizeof(int));
	*count = 0;
	start_stop* s_s = (start_stop*)arg;
	// Iterate inclusive from start to end and increment count for primes
	for (int i = s_s->start; i <= s_s->stop; i++) {
		if (is_prime(i)) {
			(*count)++;
		}
	}
	// Return pointer to the result
	pthread_exit(count);
}
int main() {
	int threads_size;

	// Ask user for number of threads to use
	printf("Number of threads : ");
	scanf("%d", &threads_size);

	// Allocate arrays for pthread_t handles and start_stop endpoints
	pthread_t* threads = malloc(threads_size * sizeof(pthread_t));
	start_stop* range = malloc(threads_size * sizeof(start_stop));

	/* Divide the interval [1..MAX] into roughly equal blocks.
	   Last thread takes the  remainder threads */
	int block_size = MAX / threads_size;

	for (int i = 0; i < threads_size; i++) {
		/* Compute inclusive start and stop for this thread's block */
		range[i].start = (i * block_size) + 1;
		range[i].stop = (i == threads_size - 1) ? MAX : (i + 1) * block_size;

		/* Create thread to count primes in range[i] */
		pthread_create(&threads[i], NULL, prime_finder, &range[i]);
	}

	/*
	Comment to verify prime count | uncomment lines with : // C3
	*/
	// int total_prime_number = 0; // C3
	for (int i = 0; i < threads_size; i++) {
		void* retval;
		pthread_join(threads[i], &retval);
		printf("Thread %d found %d prime numbers\n", i, *(int*)retval);
		// total_prime_number += *(int*)retval; // C3
		free(retval); // free the heap-allocated count returned by thread
	}
	// Print final total and free allocated arrays
	// printf("\nAt the end, total prime numbers : %d\n", total_prime_number); // C3
	free(threads);
	free(range);

	return 0;
}
