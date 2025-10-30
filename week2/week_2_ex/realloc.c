#include <stdio.h>
#include <stdlib.h>

int main() {
    int n;
    printf("Enter initial number of elements: ");
    scanf("%d", &n);

    int *arr = (int*) malloc(n * sizeof(int));

    printf("Enter %d integers:\n", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
    }

    // Increase array size
    int newSize;
    printf("Enter new size: ");
    scanf("%d", &newSize);

    arr = (int*) realloc(arr, newSize * sizeof(int));

    if (arr == NULL) {
        printf("Reallocation failed!\n");
        return 1;
    }

    // Input new elements if size increased
    if (newSize > n) {
        printf("Enter %d new elements:\n", newSize - n);
        for (int i = n; i < newSize; i++) {
            scanf("%d", &arr[i]);
        }
    }

    printf("Updated array:\n");
    for (int i = 0; i < newSize; i++) {
        printf("%d ", arr[i]);
    }

    free(arr);
    return 0;
}
