#include <stdio.h>
void reverse(int a[], int n){
    for(int i = 0; i < n/2; i++){
        int temp = a[i];
        a[i] = a[n - 1 - i];
        a[n - i - 1] = temp;
    }
}
int main(){
    int a[] = {11 ,22, 3, 44, 55};
    reverse(a, 5);
    for (size_t i = 0; i < 5; i++)
    {
        printf("%d ", a[i]);
    }
    


}