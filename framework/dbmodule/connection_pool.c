#include "connection_pool.h"
#include <malloc.h>

void connection_pool_init(connectionpool_t *pool, char *name, char *passwd, char *db, int max_sz) {
	pool->name = name;
	pool->passwd = passwd;
	pool->db = db;
	pool->max_size = max_sz;
	pool->cur_size = 0;
	pool->head = pool->tail = NULL;
	pthread_cond_init(&(pool->cond), NULL);
	pthread_mutex_init(&(pool->mutex), NULL);
}

void connection_pool_push(connectionpool_t *pool, char *sql) {
	pthread_mutex_lock(&(pool->mutex));
	if ( pool->head != NULL ) { // 说明有空闲连接
		node_t *tmp = pool->head;
		pool->head = tmp->next;
		
		// 取下连接节点，执行任务
		pthread_mutex_unlock(&(pool->mutex));
		update(tmp->conn, sql);
		pthread_mutex_lock(&(pool->mutex));
		
		tmp->next = pool->head;
		pool->head = tmp;
		pthread_cond_signal(&(pool->cond));
	} else if ( pool->cur_size < pool->max_size ) { // 可以创建连接
		node_t *tmp = (node_t*)malloc(sizeof(node_t));
		connection(&(tmp->conn), pool->name, pool->passwd, pool->db);

		// 新连接，直接执行sql
		pthread_mutex_unlock(&(pool->mutex));
		update(tmp->conn, sql);
		pthread_mutex_lock(&(pool->mutex));
		
		// 再插入连接队列
		tmp->next = pool->head;
		pool->head = tmp;
		pool->cur_size++;
		pthread_cond_signal(&(pool->cond));
	}  else { // 不可创建新的连接，只能等待
		while ( pool->head == NULL ) {
			pthread_cond_wait(&(pool->cond), &(pool->mutex));
		}

		node_t *tmp = pool->head;
		pool->head = tmp->next;
		
		// 取下连接节点，执行任务
		pthread_mutex_unlock(&(pool->mutex));
		update(tmp->conn, sql);
		pthread_mutex_lock(&(pool->mutex));
		
		tmp->next = pool->head;
		pool->head = tmp;
		pthread_cond_signal(&(pool->cond));
	}
	pthread_mutex_unlock(&(pool->mutex));
}

