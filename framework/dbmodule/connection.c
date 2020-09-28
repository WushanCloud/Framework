#include <stdio.h>
#include <stdlib.h>

#include "connection.h"

// 连接数据库
int connection(MYSQL **my, char *name, char *passwd, char *db) {
	*my = mysql_init(NULL);
	mysql_real_connect(*my, "localhost", name, passwd, db, 0, NULL, 0);
    return 0;
}

// 插入，删除 
int update(MYSQL *my, char *sql) {
	if ( mysql_query(my, sql) )
		printf("mysql_query : %s\n", mysql_error(my));
	return 0;
}

// 查询
MYSQL_RES *query(MYSQL *my, char *sql) {
	mysql_query(my, sql);
	return mysql_store_result(my);
}

