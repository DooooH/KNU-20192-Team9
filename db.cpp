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

bool DB::is_mac(std::string mac) { //xx-xx-xx-xx-xx-xx 17자리
	if (mac.length() != 17)
		return false;
	else {
		register int pos;
		for (pos = 0; pos < 15; pos += 3) {
			if (!isalnum(mac[pos]) || !isalnum(mac[pos+1]))
				return false;
			if (mac[pos+2] != '-')
				return false;
		}
		if (!isalnum(mac[pos]) || !isalnum(mac[pos + 1]))
			return false;
	}
	return true;
}


/*
MYSQL_RES sql_query(const char* query) {
	if ((query_stat = mysql_query(connection, query)) != 0) {
		std::cout << "Mysql 에러 : " << mysql_error(&conn) << std::endl;
		return *NULL;
	}
}*/
std::string DB::get_all_data(const char* table) {
	std::string tmp = "select *from ", result_str;
	tmp = tmp + table;
	std::lock_guard<std::mutex> guard(DB_mtx);
	if ((query_stat = mysql_query(connection, tmp.c_str())) != 0) {
		std::cout << "Mysql 에러 : " << mysql_error(&conn) << std::endl;
		return NULL;
	}
	sql_result = mysql_store_result(connection);

	int num_fields = mysql_num_fields(sql_result);
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		int i;
		for (i = 0; i < num_fields-1; i++)
			result_str = result_str + sql_row[i] + " ";
		result_str = result_str + sql_row[i] + "\n";
	}
	mysql_free_result(sql_result);
	return result_str;
}
std::string DB::search(const char* table, const char* key, const char* value) {
	std::string tmp = "select *from ", result_str;
	tmp = tmp + table + " where " + key + "=" + "'" + value + "';";
	std::lock_guard<std::mutex> guard(DB_mtx);
	if ((query_stat = mysql_query(connection, tmp.c_str())) != 0) {
		std::cout << "Mysql 에러 : " << mysql_error(&conn) << std::endl;
		return NULL;
	}
	if ((sql_result = mysql_store_result(connection)) == NULL) {
		std::cout << "Mysql 에러 : " << mysql_error(&conn) << std::endl;
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
	//DB_mtx.unlock();
	return result_str;
	//return result_str;
}
std::string DB::search(const char* table, const char* column, const char* key, const char* value) {
	std::string tmp = "select ", result_str;
	tmp = tmp + column + " from " + table + " where " + key + "=" + "'" + value + "';";

	std::lock_guard<std::mutex> guard(DB_mtx);
	if ((query_stat = mysql_query(connection, tmp.c_str())) != 0) {
		std::cout << "Mysql 에러 : " << mysql_error(&conn) << std::endl;
		return NULL;
	}
	sql_result = mysql_store_result(connection);

	int num_fields = mysql_num_fields(sql_result);
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {

		for (int i = 0; i < num_fields; i++)
			result_str = result_str + sql_row[i] + " ";
	}
	mysql_free_result(sql_result);
	//DB_mtx.unlock();

	return result_str;
}
bool DB::exist(const char* table, const char* column, const char* target_val) {
	std::string tmp = "select EXISTS(select * from ";
	tmp = tmp + table + " where " + column + "=" + target_val+") as success";
	std::lock_guard<std::mutex> guard(DB_mtx);
	if ((query_stat = mysql_query(connection, tmp.c_str())) != 0) {
		std::cout << "Mysql 에러 : " << mysql_error(&conn) << std::endl;
		return NULL;
	}
	sql_result = mysql_store_result(connection);
	sql_row = mysql_fetch_row(sql_result);
	mysql_free_result(sql_result);
	if (atoi(*sql_row)) 
		return true;
	else 
		return false;
}
bool DB::update(const char* table, const char* target, const char* target_val, int args, ...) {
	std::string tmp = "update ";
	tmp = tmp + table + " set ";
	va_list ap;
	va_start(ap, args);
	for (int i = 0; i < args - 1; i++) {
		tmp = tmp + va_arg(ap, const char*) + "=" + va_arg(ap, const char*) + ", "; //케릭터 형태로 받는게 오버헤드 적음
	}
	tmp = tmp + va_arg(ap, const char*) + "=" + va_arg(ap, const char*);
	va_end(ap);
	tmp = tmp + " where " + target + "=" + target_val + ";";
	std::lock_guard<std::mutex> guard(DB_mtx);
	if ((query_stat = mysql_query(connection, tmp.c_str())) != 0) {
		std::cout << "Mysql 에러 : " << mysql_error(&conn) << std::endl;
		return false;
	}
	//DB_mtx.unlock();
	return true;
}
bool DB::insert(const char* table, int args, ...) {
	std::string tmp = "insert into ";
	tmp = tmp + table + " values (";
	va_list ap;
	va_start(ap, args);
	for (int i = 0; i < args - 1; i++) {
		tmp = tmp + va_arg(ap, std::string ) + ", "; //원래 형태가 스트링
	}
	tmp = tmp + va_arg(ap, std::string) + ");";
	va_end(ap);
	//std::cout << tmp;
	std::lock_guard<std::mutex> guard(DB_mtx);
	if ((query_stat = mysql_query(connection, tmp.c_str())) != 0) {
		std::cout << "Mysql 에러 : " << mysql_error(&conn) << std::endl;
		return false;
	}
	//DB_mtx.unlock();
	return true;
}
bool DB::del(const char* table, const char* column, const char* target_val) {
	std::string tmp = "delete from ";
	tmp = tmp + table + " where " + column + "=" + target_val;
	std::lock_guard<std::mutex> guard(DB_mtx);
	if ((query_stat = mysql_query(connection, tmp.c_str())) != 0) {
		std::cout << "Mysql 에러 : " << mysql_error(&conn) << std::endl;
		return false;
	}
	return true;
}


#include <conio.h>
int DB_handle() {

	database = &DB("127.0.0.1", "latter2005", "opentime4132@", "project");
	while (!database->class_stat) {
		std::cout << "press any key to restart db" << std::endl;
		_getch();
		database = &DB("127.0.0.1", "latter2005", "opentime4132@", "project");
	}

	init_view();
	
	//database->del("employees", "name", "'eee'");
	//std::cout << tmp;
	//std::cout << "mysql client version : " << mysql_get_client_info() << std::endl;
	database->exit_db();
	std::cout << "db헨들 종료" << std::endl;
	return 1;
}


/*
	nana::form fm;

	//Define a label and display a text.
	nana::label lab{ fm, "Hello, <bold blue size=16>Nana C++ Library</>" };
	lab.format(true);

	//Define a button and answer the click event.
	nana::button btn{ fm, "Quit" };
	btn.events().click([&fm] {
		fm.close();
	});
	nana::label lab{fm, "Hello, <bold blue size=16>Database </>" };
	lab.format(true);
	fm.events().click([&fm] {
		nana::form fm_modal(fm);
		fm_modal.modality();
	});
	nana::button btn{ fm, "Quit" };
	fm.caption(("Server"));

	fm.div("vert <><<><weight=80% text><>><><weight=24<><button><>><>");
	fm["text"] << lab;
	fm["button"] << btn;
	fm.collocate();

	//Layout management
	fm.div("vert <><<><weight=80% text><>><><weight=24<><button><>><>");
	fm["text"] << lab;
	fm["button"] << btn;
	fm.collocate();

	//Show the form
	fm.show();

	//Start to event loop process, it blocks until the form is closed.
	nana::exec();
	std::cout << "Hello World!\n";
	*/