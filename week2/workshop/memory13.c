#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

int n = 10;
void inc(int* w) {
    int i;
    for (i = 0;i < n;i++) {
        *w = *w + 1;
        w++;
    }

}
int main() {
    int i;
    int* x, * y;
    x = malloc(sizeof(int) * n);
    y = x;

    for (i = 0;i < n;i++) {
        *y = 2 * i;
        y++;

    }

    inc(x);
    y = x;

    for (i = 0;i < n;i++) {
        printf("%d,%d\n", i, *y);
        y++;

    }

    free(x);
    return EXIT_SUCCESS;

}
