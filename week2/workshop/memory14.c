#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

int n = 10;
void initialise(int* w) {
    int i;
    for (i = 0;i < n;i++) {
        *w = 2 * i;
        w++;
    }
}
void inc(int* w) {
    int i;
    for (i = 0;i < n;i++) {
        *w = *w + 1;
        w++;
    }
}
void output(int* w) {
    int i;
    for (i = 0;i < n;i++) {
        printf("%d,%d\n", i, w[i]);
    }
}
int main() {
    int* x;
    x = malloc(sizeof(int) * n);

    initialise(x);
    inc(x);
    output(x);
    free(x);
    return EXIT_SUCCESS;
}
