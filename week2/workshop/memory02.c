#include <stdio.h>
#include <stdlib.h>

int inc(int w) {
    return w + 1;
}

int main() {
    int x = 123;
    int y = inc(x);
    printf("%d,%d\n", x, y);
    return EXIT_SUCCESS;

}
