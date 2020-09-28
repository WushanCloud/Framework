#include "../include/config.h"
#include "../include/list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static list_t* list;

// 加载配置文件，在程序启动时就应该加载配置信息到内存中
void load(const char* conf_name)
{
    // 空名字直接返回
    if (conf_name == NULL) return;
    
    // 打开对应文件
    FILE* fp = fopen(conf_name, "r");
    if (fp == NULL) return;

    // 读取配置信息
    char line_buf[501];
    while (!feof(fp)){
        memset(line_buf, '\0', sizeof(line_buf));
        
        // 读取一行数据
        if (fgets(line_buf, 500, fp) == NULL) continue;

        // 读取检测
        if (line_buf[0] == '\0') continue;
        if (line_buf[0] == '\r' ||
            line_buf[0] == '\n' ||
            line_buf[0] == '\t' ||
            line_buf[0] == '#'  ||
            line_buf[0] == ' '  ||
            line_buf[0] == '['  )
            continue;
        
        // 读取处理
        list = list_init();
        // 去除行尾空格
        while(line_buf[strlen(line_buf)-1] == ' '  ||
              line_buf[strlen(line_buf)-1] == '\t' ||
              line_buf[strlen(line_buf)-1] == '\n' ){
            line_buf[strlen(line_buf)-1] = '\0';
        }
        char* pfind = strchr(line_buf, '=');
        if (pfind != NULL){
            char name[500] = {0};
            char value[500] = {0};
            strncpy(name, line_buf, (size_t)(pfind-line_buf));
            strcpy(value, pfind+1);
        // 去除等号两边空格
            myrtrim(name);
            myltrim(value);

            list_insert(list, name, value);
        }
    }

}

// 读取字符串类型的配置项
const char* get_string(const char* key)
{
    node_t* fnode = list->head;
    while (fnode != NULL){
        if (strcmp(fnode->key, key)) {
            return fnode->value;
        }
        fnode = fnode->next;
    }
    return NULL;
}

// 读取整数类型的配置项
int get_int_default(const char* key, const int def)
{
    int ret = def;

    node_t* fnode = list->head;
    while (fnode != NULL) {
        if (strcmp(fnode->key, key)) {
            ret = atoi(fnode->value);
        }
        fnode = fnode->next;
    }
    return ret;
}



// 清除字符串左边的空格
void myltrim(char* str)
{
    if (str == NULL) return;
    
    char* pstr1 = str;
    if (*pstr1 != ' ')
        return;
    char* pstr2 = str;
    while (*pstr2 == ' '){
        pstr2++;
    }
    while (*pstr2 != '\0'){
        *pstr1++ = *pstr2++;
    }
}

// 清除字符串右边的空格
void myrtrim(char* str)
{
    if (str == NULL) return;
    int len = strlen(str);
    
    while (len > 0 && str[len-1] == ' '){
        str[--len] = '\0';
    }
}

