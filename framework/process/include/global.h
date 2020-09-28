#ifndef __GLOBAL_H__
#define __GLOBAL_H__

extern char** g_os_argv; // 命令行参数的起始位置
extern int g_envmemlen;  // 环境变量的空间大小
extern char* g_envmem;   // 环境变量

void init_proctitle();
void set_proctitle(const char* name);

#endif // __GLOBAL_H__
