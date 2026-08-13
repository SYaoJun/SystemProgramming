#include <stdio.h>

// Pointer type for comparison function
typedef int (*compare_func_t)(int, int);

// Swap two integer values
void swap(int* a, int* b) {
    int temp = *a;
    *a       = *b;
    *b       = temp;
}

// Bubble sort function
void bubbleSort(int arr[], int n, compare_func_t compare) {
    int i, j;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (compare(arr[j], arr[j + 1])) {
                swap(&arr[j], &arr[j + 1]);
            }
        }
    }
}

// Ascending comparison function
int ascendingCompare(int a, int b) {
    return a > b;
}

// Descending comparison function
int descendingCompare(int a, int b) {
    return a < b;
}

// Test example
int main() {
    int arr[] = { 64, 34, 25, 12, 22, 11, 90 };
    int n     = sizeof(arr) / sizeof(arr[0]);

    printf("升序排序结果: ");
    bubbleSort(arr, n, ascendingCompare);
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    printf("降序排序结果: ");
    bubbleSort(arr, n, descendingCompare);
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    return 0;
}
