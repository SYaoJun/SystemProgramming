#include<stdlib.h>
#include <stdio.h>
typedef struct node{
    int val;
}node;


void init(node **p){ // 此处传递的是指针的地址，即0x16ee52820
    *p = (node *)malloc(sizeof(node)); // 这里表示对0x16ee52820这个地址中的内容进行操作赋值，原来是NULL，现在是有效的堆内存地址
    printf("init, p content = %p\n", *p);
    (*p)->val = 10;
}
int main(){
    node *p = NULL;
    printf("p = %p\n", p);
    init(&p);
    printf("p = %p, val = %d\n", p, p->val);
    return 0;
}
/*
p = 0x0
init, p content = 0x13cf04080
p = 0x13cf04080, val = 10
*/