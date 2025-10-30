#include <stdio.h>
#include "array_operation.h"

void main() {
    int arr[5];
    printf("Enter 5 integers: \n");
    for (int i = 0; i < 5; i++) {
        scanf("%d", &arr[i]);
    }

    int max_val = find_max(arr, 5);
    printf("The maximum value is: %d\n", max_val);

    sort_ascending(arr, 5);

    printf("Sorted Numbers: ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", arr[i]);
    }
}
