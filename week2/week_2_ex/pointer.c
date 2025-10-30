#include<stdio.h>
int main() {
    int x = 10;
    int* px = &x;

    printf("value of x: %d\n", x);
    printf("address of x: %p\n", &x);
    printf("Size of x: %zu bytes \n", sizeof(x));
    printf("size of pointer to x(&x): %zu bytes\n", sizeof(&x));

    printf("value of px (address of x): %p\n", px);
    printf("value pointed by px (*px): %d\n", *px);
    printf("Size of pointer px: %zu bytes\n", sizeof(px));
    // printf("Size of pointer to px (&px): %zu bytes\n",sizeof(&px));

    //pointer to pointer
    //pointer variable that stores the address of another pointer variable
    int** ppx = &px;
    printf("value of ppx (address of px): %p\n", ppx);
    printf("value pointed by ppx (*ppx) (address of x): %p\n", *ppx);
    printf("value pointed by value pointed by ppx (**ppx) (value of x): %d\n", **ppx);
    printf("Size of pointer ppx: %zu bytes\n", sizeof(ppx));

    *px = 20; //changing value of x using pointer px
    printf("New value of x after *px=20: %d\n", x);


    **ppx = 30; //changing value of x using pointer to pointer ppx
    printf("New value of x after **ppx=30: %d\n", x);



    return 0;
}
