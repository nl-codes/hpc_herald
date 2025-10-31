#include <stdio.h>
#include <stdlib.h>

struct pair {
    int a;
    int b;

};
int main() {
    struct pair x;
    x.a = 12;
    x.b = 34;
    printf("%d,%d,%ld\n", x.a, x.b, sizeof(struct pair));
    return EXIT_SUCCESS;

}
