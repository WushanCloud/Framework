#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/global.h"

extern char** environ;

/*
* 设置进程名相关。
*/

//  给环境变量搬家
void init_proctitle() {
	int i;

	// 求出环境变量的大小
	for (i = 0; environ[i] != NULL; i++) {
		g_envmemlen += strlen(environ[i]) + 1;
	}

	// 给环境变量分配空间
	g_envmem = malloc(sizeof(char) * g_envmemlen);
	memset(g_envmem, 0x00, g_envmemlen);

	char* tmp = g_envmem;
	for (i = 0; environ[i] != NULL; i++) {
		int len = strlen(environ[i]) + 1;
		strcpy(tmp, environ[i]);
		environ[i] = tmp; // 环境变量搬到新的位置，environ也要更新到新位置
		tmp += len;
	}
}

// 设置进程标题
void set_proctitle(const char* name) {
	// 标题长度
	int title_len = strlen(name);
	int arg_len = 0;
	int i;

	// 求出命令行参数空间的大小
	for (i = 0; g_os_argv[i] != NULL; i++) {
		arg_len += strlen(g_os_argv[i]) + 1;
	}

	// 名字太长，放不下
	if (arg_len + g_envmemlen < title_len)
		return;

	g_os_argv[1] = NULL;
	char* tmp = g_os_argv[0];

	// 设置新标题
	strcpy(tmp, name);

	// 将后面空闲的位置清零
	tmp += title_len;
	memset(tmp, 0x00, (arg_len + g_envmemlen) - title_len);
}
