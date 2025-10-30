#include <stdio.h>
#include <stdlib.h>

int main() {
    int arraySize;
    printf("Enter size of the array [1-50]: ");
    scanf("%d", &arraySize);
    if (arraySize > 50 || arraySize < 1) {
        printf("Please enter number between 1 to 50.");
        return 1;
    }

    int* arrayList = (int*)malloc(arraySize * sizeof(int));
    if (arrayList == NULL) {
        printf("Error on memory allocation!\n");
        return 1;
    }

    for (int i = 0; i < arraySize; i++) {
        arrayList[i] = rand();
        printf("%d => %d\n", i + 1, arrayList[i]);
    }

    free(arrayList);

    return 0;
}