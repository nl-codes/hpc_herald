#include <stdio.h>

struct t {
    unsigned  int  h;
    unsigned  int  m;
    unsigned  int  s;
};

int main() {
    struct t a;
    struct t* b;

    a.h = 5;
    a.m = 9;
    a.s = 45;

    printf("Time a is %u:%02u:%02u\n", a.h, a.m, a.s);

    b = &a;
    printf("Time b is %u:%02u:%02u\n", b->h, b->m, b->s);
    return 0;
}
