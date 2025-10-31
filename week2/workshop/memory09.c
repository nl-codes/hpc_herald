#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

typedef struct {
    int a;
    int b;

} pair;
void inc(pair* w) {
    w->a = w->a + 1;
    w->b = w->b + 1;

}

int main() {
    pair* x;
    x = malloc(sizeof(pair));
    x->a = 12;
    x->b = 34;
    inc(x);
    printf("%d,%d\n", x->a, x->b);
    free(x);
    return EXIT_SUCCESS;

}
