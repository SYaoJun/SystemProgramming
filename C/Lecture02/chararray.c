#include<stdio.h>
int main(){

    //char array[] = "Hello";
    //printf("%s\n", array);
    //scanf("%5s", array);
    //printf("%s\n", array);
    // TODO: 最多存6个字符，不包括'\0'，多余的字符直接丢弃
    char array2[][6] = {"Hello", "Worldxxyz"};
    // TODO: 
    printf("%s, %s\n", array2[0], array2[1]);
    // 这里为什么没有检查读取的空间是否足够容纳输入的字符串？
    scanf("%s", array2[1]);
    //scanf("%c", &array2[1][1]);
    
    printf("%s, %s\n", array2[0], array2[1]);
    return 0;

}