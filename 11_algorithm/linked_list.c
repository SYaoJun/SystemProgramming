/*
Singly linked list implementation in C
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ListNode {
    struct ListNode* next;
    int              value;
} ListNode;

typedef struct List {
    ListNode*    head;
    ListNode*    tail;
    unsigned int len;
} List;

void add_node(int value, List* list) {
    ListNode* node = (ListNode*) malloc(sizeof(ListNode));
    node->next     = NULL;
    node->value    = value;
    // check if there are existing nodes in the list
    if (list->tail == NULL) {
        list->tail = list->head = node;
    } else {
        list->tail->next = node;
        list->tail       = node;
    }
    list->len++;
}

void remove_node(int value, List* list) {
    if (list == NULL) {
        return;
    }
    ListNode* cursor = list->head;
    ListNode* prev   = NULL;
    while (cursor) {
        if (cursor->value == value) {
            // check if deleting head or tail
            if (prev != NULL) {
                prev->next = cursor->next;
            } else {
                list->head = cursor->next;
            }
            if (list->tail == cursor) {
                list->tail = prev;
            }
            list->len--;
            return;
        }
        prev   = cursor;
        cursor = cursor->next;
    }
}
void iterate_list(List* list) {
    ListNode* cursor = list->head;
    while (cursor) {
        printf("value = %d\n", cursor->value);
        cursor = cursor->next;
    }
}

int main() {
    // create a list
    List* list = (List*) malloc(sizeof(List));
    list->head = NULL;
    list->tail = NULL;
    add_node(101, list);
    add_node(102, list);
    add_node(103, list);
    iterate_list(list); // 101 102 103
    remove_node(102, list);
    iterate_list(list); // 101 103
    return 0;
}
