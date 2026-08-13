#include <stdio.h>
#include <string.h>
#define ARRAY_LENGTH 5
#define print(a) \
    do{   \
             \
    }while(1);\

    
int main(){
    // int array[ARRAY_LENGTH];
    // 1. 
   // int array[ARRAY_LENGTH] = {0};
    // 2. 
    int array[ARRAY_LENGTH];
    
    //memset(array,0, ARRAY_LENGTH *sizeof(int));
    memset(array,0, sizeof(array));
    // 从0开始编号 从前往后
    int n = 5;
    for(int i = 0; i < n; i++){
        scanf("%d", &array[i]);
    }
    // 输出

    for(int i = 0; i < n; i++){
        printf("%d", array[i]);
    }
    printf("%d\n", array[5]);

    int a = 352;
    char* a = itoa()    
}