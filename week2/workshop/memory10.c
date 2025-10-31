#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

int n = 10;
int main() {
    int i;
    int* x;
    x = malloc(sizeof(int) * n);

    printf("%ld\n", sizeof(x));
    printf("%ld\n", sizeof(*x));

    for (i = 0;i < n;i++) {
        x[i] = 2 * i;
    }

    for (i = 0;i < n;i++) {
        printf("%d,%d\n", i, x[i]);

    }

    free(x);
    return EXIT_SUCCESS;

}
