#include<stdio.h>

int* findMid(int arr[], int size){
    return &arr[size/2];
}
int main(){
    int a[] = {10,20,30,40,50};
    int n = sizeof(a)/sizeof(a[0]);

    int *mid = findMid(a, n);
    printf("Middle element: %d\n", *mid);
    return 0;
}

