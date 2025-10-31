#include <stdio.h>

int main() {
    int arr[] = { 10, 20, 30, 40, 50 };
    int* ptr = arr;   // Pointer points to the first element of the array
    int n = sizeof(arr) / sizeof(arr[0]);
    printf("Address of arr: %p\n Address of arr[0]: %p\n", arr, &arr[0]);

    printf("Array elements using pointer arithmetic:\n");

    // Access array elements using pointer arithmetic
    for (int i = 0; i < n; i++) {
        printf("Element %d: Value = %d, Address = %p\n", i, *(ptr + i), (ptr + i));
    }

    printf("\nPointer Increment and Decrement:\n");
    printf("Pointer initially points to arr[0]: %p (Value = %d)\n", ptr, *ptr);

    ptr++;  // Move pointer to next element (arr[1])
    printf("After ptr++: %p (Value = %d)\n", ptr, *ptr);

    ptr += 2; // Move pointer forward by 2 elements (to arr[3])
    printf("After ptr += 2: %p (Value = %d)\n", ptr, *ptr);

    ptr--; // Move pointer backward by 1 element (to arr[2])
    printf("After ptr--: %p (Value = %d)\n", ptr, *ptr);

    printf("\nPointer Difference:\n");
    int* start = &arr[0];
    int* end = &arr[4];
    printf("Start = %p, End = %p\n", start, end);
    printf("Number of elements between start and end = %lld\n", end - start);

    return 0;
}
