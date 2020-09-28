#include "../include/mysignal.h"

void  sig_handler(int signo, siginfo_t* info, void* ucontext) {
	printf("recv %d\n", signo);
}

signal_t sig_arr[] = {
        {SIGHUP,    "SIGHUP",    NULL, sig_handler},
        {SIGINT,    "SIGINT",    NULL, sig_handler},
        {SIGQUIT,   "SIGQUIT",   NULL, sig_handler},
        {SIGCHLD,   "SIGCHLD",   NULL, sig_handler},
        {SIGTERM,   "SIGTERM",   NULL, sig_handler},
        {SIGSYS,    "SIGSYS",    NULL, NULL},

        //  .... 
        {0, NULL, NULL, NULL}
};


// 信号的初始化
int init_signal() {
	signal_t* sig = NULL;
	struct sigaction act;

	for (sig = sig_arr; sig->signo != 0; sig++) {
		memset(&act, 0x00, sizeof(struct sigaction));
		if (sig->handler == NULL) {
			act.sa_sigaction = (void (*)(int, siginfo_t*, void*))(SIG_IGN);
		}
	else {
			act.sa_sigaction = sig_handler;
			act.sa_flags = SA_SIGINFO;
		}
		sigemptyset(&act.sa_mask);
		sigaction(sig->signo, &act, NULL);
	}
    return 0;
}

