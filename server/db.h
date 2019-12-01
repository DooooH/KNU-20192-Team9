#pragma once
#include <iostream>
#include <string>
#include <mysql.h>
#include <mutex>
#include <algorithm>
#include <map>
#include "viewer.h"

#pragma comment (lib , "libmysql.lib")

#define FILTER 8			//+= 5범위로 핑거프린트 좌표 필터링
#define FP_DISTANCE 10000	//mm단위

typedef struct _fn_info : std::unary_function<_fn_info, bool> {
	std::string ap_MAC;
	int level;
	_fn_info(std::string ap_MAC) : ap_MAC(ap_MAC) {}
	_fn_info(std::string ap_MAC, int level) : ap_MAC(ap_MAC), level(level) {}
	bool operator()(_fn_info const& m) const {
		if (ap_MAC == m.ap_MAC)
			return true;
		return false;
	}
	/*int &operator[](int index) {
		switch (index) {
		case 0: return this->up;
		case 1: return this->right;
		case 2: return this->down;
		case 3: return this->left;
		}
	}*/
}_fn_info;
typedef struct _fn_point : std::unary_function<_fn_point, bool> {
	int bid, num_floor;
	int x, y;
	std::vector <_fn_info> fn_info;
	std::vector <double> similarity;
	_fn_point(int bid, int num_floor, int x, int y) : bid(bid), num_floor(num_floor), x(x), y(y) { }
	bool operator()(_fn_point const& m) const {
		if (m.bid == bid && m.num_floor == num_floor && m.x == x && m.y == y)
			return true;
		return false;
	}
}_fn_point;
typedef struct level_info {
	std::string ap_name, ap_MAC;
	int level, freq;
}level_info;


typedef struct point {
	int x;
	int y;
}point;
typedef struct room {
	int rid;
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
	int size_z; //mm 단위
	short max_floor;
	std::vector <map> map_list;
	std::vector <ap_point> ap_list;
}building;



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
	std::string search_eql(const char* table, int args, ...);
	std::vector <_fn_point> get_fnprint_data(std::vector <std::string> MAC, std::vector <int> value);
	std::string search(const char* table, const char* column, const char* key, const char* value);
	bool exist(const char* table, int args, ...);
	bool exist(const char* table, const char* column, const char* target_val);
	bool update(const char* table, const char* target, const char* target_val, int args, ...);
	bool insert(const char* table, int args, ...);
	bool del(const char* table, const char* column, const char* target_val);
};


int DB_handle();
bool calc(std::string input, std::string &output);
std::vector<std::string> split(std::string str, char delimiter);



extern DB *database;
extern std::vector <building> list_building;
extern std::map <int, std::string> clnt_list;
extern std::mutex clnt_list_mtx;