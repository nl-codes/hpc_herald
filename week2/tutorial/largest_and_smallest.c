#include <stdio.h>
int minVal, maxVal;

// Function to find minimum and maximum in an array
void findArrayMinMax(int arr[], int size) {
    minVal = maxVal = arr[0]; // Initialize both to first element

    for (int i = 1; i < size; i++) {
        if (arr[i] < minVal)
            minVal = arr[i];
        if (arr[i] > maxVal)
            maxVal = arr[i];
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

    // Function call
    findArrayMinMax(arr, n);

    printf("Minimum value = %d\n", minVal);
    printf("Maximum value = %d\n", maxVal);

    return 0;
}
