#include <stdio.h>
void my_func(char *p){
    printf("p = %p, sizeof = %ld\n", p, sizeof(p));
    printf("p+1 = %p\n", p+1);
    printf("p+2 = %p\n", p+2);
    printf("p+3 = %p\n", p+3);
}
int main(){
    char str[4] = "abc";

    int n = 4;
    for(int i = 0; i < n; i++){
        printf("i = %p\n", str+i);
    }
/*
i = 0x16ce8e828
i = 0x16ce8e829
i = 0x16ce8e82a
i = 0x16ce8e82b
p = 0x16b13e828, sizeof = 8
p+1 = 0x16b13e829
p+2 = 0x16b13e82a
p+3 = 0x16b13e82b
*/
    my_func(str);
    return 0;
}