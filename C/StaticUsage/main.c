#include <stdio.h>

void incrementGlobalVar();
void printGlobalVar();
void accessGlobalVar();

int main() {
    incrementGlobalVar();
    printGlobalVar();

    accessGlobalVar();
    printGlobalVar();

    return 0;
}
