#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "net_global.h"
#include "threadpool.h"

#define CONNSIZE 1024
int lfd  = 0;      // 监听套接字
int epfd = 0;      // epoll句柄
threadpool_t pool; // 线程池
connection_t connections[CONNSIZE];   // 连接池
connection_t *free_connection = NULL; // 连接池中空闲连接的头
struct epoll_event evts[CONNSIZE];    //  epoll_wait的数组

void event_accept(connection_t *oc);
void read_request_handler(connection_t *oc);
void write_request_handler(connection_t *oc);
void epoll_add_event(int fd, int rdev, int wrev, int evtype, connection_t *c);

// 初始化监听套接字
int listen_init() {
	lfd = socket(AF_INET, SOCK_STREAM, 0);
	int op = 1;
	setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op));
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9898); // 端口，应该从配置文件中读取
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int r = bind(lfd, (struct sockaddr*)&addr, sizeof(addr));
	if ( r == -1 ) perror("bind"),exit(1);
	listen(lfd, SOMAXCONN);

	return lfd;
}

// 从连接池中获取一个空闲连接
connection_t *get_connection(int fd) {
	connection_t *c = free_connection;
	free_connection = c->data;
	c->fd = fd;
	return c;
}

// 关闭一个连接
void close_connection(connection_t *c) {
	close(c->fd);
	c->fd = -1;
	c->data = free_connection;
	free_connection = c;
}

int epoll_init() {
	epfd = epoll_create1(EPOLL_CLOEXEC);
	threadpool_init(&pool, 2); // 线程池中线程的个数,应该从配置文件中读取
	int i = CONNSIZE;

	connection_t *c = connections; // 抓住连接池
	connection_t *next  = NULL;   //  当前节点的下一个节点
	do { // 对连接池的初始化
		i--;
		c[i].data = next;
		c[i].fd   = -1;
		next = &c[i];
	} while ( i );
	free_connection = next;
	c = get_connection(lfd); // 给监听套接字分配连接池节点
	c->rdhandler = event_accept; // 监听套接字对应的回调函数

				  //描述符 读  写
	epoll_add_event(c->fd,  1, 0, EPOLL_CTL_ADD, c);
    return 0;
}

void event_accept(connection_t *oc) {
	int newfd = accept(oc->fd, NULL, NULL);   // 接收客户端到来
	connection_t *nc = get_connection(newfd); // 分配连接池
	nc->rdhandler = read_request_handler;     // 读回调
	nc->wrhandler = write_request_handler;    // 写回调函数
	
				  //描述符 读  写
	epoll_add_event(nc->fd,  1, 0, EPOLL_CTL_ADD, nc);
}

void epoll_add_event(int fd, int rdev, int wrev, int evtype, connection_t *c) {
	struct epoll_event ev;
	if ( evtype == EPOLL_CTL_MOD ) {
		epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);
		ev.data.ptr = c;
		ev.events = c->events;
		if ( c->events == EPOLLIN ) {
			ev.events = EPOLLOUT;
			c->events = EPOLLOUT;
		} else if (c->events == EPOLLOUT ) {
			ev.events = EPOLLIN;
			c->events = EPOLLIN;
		}
		epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
	} else {
		if ( rdev == 1 ) {
			ev.events = EPOLLIN;
			c->events = EPOLLIN;
		} else if ( wrev == 1 ) {
			ev.events = EPOLLOUT;
			c->events = EPOLLOUT;
		}
		ev.data.ptr = c;
		epoll_ctl(epfd, evtype, fd, &ev);
	}
}

void *run(void *arg) {
	connection_t *c = (connection*)arg;
	// 业务处理
	int i;
	for (i=0; c->buffer[i]; i++) {
		if ( c->buffer[i]>='a' && c->buffer[i]<='z' )
			c->buffer[i] = c->buffer[i]-32;
	}

	epoll_add_event(c->fd, 0, 0, EPOLL_CTL_MOD, c);
    return NULL;
}

void read_request_handler(connection_t *c) {
	int r = read(c->fd, c->buffer, 2000);
	if ( r <= 0 ) {
		epoll_ctl(epfd, EPOLL_CTL_DEL, c->fd, 0); // 对方关闭,从红黑树删除
		close_connection(c); // 归还连接池
	}
	// 将任务放入到线程池去处理
	threadpool_add_task(&pool, run, c);
}

void write_request_handler(connection_t *c) {
	write(c->fd, c->buffer, strlen(c->buffer));
	// 发送完毕后，就应该转成监控epollin事件
	epoll_add_event(c->fd, 0, 0, EPOLL_CTL_MOD, c);
}

// 获取发生的事件
void epoll_process() {
	for ( ; ; ) {
		int ready = epoll_wait(epfd, evts, CONNSIZE, -1);
		int i;
		connection_t *c = NULL;
		for (i=0; i<ready; i++) {
			int revents = evts[i].events;
			c = (connection_t*)evts[i].data.ptr;
			if ( revents & EPOLLIN ) {
				(c->rdhandler)(c);
			}
			if ( revents & EPOLLOUT ) {
				(c->wrhandler)(c);
			}
		}
	}
}

