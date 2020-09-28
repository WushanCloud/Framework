#ifndef __THREADPOLL_H__
#define __THREADPOLL_H__

#include <pthread.h>

// 任务节点
typedef struct node {
	void *(*callback)(void*args);   // 任务回调函数
	void *args;                     // 传递的参数
	struct node *next;
} node_t;

// 线程池结构体
typedef struct {
	pthread_cond_t cond;
	pthread_mutex_t mutex;
	node_t *head;   // 任务队列的队头
	node_t *tail;   // 队尾
	int max_thread; // 线程池最多允许创建的线程个数
	int counter;    // 线程池中当前的线程个数
	int idle;       // 空闲线程的个数
	int quit;       // 如果为1表示要销毁线程池
}threadpool_t;

// 初始化线程池
void threadpool_init(threadpool_t *pool, int max_thread);

// 往线程池中添加任务
void threadpool_add_task(threadpool_t *pool, void *(task)(void*), void *arg);

// 销毁线程池
void threadpool_destroy(threadpool_t *pool);

#endif //__THREADPOLL_H__

