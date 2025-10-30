#include<stdio.h>
int main()
{
    int m=3,n=100, *p;
    p = &m;
    printf("Value of m: %d\n",*p);
    m++;
    printf("Value of m after m++: %d\n",*p);
    
    p = &n;
    printf("Value of n: %d\n",*p);
    *p = *p + 50;
    printf("Value of n after *p = *p + 50: %d\n",*p);
}