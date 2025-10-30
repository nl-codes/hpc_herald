#include <stdio.h>

void swap(int* a, int* b) {
    int temp;
    temp = *b;
    *b = *a;
    *a = temp;
}

int main() {
    int num1 = 1, num2 = 2;

    printf("Before Swapping: num1 = %d and num2 = %d\n", num1, num2);
    swap(&num1, &num2);
    printf("After Swapping : num1 = %d and num2 = %d", num1, num2);

    return 0;
}

