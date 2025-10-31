#include <stdlib.h>
#include <stdio.h>

int main() {
    int a = 2;
    int b = 3;
    int c = 2;
    int d = 4;

    printf("There are no booleans in c\n");
    printf("%d\n", a == b);
    printf("%d\n", a == c);
    printf("%d\n", a != b);
    printf("%d\n", a != c);
    printf("%d\n", a == b);
    printf("%d\n", !(a == b));

    int e = (a == b) || (a == c);
    int f = (a == b) && (a == c);

    printf("e=%d\n", e);
    printf("f=%d\n", f);

    if (e) {
        printf("e=true\n");

    } else {
        printf("e=false\n");

    }

    if (f) {
        printf("f=true\n");

    } else {
        printf("f=false\n");

    }

    return EXIT_SUCCESS;

}
