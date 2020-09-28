#define _GNU_SOURCE  // 放在第一行
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sched.h>		// CPU调度用的头文件
#include "../include/process.h"


// 主进程创建子进程，然后停在这个函数中
void master_process_cycle() {
	sigset_t set;		
	sigemptyset(&set);

	sigaddset(&set, SIGHUP);
	sigaddset(&set, SIGINT);
	sigaddset(&set, SIGQUIT);
	sigaddset(&set, SIGABRT);
	sigaddset(&set, SIGABRT);
	sigaddset(&set, SIGBUS);
	sigaddset(&set, SIGUSR1);
	sigaddset(&set, SIGUSR2);
	sigaddset(&set, SIGSEGV);
	sigaddset(&set, SIGPIPE);
	sigaddset(&set, SIGALRM);
	sigaddset(&set, SIGTERM);
	sigaddset(&set, SIGCHLD);
	sigaddset(&set, SIGTTIN);
	sigaddset(&set, SIGTTOU);
	sigaddset(&set, SIGURG);
	sigaddset(&set, SIGWINCH);
	sigaddset(&set, SIGIO);

	// 在创建子进程之前，要屏蔽上面信号，避免在创建子进程期间被打断
	sigprocmask(SIG_BLOCK, &set, NULL);

	int worker_process_num = get_int_default("WorkerProc", 1);
	start_worker_process(worker_process_num, -1); // -1表示新创建子进程, 子进程就在这个函数中，不会出来了

	// 父进程
	// 清空信号屏蔽
	sigemptyset(&set);
	// 设置主进程的名字
	set_proctitle("framework: master");
	for (; ; ) {
		sigsuspend(&set); // 阻塞在这里，直到有信号到来，才被唤醒
		printf("sigsuspend reutrn\n");
	}
}

// 真正创建子进程
void spawn_process(int inum, const char* procname, int type) {
	pid_t pid = fork();
	switch (pid) {
	case -1:
		perror("fork"), exit(1);
	case 0: // 子进程
		worker_process_cycle(inum, procname); //  子进程在这个函数中循环
		break;
	default: // 父进程
		break;
	}
}

// nums : 要创建的子进程个数
// type : -1表示新创建子进程
void start_worker_process(int nums, int type) {
	int i;

	for (i = 0; i < nums; i++) {
        // 开始创建子进程
		spawn_process(i, "framework: worker process", type);
	}
}

void worker_init(int inum) {
	// 子进程要解除信号屏蔽, 可以接收信号
	sigset_t set;
	sigemptyset(&set);
	sigprocmask(SIG_SETMASK, &set, NULL);

	// 设置CPU亲和/亲缘
	cpu_set_t cpu;
	CPU_ZERO(&cpu);	// 清空 
	int cpu_cnt = get_int_default("CPUCNT", 1);	// 配置文件中获取cpu个数
	CPU_SET(inum%cpu_cnt, &cpu);	// 将指定的cpu放到cpu集中
	sched_setaffinity(0, sizeof(cpu), &cpu);	// 绑定进程到cpu
}

// 子进程执行函数
void worker_process_cycle(int inum, const char *procname) {
	worker_init(inum); // 子进程的初始化
	set_proctitle(procname); // 设置子进程的标题
	for ( ; ; ) {
		// 将来这个地方是阻塞在epoll_wait()
		sleep(1);
		printf("worker process %d\n", inum);
	}
}

