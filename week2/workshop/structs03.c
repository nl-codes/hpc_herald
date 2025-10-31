#include <stdio.h>
#include <malloc.h>

typedef struct {
    unsigned int h;
    unsigned int m;
    unsigned int s;

} t;
int main() {
    t* a;

    a = (t*)malloc(sizeof(t));

    a->h = 5;
    a->m = 9;
    a->s = 45;

    printf("Time a is %u:%02u:%02u\n", a->h, a->m, a->s);

    free(a);
    return 0;

}
