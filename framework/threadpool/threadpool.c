#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "threadpool.h"

// 初始化线程池
void threadpool_init(threadpool_t *pool, int max_thread) {
	pthread_cond_init(&(pool->cond), NULL);
	pthread_mutex_init(&(pool->mutex), NULL);
	pool->head = NULL;
	pool->tail = NULL;
	pool->max_thread = max_thread;
	pool->counter    = 0;
	pool->idle       = 0;
	pool->quit       = 0;
}

void *routine(void *arg) {
	threadpool_t *pool = (threadpool_t*)arg;

	while ( 1 ) {
		pthread_mutex_lock(&(pool->mutex));
		pool->idle++;

		while ( pool->head==NULL && pool->quit == 0 ) {
			pthread_cond_wait(&(pool->cond), &(pool->mutex));
		}
		pool->idle--;

		if ( pool->head != NULL ) {
			node_t *tmp = pool->head;
			pool->head = tmp->next;
			
			// 执行回调函数可能要的时间比较长，导致其他线程不能操作
			// 并且任务节点已经从任务队列去掉，所有解锁不会有问题
			pthread_mutex_unlock(&(pool->mutex));
			tmp->callback(tmp->args);
			free(tmp);
			pthread_mutex_lock(&(pool->mutex));
		}

		if ( pool->quit == 1 && pool->head == NULL ) {
			pool->counter--;
			if ( pool-> counter == 0 ) {
				pthread_cond_signal(&(pool->cond));
			}
			pthread_mutex_unlock(&(pool->mutex));
			break;
		}

		pthread_mutex_unlock(&(pool->mutex));
	}
    return NULL;
}

// 往线程池中添加任务
//  将task放入任务队列
//  如果有空闲线程，就直接回调这个任务
//  如果没有，并且当前的线程数没有达到最大值，就创建新的线程来执行任务
void threadpool_add_task(threadpool_t *pool, void *(*task)(void*), void *arg) {
	node_t *newnode = (node_t*)malloc(sizeof(node_t));
	memset(newnode, 0x00, sizeof(node_t));
	
	newnode->callback = task;
	newnode->args     = arg;
	
	pthread_mutex_lock(&(pool->mutex));

	if ( pool->head == NULL ) {
		pool->head = newnode;
	} else {
		pool->tail->next = newnode;
	}
	pool->tail = newnode;

	if ( pool->idle > 0 ) {
		pthread_cond_signal(&(pool->cond));
	} else if ( pool->counter < pool->max_thread ) {
		pthread_t tid;
		pthread_create(&tid, NULL, routine, (void*)pool);
		pthread_detach(tid);
		pool->counter++;
	} else {
		// 达到上限，只能等待
	}

	pthread_mutex_unlock(&(pool->mutex));
}

// 销毁线程池
void threadpool_destroy(threadpool_t *pool) {
	if ( pool->quit == 1 )
		return;
	pthread_mutex_lock(&(pool->mutex));
	pool->quit = 1;
	if ( pool->counter > 0 ) {
		if ( pool->idle > 0 ) { //  当空闲线程大于0,才去通知
			pthread_cond_broadcast(&(pool->cond));
		}
		// 阻塞在这里，直到所有线程池中的线程销毁，才离开
		while ( pool->counter > 0 )
			pthread_cond_wait(&(pool->cond), &(pool->mutex));
	}

	pthread_mutex_unlock(&(pool->mutex));
	pthread_cond_destroy(&(pool->cond));
	pthread_mutex_destroy(&(pool->mutex));
}

