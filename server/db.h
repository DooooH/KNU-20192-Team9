#pragma once
#include <iostream>
#include <string>
#include <mysql.h>
#include <mutex>
#include <algorithm>

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
	std::string search(const char* table, int args, ...);
	std::string search(const char* table, const char* column, const char* key, const char* value);
	bool exist(const char* table, const char* column, const char* target_val);
	bool update(const char* table, const char* target, const char* target_val, int args, ...);
	bool insert(const char* table, int args, ...);
	bool del(const char* table, const char* column, const char* target_val);
};
int DB_handle();
void calc(std::string input);
std::vector<std::string> split(std::string str, char delimiter);

typedef struct point {
	int x;
	int y;
}point;
typedef struct room {
	std::string rname;
	int start_x;
	int start_y;
	std::vector <point> point;
}room;
typedef struct map {
	short num_floor;
	std::vector <room> room_list;
}map;
typedef struct ap_point {
	short num_floor;
	int x, y, z;
	std::string ap_name;
	float ap_freg;
	std::string ap_MAC;
}ap_point;
typedef struct building {
	std::string bname;
	int size_x;
	int size_y;
	int size_z; //mm ¥‹¿ß
	short max_floor;
	std::vector <map> map_list;
	std::vector <ap_point> ap_list;
}building;


extern DB *database;
extern std::vector <building> list_building;
