#include <stdio.h>
#include <omp.h>

int main() {
#pragma omp parallel num_threads(10)
    printf("Hello world from OpenMP!\n");
    return 0;
}
