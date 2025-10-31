#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

int n = 10;

int main() {
    int i;
    int* x, * y;
    x = malloc(sizeof(int) * n);
    y = x;

    for (i = 0;i < n;i++) {
        *y = 2 * i;
        y++;
    }

    y = x;

    for (i = 0;i < n;i++) {
        printf("%d,%d\n", i, *y);
        y++;
    }

    free(x);
    return EXIT_SUCCESS;
}
