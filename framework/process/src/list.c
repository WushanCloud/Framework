#include "../include/list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 初始化链表
list_t* list_init()
{
    list_t* plist = (list_t*)malloc(sizeof(list_t));
    memset(plist, '\0', sizeof(list_t));

    plist->head = NULL;
    plist->list_lenth = 0;
    
    return plist;
}

// 链表中插入数据
int list_insert(list_t* list, const char* key, const char * value)
{
    node_t* newnode = (node_t*)malloc(sizeof(node_t));
    memset(newnode, '\0', sizeof(node_t));

    newnode->key = (char*)malloc(strlen(key)+1);
    strncpy(newnode->key, key, strlen(key));
    newnode->value = (char*)malloc(strlen(value)+1);
    strncpy(newnode->value, value, strlen(value));
    newnode->next = NULL;

    if (list->head == NULL){
        list->head = newnode;
    }
    else{// 前插法
        newnode->next = list->head;
        list->head = newnode;
    }
    list->list_lenth++;
    return 1;
}
