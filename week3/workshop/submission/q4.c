#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define MAX 10000

int prime_counts = 0;
pthread_mutex_t lock;

typedef struct {
	int start, stop;
	pthread_t* threads;
	int total_threads;
} start_stop;

// Check if n is prime
int is_prime(int n) {
	if (n < 2) return 0;
	int limit = (int)sqrt(n);
	for (int i = 2; i <= limit; i++)
		if (n % i == 0)
			return 0;
	return 1;
}

// Thread function
void* print_five_primes(void* arg) {
	start_stop* s_s = (start_stop*)arg;

	for (int i = s_s->start; i <= s_s->stop; i++) {
		pthread_testcancel(); // cancellation checkpoint

		if (is_prime(i)) {
			pthread_mutex_lock(&lock);

			if (prime_counts < 5) {
				prime_counts++;
				printf("%d. Thread %ld found %d\n", prime_counts, pthread_self(), i);
			}
			// If 5th prime found, cancel all other threads and exit
			if (prime_counts >= 5) {
				for (int t = 0; t < s_s->total_threads; t++) {
					if (!pthread_equal(s_s->threads[t], pthread_self())) {
						pthread_cancel(s_s->threads[t]);
					}
				}
				pthread_mutex_unlock(&lock);
				pthread_exit(NULL);
			}
			pthread_mutex_unlock(&lock);
		}
	}
	return NULL;
}

int main() {
	int threads_size;
	printf("Number of threads: ");
	scanf("%d", &threads_size);

	pthread_t* threads = malloc(threads_size * sizeof(pthread_t));
	start_stop* range = malloc(threads_size * sizeof(start_stop));

	int block_size = MAX / threads_size;

	pthread_mutex_init(&lock, NULL);

	for (int i = 0; i < threads_size; i++) {
		range[i].start = i * block_size + 1;
		range[i].stop = (i == threads_size - 1) ? MAX : (i + 1) * block_size;
		range[i].threads = threads;
		range[i].total_threads = threads_size;
		pthread_create(&threads[i], NULL, print_five_primes, &range[i]);
	}

	for (int i = 0; i < threads_size; i++)
		pthread_join(threads[i], NULL);

	pthread_mutex_destroy(&lock);
	free(threads);
	free(range);
	return 0;
}
