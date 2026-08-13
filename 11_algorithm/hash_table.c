/*
1. Hash table implementation in C
key: string
value: void*
2. Compile command
gcc dict.c -g -w
3. Output
Add succeeded
Add succeeded
value = 300
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct dictEntry {
    void*             key;
    void*             val;
    struct dictEntry* next;
} dictEntry;

typedef struct dict {
    dictEntry**   table;
    unsigned long size;
    unsigned long sizemask;
    unsigned long used;
} dict;

static int dictPut(dict* ht, void* key, void* val);
static int dictDelete(dict* ht, const void* key);
// A string hash can be used here instead, or a custom encoding method to
// convert strings to integers
int hashCode(dict* ht, void* key) {
    char*         k = (char*) key;
    unsigned long h = 0;
    while (*k) {
        h               = (h << 4) + *k++;
        unsigned long g = h & 0xF0000000L;
        if (g) {
            h ^= g >> 24;
        }
        h &= ~g;
    }
    return h % ht->size;
}

static void* dictGet(dict* ht, void* key) {
    int t = hashCode(ht, key);
    // start traversing from the first valid node
    dictEntry* cursor = ht->table[t]->next;
    while (cursor) {
        if (strcmp((char*) cursor->key, (char*) key) == 0) {
            return cursor->val;
        }
        cursor = cursor->next;
    }
    return NULL;
}

static int dictPut(dict* ht, void* key, void* val) {
    // check if the key already exists
    if (dictGet(ht, key) != NULL) {
        return 0;
    }
    dictEntry* dictEntry_ = (dictEntry*) malloc(sizeof(dictEntry));
    dictEntry_->key       = key;
    dictEntry_->val       = val;
    dictEntry_->next      = NULL;
    // generate hashCode based on key
    int t              = hashCode(ht, key);
    dictEntry_->next   = ht->table[t]->next;
    ht->table[t]->next = dictEntry_;
    return 1;
}

#define N 10

void printReturnStatus(int status) {
    if (!status) {
        puts("添加失败，已存在重复元素");
    } else {
        puts("添加成功");
    }
}
int main() {
    dict* dict_ = (dict*) malloc(sizeof(dict));
    dict_->size = N;
    int i;
    dict_->table = (dictEntry**) malloc(N * sizeof(dictEntry*));
    dictEntry* dictEntry_;
    for (i = 0; i < dict_->size; i++) {
        // dummy head node
        dictEntry_      = (dictEntry*) malloc(sizeof(dictEntry));
        dictEntry_->key = dictEntry_->val = dictEntry_->next = NULL;
        dict_->table[i]                                      = dictEntry_;
    }
    char* key   = "yaojun";
    int   value = 200;
    int   ret   = dictPut(dict_, (void*) key, (void*) value);
    printReturnStatus(ret);
    char* key_   = "jason";
    int   value_ = 300;
    ret          = dictPut(dict_, (void*) key_, (void*) value_);
    printReturnStatus(ret);
    void* returnVal = dictGet(dict_, key_);
    if (returnVal == NULL) {
        puts("查询的元素不存在");
    }
    printf("value = %d\n", (int) (returnVal));
    return 0;
}
