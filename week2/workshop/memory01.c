#include <stdio.h>
#include <stdlib.h>

int main() {
    int x = 123;
    long long int y = 321;
    printf("%d  %ld\n", x, sizeof(x));
    printf("%lld  %lld\n", y, sizeof(y));
    return EXIT_SUCCESS;

}
