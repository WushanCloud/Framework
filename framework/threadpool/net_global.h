#ifndef __NET_GLOBAL_H__
#define __NET_GLOBAL_H__

typedef struct connection {
	int fd; // 已连接套接字
	int events; // 当前的状态，EPOLLIN还是EPOLLOUT
	void (*rdhandler)(struct connection *c); // 读回调函数
	void (*wrhandler)(struct connection *c); // 写回调
	char buffer[2000];
	struct connection *data; //  相当于链表的next指针
}connection_t;

int listen_init() ; // 初始化监听套接字
int epoll_init() ;
void epoll_process() ; // 获取发生的事件


#endif //__NET_GLOBAL_H__

