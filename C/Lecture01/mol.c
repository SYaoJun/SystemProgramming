#include<stdio.h>
#include<math.h>
int main(){
    double Q = 6.0 / 32.0 * 6.02 * pow(10.0, 23.0);
    const double MOL = 6.02e+23;
    double Q2 = 6.0 / 32.0 * MOL;
    printf("%.3e\n", Q2);
    printf("%5e\n", Q);// minimal width
    printf("%.5g\n", Q);
    return 0;
}