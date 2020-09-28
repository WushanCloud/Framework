#ifndef __CONFIG_H__
#define __CONFIG_H__


// 加载配置文件，在程序启动时就应该加载配置信息到内存中
void load(const char* conf_name);

// 读取字符串类型的配置项
const char* get_string(const char* key);

// 读取整数类型的配置项
int get_int_default(const char* key, const int def);




// 其他辅助函数

void myltrim(char* str);
void myrtrim(char* str);

#endif  //__CONFIG_H__

