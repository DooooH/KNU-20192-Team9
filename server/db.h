#pragma once
#include <iostream>
#include <string>
#include <mysql.h>
#include <mutex>
#pragma once

#pragma comment (lib , "libmysql.lib")


class DB {
private:
	MYSQL * connection, conn;
	MYSQL_RES *sql_result;
	MYSQL_ROW sql_row;
	int query_stat;
	std::mutex DB_mtx;

public:
	int class_stat;
	DB();
	DB(const char* host, const char* user, const char* passwd, const char* db);
	bool exit_db();
	std::string search(const char* table, const char* key, const char* value);
	std::string search(const char* table, const char* column, const char* key, const char* value);
	bool update(const char* table, const char* target, const char* target_val, const char* column, const char* sub);
	bool insert(const char* table, int args, ...);

};
int DB_handle();

extern DB *database;
