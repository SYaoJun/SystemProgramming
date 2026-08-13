#include <stdio.h>

// 比较函数的指针类型
typedef int (*compare_func_t)(int, int);

// 交换两个整数的值
void swap(int* a, int* b) {
    int temp = *a;
    *a       = *b;
    *b       = temp;
}

// 冒泡排序函数
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

// 升序比较函数
int ascendingCompare(int a, int b) {
    return a > b;
}

// 降序比较函数
int descendingCompare(int a, int b) {
    return a < b;
}

// 测试示例
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
