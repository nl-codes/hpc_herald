#include <stdio.h>
#include <omp.h>

int main() {
    int i, c;
    printf("Prime numbers between 1 and 1000 are :\n");
#pragma omp parallel for
    for (i = 1; i <= 1000; i++) {
        for (c = 2; c <= i - 1; c++) {
            if (i % c == 0)
                break;
        }
        if (c == i)
            printf("%d\t", i);
    }
    return 0;
}
