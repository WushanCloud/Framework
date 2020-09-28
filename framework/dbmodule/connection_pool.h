#ifndef __CONNECTION_POLL_H__
#define __CONNECTION_POLL_H__

#include <pthread.h>
#include "connection.h"

typedef struct node {
	MYSQL *conn;
	struct node *next;
}node_t;

typedef struct connectionpool {
	char *name;
	char *passwd;
	char *db;
	int max_size;
	int cur_size;
	node_t *head;
	node_t *tail;
	pthread_cond_t cond;
	pthread_mutex_t mutex;
}connectionpool_t;

void connection_pool_init(connectionpool_t *pool, char *name, char *passwd, char *db, int max_sz);
void connection_pool_push(connectionpool_t *pool, char *sql);

#endif // __CONNECTION_POLL_H__

