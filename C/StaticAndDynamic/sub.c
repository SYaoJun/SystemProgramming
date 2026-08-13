#include "sub.h"
#include "add.h"
int sub(int a, int b){
    add(a, b);
    return a - b;
}