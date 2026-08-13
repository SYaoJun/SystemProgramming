#include <stdio.h>  

#define N 4
int main(){
    int a[N]={0}, i = 0;
    int b[N]={0};

        while( i< N){
            //i = 3
            a[i] = b[i++]; // b[3]
            // a[1] = b[0], i=1
            printf("i = %d\n", i);
        }
    return 0;
}