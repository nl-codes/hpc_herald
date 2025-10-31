#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int a;
    int b;

}pair;
int main() {
    pair x;
    x.a = 12;
    x.b = 34;
    printf("%d,%d,%ld\n", x.a, x.b, sizeof(pair));
    return EXIT_SUCCESS;

}
