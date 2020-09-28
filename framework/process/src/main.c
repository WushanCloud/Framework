#include "../include/config.h"
#include "../include/global.h"
#include "../include/mysignal.h"
#include "../include/process.h"
#include <unistd.h>

extern char **environ;

char **g_os_argv; // 命令行参数的起始位置
int g_envmemlen;  // 环境变量的空间大小
char *g_envmem;   // 环境变量

int main( int argc, char *argv[] ) {
	g_os_argv = argv;
	load("my.conf"); // 加载配置文件
	init_proctitle(); // 环境变量搬家
	init_signal(); //  初始化信号处理

	// 如果要以守护进程的方式运行，就调用set_daemon();
	if ( get_int_default("Daemon", 0) == 1 ) 
		set_daemon();

	master_process_cycle(); // 进入主循环
}

