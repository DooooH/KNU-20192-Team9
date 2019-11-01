#pragma once
#include <iostream>
#include <string>
#include <mysql.h>
#include <mutex>
#include "viewer.h"

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
	static bool is_mac(std::string mac);
	std::string get_all_data(const char* table);
	std::string search(const char* table, const char* key, const char* value);
	std::string search(const char* table, const char* column, const char* key, const char* value);
	bool exist(const char* table, const char* column, const char* target_val);
	bool update(const char* table, const char* target, const char* target_val, int args, ...);
	bool insert(const char* table, int args, ...);
	bool del(const char* table, const char* column, const char* target_val);
};
int DB_handle();

extern DB *database;
