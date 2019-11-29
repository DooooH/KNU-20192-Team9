#include "db.h"


DB* database;

DB::DB() : connection(NULL) {};
DB::DB(const char* host, const char* user, const char* passwd, const char* db) {
	mysql_init(&conn);
	if ((connection = mysql_real_connect(&conn, host, user, passwd, db, 3306, (char*)NULL, 0)) == NULL) {
		std::cout << "Mysql 에러 : " << mysql_error(&conn) << std::endl;
		class_stat = false;
		return;
	}

	if (mysql_select_db(&conn, db)) {
		std::cout << mysql_errno(&conn) << " 에러 : " << mysql_error(&conn) << std::endl;
		class_stat = false;
		return;
	}
	mysql_query(connection, "set session character_set_connection=euckr;");
	mysql_query(connection, "set session character_set_results=euckr;");
	mysql_query(connection, "set session character_set_client=euckr;");
	class_stat = true;
}
bool DB::exit_db() {
	if (class_stat) {
		mysql_close(connection);
		return true;
	}
	return false;
}
/*
MYSQL_RES sql_query(const char* query) {
	if ((query_stat = mysql_query(connection, query)) != 0) {
		std::cout << "Mysql 에러 : " << mysql_error(&conn) << std::endl;
		return *NULL;
	}
}*/
std::string DB::search(const char* table, const char* key, const char* value) {
	std::string tmp = "select *from ", result_str;
	tmp = tmp + table + " where " + key + "=" + "'" + value + "';";

	std::lock_guard<std::mutex> guard(DB_mtx);
	if ((query_stat = mysql_query(connection, tmp.c_str())) != 0) {
		std::cout << "Mysql 에러 : " << mysql_error(&conn) << std::endl;
		DB_mtx.unlock();
		return NULL;
	}
	if ((sql_result = mysql_store_result(connection)) == NULL) {
		std::cout << "Mysql 에러 : " << mysql_error(&conn) << std::endl;
		DB_mtx.unlock();
		return NULL;
	}
	int num_fields = mysql_num_fields(sql_result);
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		for (int i = 0; i < num_fields; i++)
			result_str = result_str + sql_row[i] + " ";

		/*
		for (int i = 0; i < num_fields; i++)
			std::cout << sql_row[i] << " ";
		std::cout << std::endl;*/
	}
	mysql_free_result(sql_result);
	DB_mtx.unlock();

	return result_str;
}
std::string DB::search(const char* table, const char* column, const char* key, const char* value) {
	std::string tmp = "select ", result_str;
	tmp = tmp + column + " from " + table + " where " + key + "=" + "'" + value + "';";

	std::lock_guard<std::mutex> guard(DB_mtx);
	if ((query_stat = mysql_query(connection, tmp.c_str())) != 0) {
		std::cout << "Mysql 에러 : " << mysql_error(&conn) << std::endl;
		DB_mtx.unlock();
		return NULL;
	}
	sql_result = mysql_store_result(connection);

	int num_fields = mysql_num_fields(sql_result);
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {

		for (int i = 0; i < num_fields; i++)
			result_str = result_str + sql_row[i] + " ";
	}
	mysql_free_result(sql_result);
	DB_mtx.unlock();

	return result_str;
}
bool DB::update(const char* table, const char* target, const char* target_val, const char* column, const char* sub) {
	std::string tmp = "update ";
	tmp = tmp + table + " set " + column + "=" + sub + " where " + target + "=" + target_val;
	std::lock_guard<std::mutex> guard(DB_mtx);
	if ((query_stat = mysql_query(connection, tmp.c_str())) != 0) {
		std::cout << "Mysql 에러 : " << mysql_error(&conn) << std::endl;
		DB_mtx.unlock();
		return false;
	}
	DB_mtx.unlock();
	return true;
}
bool DB::insert(const char* table, int args, ...) {
	std::string tmp = "insert into ";
	tmp = tmp + table + " values (";
	va_list ap;
	va_start(ap, args);
	for (int i = 0; i < args - 1; i++) {
		tmp = tmp + va_arg(ap, const char*) + ", ";
	}
	tmp = tmp + va_arg(ap, const char*) + ");";
	va_end(ap);
	//std::cout << tmp;
	std::lock_guard<std::mutex> guard(DB_mtx);
	if ((query_stat = mysql_query(connection, tmp.c_str())) != 0) {
		std::cout << "Mysql 에러 : " << mysql_error(&conn) << std::endl;
		DB_mtx.unlock();
		return false;
	}
	DB_mtx.unlock();
	return true;
}


int DB_handle() {
	std::string tmp;
	database = &DB("127.0.0.1", "latter2005", "opentime4132@", "project");
	if (!database->class_stat) {
		return 0;
	}

	tmp = database->search("employees", "name", "aaa");
	database->insert("employees", 3, "5", "'eee'", "'9C-B9-D0-20-55-A1'");
	std::cout << tmp;
	//std::cout << "mysql client version : " << mysql_get_client_info() << std::endl;
	database->exit_db();
	return 1;
}