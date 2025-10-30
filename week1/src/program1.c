#include <stdio.h>

void main() {
    for (int n = 0; n < 10; n++) {
        if (n == 5)
            continue;
        if (n == 8)
            break;
        printf("%d\n", n);
    }
}
