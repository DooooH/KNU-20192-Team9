#include "db.h"


DB* database;

DB::DB() : connection(NULL) {};
DB::DB(const char* host, const char* user, const char* passwd, const char* db) {
	mysql_init(&conn);
	if ((connection = mysql_real_connect(&conn, host, user, passwd, db, 3306, (char*)NULL, 0)) == NULL) {
		std::cout << "Mysql ���� : " << mysql_error(&conn) << std::endl;
		class_stat = false;
		return;
	}

	if (mysql_select_db(&conn, db)) {
		std::cout << mysql_errno(&conn) << " ���� : " << mysql_error(&conn) << std::endl;
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

bool DB::is_mac(std::string mac) { //xx-xx-xx-xx-xx-xx 17�ڸ�
	if (mac.length() != 17)
		return false;
	else {
		register int pos;
		for (pos = 0; pos < 15; pos += 3) {
			if (!isalnum(mac[pos]) || !isalnum(mac[pos+1]))
				return false;
			if (mac[pos+2] != ':')
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
		std::cout << "Mysql ���� : " << mysql_error(&conn) << std::endl;
		return *NULL;
	}
}*/
std::string DB::get_all_data(const char* table) {
	std::string tmp = "select *from ", result_str;
	tmp = tmp + table;
	std::lock_guard<std::mutex> guard(DB_mtx);
	if ((query_stat = mysql_query(connection, tmp.c_str())) != 0) {
		std::cout << "Mysql ���� : " << mysql_error(&conn) << std::endl;
		return NULL;
	}
	sql_result = mysql_store_result(connection);

	int num_fields = mysql_num_fields(sql_result);
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		register int i;
		for (i = 0; i < num_fields-1; i++)
			result_str = result_str + sql_row[i] + " ";
		result_str = result_str + sql_row[i] + "\n";
	}
	mysql_free_result(sql_result);
	return result_str;
}
std::string DB::search_eql(const char* table, int args, ...) {
	std::string tmp = "select *from ", result_str;
	tmp = tmp + table + " where "; //+ key + "=" + "'" + value + "';";
	va_list ap;
	va_start(ap, args * 2);
	for (int i = 0; i < args - 1; i++) {
		tmp = tmp + va_arg(ap, const char*) + "=" + va_arg(ap, const char*) + " and "; //�ɸ��� ���·� �޴°� ������� ����
	}
	tmp = tmp + va_arg(ap, const char*) + "=" + va_arg(ap, const char*) + ";";
	va_end(ap);

	std::lock_guard<std::mutex> guard(DB_mtx);
	if ((query_stat = mysql_query(connection, tmp.c_str())) != 0) {
		std::cout << "Mysql ���� : " << mysql_error(&conn) << std::endl;
		return NULL;
	}
	if ((sql_result = mysql_store_result(connection)) == NULL) {
		std::cout << "Mysql ���� : " << mysql_error(&conn) << std::endl;
		return NULL;
	}
	int num_fields = mysql_num_fields(sql_result);
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		register int i;
		for (i = 0; i < num_fields - 1; i++)
			result_str = result_str + sql_row[i] + " ";
		result_str = result_str + sql_row[i] + "\n";

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


std::vector <_fn_point> DB::get_fnprint_data(std::vector <std::string> MAC, std::vector <int*> value) { // �÷� ������ ū��
	std::string tmp;
	std::vector <_fn_point> result;
	std::vector <std::vector<std::string>> list_pos[4];
	const char* drec[4] = { "up_level", "right_level", "down_level", "left_level" };
	//std::lock_guard<std::mutex> guard(DB_mtx);
	for (int d = 0; d < 4; d++) {
		for (int i = 0; i < MAC.size(); i++) {
			tmp = "select bid, num_floor, x, y from fn_print where ap_MAC='" + MAC[i] + "' and " + drec[d] + " between " + std::to_string(value[i][0]) + " and " + std::to_string(value[i][1]) + ";";
			DB_mtx.lock();
			if ((query_stat = mysql_query(connection, tmp.c_str())) != 0) {
				std::cout << "Mysql ���� : " << mysql_error(&conn) << std::endl;
				return result;
			}
			sql_result = mysql_store_result(connection);
			int num_fields = mysql_num_fields(sql_result);
			if (i == 0) while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
				//std::string t[] = { sql_row[0], sql_row[1], sql_row[2], sql_row[3] };
				list_pos[d].push_back({ sql_row[0], sql_row[1], sql_row[2], sql_row[3] });
			}
			else {
				std::vector <std::vector<std::string>> tmp_pos;
				while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
					//std::string t[] = { sql_row[0], sql_row[1], sql_row[2], sql_row[3] };
					tmp_pos.push_back({ sql_row[0], sql_row[1], sql_row[2], sql_row[3] });
				}
				for (int j = list_pos[d].size() - 1; j >= 0; j--) {
					if (std::find(tmp_pos.begin(), tmp_pos.end(), list_pos[d][j]) == tmp_pos.end()) {
						list_pos[d].erase(list_pos[d].begin() + j);
					}
				}
			}
			mysql_free_result(sql_result);
			DB_mtx.unlock();
		}
	}
	for (int d = 0; d < 4; d++) {
		for (auto list_iter : list_pos[d]) {
			_fn_point tmp_point = { std::atoi(list_iter[0].c_str()), std::atoi(list_iter[1].c_str()) , std::atoi(list_iter[2].c_str()), std::atoi(list_iter[3].c_str()) };
			if (std::find_if(result.begin(), result.end(), _fn_point(tmp_point)) != result.end())
				continue;
			DB_mtx.lock();
			tmp = std::string("") + "select ap_MAC, " + drec[d] +  " from fn_print where bid=" + list_iter[0] + " and num_floor=" + list_iter[1] + " and x=" + list_iter[2] + " and y=" + list_iter[3] + " order by " + drec[d] + " desc;";
			if ((query_stat = mysql_query(connection, tmp.c_str())) != 0) {
				std::cout << "Mysql ���� : " << mysql_error(&conn) << std::endl;
				return result;
			}
			sql_result = mysql_store_result(connection);
			
			while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
				_fn_info tmp_info = { sql_row[0]};
				if (sql_row[1])
					tmp_info.level = std::atoi(sql_row[1]);
				else
					tmp_info.level = -200;
				
				tmp_point.fn_info.push_back(tmp_info);
			}
			result.push_back(tmp_point);


			mysql_free_result(sql_result);
			DB_mtx.unlock();
		}
		
		
	}
	return result;
	
}
std::string DB::search(const char* table, const char* column, const char* key, const char* value) {
	std::string tmp = "select ", result_str;
	tmp = tmp + column + " from " + table + " where " + key + "=" + value + ";";

	std::lock_guard<std::mutex> guard(DB_mtx);
	if ((query_stat = mysql_query(connection, tmp.c_str())) != 0) {
		std::cout << "Mysql ���� : " << mysql_error(&conn) << std::endl;
		return NULL;
	}
	sql_result = mysql_store_result(connection);

	int num_fields = mysql_num_fields(sql_result);
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		register int i;
		for (i = 0; i < num_fields-1; i++)
			result_str = result_str + sql_row[i] + " ";
		result_str = result_str + sql_row[i] + "\n";
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
		std::cout << "Mysql ���� : " << mysql_error(&conn) << std::endl;
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
		tmp = tmp + va_arg(ap, const char*) + "=" + va_arg(ap, const char*) + ", "; //�ɸ��� ���·� �޴°� ������� ����
	}
	tmp = tmp + va_arg(ap, const char*) + "=" + va_arg(ap, const char*);
	va_end(ap);
	tmp = tmp + " where " + target + "=" + target_val + ";";
	std::lock_guard<std::mutex> guard(DB_mtx);
	if ((query_stat = mysql_query(connection, tmp.c_str())) != 0) {
		std::cout << "Mysql ���� : " << mysql_error(&conn) << std::endl;
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
		tmp = tmp + va_arg(ap, std::string ) + ", "; //���� ���°� ��Ʈ��
	}
	tmp = tmp + va_arg(ap, std::string) + ");";
	va_end(ap);
	//std::cout << tmp;
	std::lock_guard<std::mutex> guard(DB_mtx);
	if ((query_stat = mysql_query(connection, tmp.c_str())) != 0) {
		std::cout << "Mysql ���� : " << mysql_error(&conn) << std::endl;
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
		std::cout << "Mysql ���� : " << mysql_error(&conn) << std::endl;
		return false;
	}
	return true;
}
std::vector<std::string> split(std::string str, char delimiter) {
	std::vector<std::string> internal;
	std::stringstream ss(str);
	std::string temp;

	while (getline(ss, temp, delimiter)) {
		internal.push_back(temp);
	}

	return internal;
}

#include <conio.h>
#include <cmath>

std::vector <building> list_building;


bool in_room(point pos, std::vector <point> poly) {
	int bef = 0;
	bool result = false;
	for (int i = 1; i < poly.size(); bef++, i++) {
		if (poly[bef].x < pos.x && poly[i].x < pos.x) // ���� ��� ����
			continue;

		point p_1, p_2;  //p1.x < p2.x
		if (poly[bef].x < poly[i].x) { p_1 = poly[bef]; p_2 = poly[i]; }
		else { p_1 = poly[i]; p_2 = poly[bef]; }

		double a, b; //x = ay + b
		if (p_1.y == p_2.y) {
			if (pos.y == p_1.y) {
				if (pos.x < p_1.x)
					result = !result;
			}
		}
		else if (p_1.x == p_2.x) {
			if (p_1.y > p_2.y) {
				if (pos.y >= p_2.y&&pos.y <= p_1.y)
					result = !result;
			}
			else {
				if (pos.y <= p_2.y&&pos.y >= p_1.y)
					result = !result;
			}
		}
		else {
			a = (double)(p_2.x - p_1.x) / (p_2.y - p_1.y);
			b = p_1.x - a * p_1.y;
			double tmp = a * pos.y + b;
			if (tmp >= p_1.x && tmp <= p_2.x)
				result = !result;
		}
	}
	return result;
}

//void calc_similarity(std::vector <_fn_point> *fn_point_list, std::vector <level_info> clnt_info) {
//	for (auto point_iter : *fn_point_list) {
//		std::vector <double> sim_tmp[4];
//		double sim_add[4] = { 0 };
//		for (auto clnt_iter : clnt_info) {
//			_fn_info* target_info = NULL;
//			for (auto info_iter : point_iter.fn_info) if (clnt_iter.ap_MAC == info_iter.ap_MAC) { target_info = &info_iter; break; }
//			double d = 0;
//			for (auto j : list_building[point_iter.bid].ap_list) if (j.ap_MAC == clnt_iter.ap_MAC) {
//				d = std::pow(std::pow(j.x - point_iter.x, 2) + std::pow(j.y - point_iter.y, 2), 0.5);
//				break;
//			}
//			if (target_info) {
//				//d * (1 - std::pow((double)10, (double)(info_iter.up - target_info->level)/20));
//				sim_tmp[0].push_back(d * (1 - std::pow((double)10, (double)(target_info->level - clnt_iter.level) / 20)));
//				/*sim_tmp[1].push_back(d * (1 - std::pow((double)10, (double)(target_info->right - clnt_iter.level) / 20)));
//				sim_tmp[2].push_back(d * (1 - std::pow((double)10, (double)(target_info->down - clnt_iter.level) / 20)));
//				sim_tmp[3].push_back(d * (1 - std::pow((double)10, (double)(target_info->left - clnt_iter.level) / 20)));*/
//			}
//			else {
//				sim_tmp[0].push_back(d * (1 - std::pow((double)10, (double)5)));
//				/*sim_tmp[1].push_back(d * (1 - std::pow((double)10, (double)5)));
//				sim_tmp[2].push_back(d * (1 - std::pow((double)10, (double)5)));
//				sim_tmp[3].push_back(d * (1 - std::pow((double)10, (double)5)));*/
//			}
//		}
//		for (int i = 0; i < 4; i++) {
//			for (auto j : sim_tmp[i])
//				sim_add[i] += j;
//		}
//	}
//}

bool calc(std::string input) {
	std::string clnt_mac;
	std::vector <level_info> clnt_info;
	std::vector <_fn_point> fn_point_list;
	//database->get_fnprint_pos(std::string("40:e3:d6:5f:3e:80"), "up_level", -90, -80);
	std::vector <std::string> line_vector = split(input, '\n'), patch_row;
	clnt_mac = line_vector[0];
	
	if (!database->exist("employees", "MAC_address", ("'" + clnt_mac + "'").c_str())) {
		std::cout << clnt_mac + "Ŭ���̾�Ʈ ��Ͽ� ���� ����" << std::endl;
		return false;
	}

	std::cout << "employee : " << database->search("employees", "name", "MAC_address", ("'" + clnt_mac + "'").c_str()) << "connected" << std::endl;

	line_vector.erase(line_vector.begin());
	for (auto i : line_vector) {
		patch_row = split(i, ' ');
		if (database->exist("ap_list", "ap_MAC", ("'" + patch_row[1] + "'").c_str())) {
			level_info tmp = { patch_row[0], patch_row[1], atoi(patch_row[2].c_str()), atoi(patch_row[3].c_str()) };
			clnt_info.push_back(tmp);
		}
	}
	std::vector <std::string> mac;
	std::vector <int*> value;
	for (auto i : clnt_info) {
		mac.push_back(i.ap_MAC);
		int v[] = { i.level - FILTER, i.level + FILTER };
		value.push_back(v);
	}
	fn_point_list = database->get_fnprint_data(mac, value);
	while (fn_point_list.size() == 0) {
		if (mac.empty() && value.empty()) 
			break;
		else {
			mac.pop_back();
			value.pop_back();
		}
		fn_point_list = database->get_fnprint_data(mac, value);
	}
	//calc_similarity(&fn_point_list, clnt_info);
	/*
	for (auto i : fn_point_list) {
		std::cout << i.bid << " " << i.num_floor << " " << i.x << " " << i.y << " : " << std::endl;
		for (auto j : i.fn_info)
			std::cout << j.ap_MAC << " " << j.up << " " << j.right << " " << j.down << " " << j.left << std::endl;
		std::cout << std::endl;
	}*/
}



void init_building(){
	std::string db_data = database->get_all_data("building");
	std::vector <std::string> line_vector = split(db_data, '\n'), patch_row;
	//building
	for (auto i : line_vector) {
		patch_row = split(i, ' ');
		building tmp = {patch_row[1], atoi(patch_row[2].c_str()), atoi(patch_row[3].c_str()) , atoi(patch_row[4].c_str()) ,atoi(patch_row[5].c_str()) };
		
		for (int j = 0; j < tmp.max_floor; j++) {
			map tmp_map = {j+1};
			
			std::string map_data = database->search_eql("map", 2,  patch_row[0].c_str(), "bid", std::to_string(j+1).c_str(), "num_floor" );
			std::vector <std::string> map_vector = split(map_data, '\n'), patch_row_map;
			for (auto map_iter : map_vector) {
				patch_row_map = split(map_iter, ' ');
				room tmp_room = {patch_row_map[3], atoi(patch_row_map[4].c_str()) , atoi(patch_row_map[5].c_str()) };
				
				
				std::string room_data = database->search_eql("room", 3,  patch_row[0].c_str(), "bid",  std::to_string(j + 1).c_str(), "num_floor",  patch_row_map[2].c_str(), "rid");
				std::vector <std::string> room_vector = split(room_data, '\n'), patch_row_room;
				point pos = { tmp_room.start_x, tmp_room.start_y };
				bool flag = false; // false ���� true �¿�
				tmp_room.point.push_back(pos);
				for (auto room_iter : room_vector) {
					patch_row_room = split(room_iter, ' ');
					if (flag)
						pos.x += atoi(patch_row_room[3].c_str());
					else
						pos.y += atoi(patch_row_room[3].c_str());
					flag = !flag;
					tmp_room.point.push_back(pos);
				}
				
				//�������� x �������� room ���� ()
				tmp_map.room_list.push_back(tmp_room);
				std::sort(tmp_map.room_list.begin(), tmp_map.room_list.end(), [](room x, room y) {
					return x.start_x > y.start_x;
				});
			}
			tmp.map_list.push_back(tmp_map);
			
		}
		std::string ap_data = database->search_eql("ap_list", 1, patch_row[0].c_str(), "bid");
		std::vector <std::string> ap_vector = split(ap_data, '\n'), patch_row_ap;
		for (auto ap_iter : ap_vector) {
			patch_row_ap = split(ap_iter, ' ');
			ap_point tmp_ap = { atoi(patch_row_ap[1].c_str()), atoi(patch_row_ap[2].c_str()), atoi(patch_row_ap[3].c_str()), atoi(patch_row_ap[4].c_str()) , patch_row_ap[5], atoi(patch_row_ap[6].c_str()), patch_row_ap[7] };
			tmp.ap_list.push_back(tmp_ap);
		}
		list_building.push_back(tmp);
		
	}

	////map
	//db_data.clear();
	//line_vector.clear();
	//patch_row.clear();
	//db_data = database->search("map", 1, "")
	//line_vector = split(db_data, '\n');
	//for (auto i : line_vector) {
	//	patch_row = split(i, ' ');
	//	std::vector <room> room_list;
	//	room tmp_room;
	//	tmp_room.
	//	map tmp = { atoi(patch_row[1].c_str()), patch_row[3], atoi(patch_row[4].c_str()) , atoi(patch_row[5].c_str()) };
	//	list_building[atoi(patch_row[0].c_str())].map_list.push_back(tmp);
	//}
	//db_data.clear();
	//line_vector.clear();
	//patch_row.clear();
	//db_data = database->get_all_data("room");
	//line_vector = split(db_data, '\n');
	//int x=0, y=0;
	//int count = 0;
	//bool flag = 0; //0 ����, 1 �¿�
	//register int bid = -1, num_floor = -1, rid = -1;
	//register int n_bid, n_num_floor, n_rid;
	//for (auto i : line_vector) {
	//	patch_row = split(i, ' ');
	//	n_bid = atoi(patch_row[0].c_str()); n_num_floor = atoi(patch_row[1].c_str()); n_rid = atoi(patch_row[2].c_str());
	//	if (rid != n_rid || num_floor != n_num_floor || bid != n_bid) { //next
	//		rid = n_rid; num_floor = n_num_floor; bid = n_bid;
	//		x=list_building[bid].map_list[num_floor].room_list[rid].
	//	}
	//	else {

	//	}
	//}


	/*for (auto i : list_building) {
		for (auto j : i.map_list) {
			x = j.start_x; y = j.start_y;
			point tmp;
			patch_row = line_vector[count++].data;
			for(int k = 0; k<)
			j.pos.push_back(tmp);
		}
	}*/
	//for(int i=0;i<)
	//point
}

int DB_handle() {

	database = &DB("127.0.0.1", "latter2005", "opentime4132@", "project");
	while (!database->class_stat) {
		std::cout << "press any key to restart db" << std::endl;
		_getch();
		database = &DB("127.0.0.1", "latter2005", "opentime4132@", "project");
	}
	
	init_building();
	init_view();
	
	//database->del("employees", "name", "'eee'");
	//std::cout << tmp;
	//std::cout << "mysql client version : " << mysql_get_client_info() << std::endl;
	database->exit_db();
	std::cout << "db��� ����" << std::endl;
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