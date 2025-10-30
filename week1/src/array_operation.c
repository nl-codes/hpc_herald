#include <stdio.h>
#include "array_operation.h"

int find_max(int arr[], int size)
{
    int max_num = arr[0];
    for (int i = 0; i < size; i++)
    {
        if (arr[i] > max_num)
            max_num = arr[i];
    }
    return max_num;
}
void sort_ascending(int arr[], int size)
{
    int temp;
    for (int i = 0; i < size; i++)
    {
        for (int j = i; j < size; j++)
        {
            if (arr[i] > arr[j])
            {
                temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
}
