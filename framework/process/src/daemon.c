#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int set_daemon() {
    // 创建子进程
	switch ( fork() ) {
		case 0:
			break;
		default:
			exit(0);
	}
    // 让进程完全独立出来
	if ( setsid() == -1 ) 
		perror("setsid"),exit(1);
    // 设置文件权限掩码
	umask(0);
    // 修改工作目录
	chdir("/");
    // 重定向文件描述符
	int fd = open("/dev/null", O_RDWR);
	dup2(fd, STDIN_FILENO);
	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);
	
	return 0;
}
