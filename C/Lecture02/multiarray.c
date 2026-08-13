#include<stdio.h>
#define ROW 3
#define COL 4
int main(){
    int a[ROW][COL] = {
        {0,1,2,3},
        {1,2,3,4},
        {2,3,4,5}
    };

    for (int i = 0; i < ROW; i++){
        for(int j = 0; j < COL; j++){

            //a[i][j]
            printf("&a[%d][%d] = %p, a[%d][%d] = %d \n", 
            i, j, &a[i][j], i, j, a[i][j]);
        }
        printf("\n");
    }
    printf("%d\n", sizeof(a) );
    printf("%d\n", sizeof(a[0]));
    printf("%d\n", sizeof(a[0][0]));
    //int *p = a[0]+1;
    //printf("%p, %d\n", p, *p);
    //printf("%p, %d\n", q, *q);

    //int b[][2] = {1,2,3,4,5,6,8};
    return 0;
}