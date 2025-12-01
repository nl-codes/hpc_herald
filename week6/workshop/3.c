#include <stdio.h>
#include <omp.h>

int main() {
    int i, c;
    int tid;
    printf("Prime numbers between 1 and 1000 are :\n");
#pragma omp parallel for num_threads(5)
    for (i = 1; i <= 1000; i++) {
        for (c = 2; c <= i - 1; c++) {
            if (i % c == 0)
                break;
        }
        if (c == i) {
            tid = omp_get_thread_num();
            printf("%d thread found %d\t", tid, i);
        }
    }
    return 0;
}
