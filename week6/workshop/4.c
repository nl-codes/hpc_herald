#include <stdio.h>
#include <omp.h>

int run_parallel_count(int num_threads) {
    int i, c;
    int tid;
    int total_prime_count = 0;
#pragma omp parallel for num_threads(num_threads) default(none) private(i, c, tid) reduction(+:total_prime_count)
    for (i = 1; i <= 1000; i++) {
        for (c = 2; c <= i - 1; c++) {
            if (i % c == 0)
                break;
        }
        if (c == i) {
            tid = omp_get_thread_num();
            // printf("%d thread found %d\n", tid, i);
            total_prime_count++;
        }
    }
    return total_prime_count;
}
int main() {
    int five_threads = 5, one_thread = 1;
    int five_thread_prime_count = 0, one_thread_prime_count = 0;

    five_thread_prime_count = run_parallel_count(five_threads);
    one_thread_prime_count = run_parallel_count(one_thread);

    printf("Total prime numbers found using 5 threads: %d", five_thread_prime_count);
    printf("\nTotal prime numbers found using 1 thread: %d", one_thread_prime_count);
    return 0;
}
