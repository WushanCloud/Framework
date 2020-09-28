#ifndef __MYSIGNAL_H__
#define __MYSIGNAL_H__
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// 信号结构体
typedef struct {
	int signo;             // 信号的值 SIGINT
	char* signame;         // 信号的名字"SIGINT"
	char* name;            // 命令的名字
	void  (*handler)(int signo, siginfo_t* info, void* ucontext); // 信号处理函数 
}signal_t;


void  sig_handler(int signo, siginfo_t* info, void* ucontext);


int init_signal();
#endif // __MYSIGNAL_H__
