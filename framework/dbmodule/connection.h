#ifndef __CONNECTIONT_H__
#define __CONNECTIONT_H__

#include <mysql/mysql.h>

// 连接数据库
int connection(MYSQL **my, char *name, char *passwd, char *db);

// 插入，删除 
int update(MYSQL *my, char *sql);

// 查询
MYSQL_RES *query(MYSQL *my, char *sql);

#endif // __CONNECTIONT_H__

