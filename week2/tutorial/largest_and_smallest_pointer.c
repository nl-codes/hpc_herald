#include <stdio.h>

// Function to find minimum and maximum in an array
void findArrayMinMax(int arr[], int size, int *min, int *max) {
    *min = *max = arr[0]; // Initialize both to first element

    for (int i = 1; i < size; i++) {
        if (arr[i] < *min)
            *min = arr[i];
        if (arr[i] > *max)
            *max = arr[i];
    }
}

int main() {
    int n;

    printf("Enter number of elements: ");
    scanf("%d", &n);

    int arr[n];

    printf("Enter %d integers: ", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
    }

    int minVal, maxVal;

    // Function call
    findArrayMinMax(arr, n, &minVal, &maxVal);

    printf("Minimum value = %d\n", minVal);
    printf("Maximum value = %d\n", maxVal);

    return 0;
}

