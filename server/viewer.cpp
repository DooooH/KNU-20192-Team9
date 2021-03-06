
#include "viewer.h"


nana::listbox *emp_list_ptr;
std::mutex emp_list_mtx;
std::vector <employee> emp_vector;

void update_absence(int clnt_id, std::string pos) {
	std::lock_guard<std::mutex> guard(emp_list_mtx);
	for (auto i : emp_list_ptr->at(0)) {
		std::string id = std::to_string(clnt_id);
		while (id.size() < 3)
			id = "0" + id;
		if (i.text(0) == id) {
			i.text(3, nana::charset(pos).to_bytes(nana::unicode::utf8));
			return;
		}
	}
}

void init_list(nana::listbox *emp_list) {
	std::string tmp = database->get_all_data("employees");
	std::vector<std::string> line_vector = split(tmp, '\n'), patch_row;
	emp_list->clear();

	for (auto i : line_vector) {
		patch_row = split(i, ' ');
		while(patch_row[0].size()<3){
			patch_row[0] = "0" + patch_row[0];
		}
		employee tmp = { std::stoi(patch_row[0]) , patch_row[1], patch_row[2], std::stoi(patch_row[0]), std::stoi(patch_row[0]), std::stoi(patch_row[0]) };
		emp_list->at(0).append({ patch_row[0] , patch_row[1], patch_row[2], "X" });

	}
	//emp_list.
}


bool cho_popup(std::string msg, nana::form par) {
	bool rt_val = false;
	nana::form fm(par);
	fm.size({ 300,160 });
	msg = "<center size=12>" + msg + "</>";
	nana::label error_title(fm, msg, true);
	error_title.format(true);
	error_title.text_align(nana::align::center, nana::align_v::center);

	nana::button yes{ fm, nana::rectangle(100, 130, 40, 20), true };
	yes.caption("Yes");
	yes.events().click([&] {
		rt_val = true;
		fm.close();
	});
	nana::button no{ fm, nana::rectangle(160, 130, 40, 20), true };
	no.caption("No");
	no.events().click([&] {
		fm.close();
	});

	fm.div("vert<weight=70% msg><>");
	fm["msg"] << error_title;

	fm.collocate();
	fm.modality();
	return rt_val;
}

void popup(std::string msg, nana::form par) {
	nana::form fm(par);
	fm.size({ 300,160 });
	msg = "<center size=12>" + msg + "</>";
	nana::label error_title(fm, msg, true);
	error_title.format(true);
	error_title.text_align(nana::align::center, nana::align_v::center);

	nana::button quit{ fm, nana::rectangle(130, 130, 40, 20), true };
	quit.caption("OK");
	quit.events().click([&] {
		fm.close();
	});


	fm.div("vert<weight=70% msg><>");
	fm["msg"] << error_title;

	fm.collocate();
	fm.modality();
}


void init_view() {
	std::string tmp;
	static nana::form main_view;
	const nana::size si = { 1000, 600 };
	nana::API::track_window_size(main_view, si, true);
	nana::API::track_window_size(main_view, si, false);
	main_view.size(si);
	main_view.caption(("Server"));

	nana::listbox emp_list(main_view, true);
	emp_list_ptr = &emp_list;
	emp_list.append_header("id");
	emp_list.append_header("name");
	emp_list.append_header("MAC");
	emp_list.append_header("Absence");
	
	init_list(&emp_list);
	/*if (emp_list.sortable())
		std::cout << "��Ʈ����";
	emp_list.sortable(true);*/
	emp_list.events().dbl_click([&] {

		//std::cout << emp_list.selected().
		nana::form fm(main_view);
		fm.size({ 490,100 });
		auto pos = emp_list.selected().at(0).item;
		std::string org_id = emp_list.at(0).at(pos).text(0);
		fm.caption(emp_list.at(0).at(pos).text(1) + "'s information");

		nana::label input_id_title{ fm,"<center size=10>ID:</>", true };
		input_id_title.format(true);
		nana::textbox input_id(fm, nana::rectangle{ 10, 30, 150, 20 }, true); input_id.multi_lines(false);
		input_id.caption(emp_list.at(0).at(pos).text(0));

		nana::label input_name_title{ fm,"<center size=10> Name:</>", true };
		input_name_title.format(true);
		nana::textbox input_name(fm, nana::rectangle{ 170, 30, 150, 20 }, true); input_name.multi_lines(false);
		input_name.caption(emp_list.at(0).at(pos).text(1));

		nana::label input_MAC_title{ fm,"<center size=10>  MAC address:</>", true };
		input_MAC_title.format(true);
		nana::textbox input_MAC(fm, nana::rectangle{ 330, 30, 150, 20 }, true); input_MAC.multi_lines(false);
		input_MAC.caption(emp_list.at(0).at(pos).text(2));

		nana::button modify{ fm, nana::rectangle(165, 60, 40, 20), true };
		modify.caption("Modify");
		modify.events().click([&] {
			if (!cho_popup("Are you sure to modify client?", fm))
				return;
			std::string id, name, mac;
			input_id.getline(0, id); input_name.getline(0, name); input_MAC.getline(0, mac);
			if (!DB::is_mac(mac)) {
				popup("Modify fail : Invalid MAC address", fm);
				fm.close(); return;
			}
			if (id != org_id)
				if (database->exist("employees", "id", id.c_str())) {
					popup("Modify fail : Duplicate ID", fm);
					fm.close(); return;
				}

			if (database->update("employees", "id", org_id.c_str(), 3,
				id.c_str(), "id", ("'" + name + "'").c_str(), "name", ("'" + mac + "'").c_str(), "MAC_address")) {
				emp_list.at(0).at(pos).text(0, id);
				emp_list.at(0).at(pos).text(1, name);
				emp_list.at(0).at(pos).text(2, mac);
				popup("Modifed Successfully", fm);
			}
			else {
				popup("Modify failed : update error", fm);
			}
			//std::cout << id << name << mac;



			//
			fm.close();
		});

		nana::button not_sub{ fm, nana::rectangle(225, 60, 40, 20), true };
		not_sub.caption("Quit");
		not_sub.events().click([&] {

			fm.close();
		});

		nana::button del{ fm, nana::rectangle(285, 60, 40, 20), true };
		del.caption("Delete");
		del.events().click([&] {

			if (!cho_popup("Are you sure to delete client?", fm))
				return;
			std::string id = emp_list.at(0).at(pos).text(0);

			//std::cout << id << name << mac;

			if (database->exist("employees", "id", id.c_str())) {
				if (database->del("employees", "id", id.c_str())) {
					emp_list.erase(emp_list.selected());
					popup("Deleted successfully", fm);
				}
				else
					popup("Delete fail: Delete error", fm);
			}
			else {
				popup("Delete fail: No ID in table", fm);
			}

			//
			fm.close();
		});

		fm.div("margin=10 vert <title><><>");
		fm["title"] << input_id_title << input_name_title << input_MAC_title;
		fm.collocate();
		fm.modality();
	});

	nana::label list_title{ main_view,"<center size=15>Current list:</>", true };
	list_title.format(true);
	nana::label map_title{ main_view,"<center size=15>Map:</>", true };
	map_title.format(true);

	//quit
	nana::button quit{ main_view, nana::rectangle(950, 10, 40, 20), true };
	quit.caption("Quit");
	quit.events().click([&] {
		nana::API::exit();
		main_view.close();
	});

	//add
	nana::button db_insert{ main_view, nana::rectangle(450, 12, 40, 20), true };
	db_insert.caption("Add");
	db_insert.events().click([&] {
		nana::form fm(main_view);
		fm.size({ 490,150 });

		nana::label input_id_title{ fm,"<center size=10>ID:</>", true };
		input_id_title.format(true);
		nana::textbox input_id(fm, nana::rectangle{ 10, 30, 150, 20 }, true); input_id.multi_lines(false);

		nana::label input_name_title{ fm,"<center size=10> Name:</>", true };
		input_name_title.format(true);
		nana::textbox input_name(fm, nana::rectangle{ 170, 30, 150, 20 }, true); input_name.multi_lines(false);

		nana::label input_MAC_title{ fm,"<center size=10>  MAC address:</>", true };
		input_MAC_title.format(true);
		nana::textbox input_MAC(fm, nana::rectangle{ 330, 30, 150, 20 }, true); input_MAC.multi_lines(false);

		nana::label input_building_title{ fm,"<center size=10>Office building:</>", true };
		input_building_title.format(true);
		nana::textbox input_building(fm, nana::rectangle{ 10, 70, 150, 20 }, true); input_building.multi_lines(false);

		nana::label input_num_floor_title{ fm,"<center size=10> Floor:</>", true };
		input_num_floor_title.format(true);
		nana::textbox input_num_floor(fm, nana::rectangle{ 170, 70, 150, 20 }, true); input_num_floor.multi_lines(false);

		nana::label input_room_title{ fm,"<center size=10> Room:</>", true };
		input_room_title.format(true);
		nana::textbox input_room(fm, nana::rectangle{ 330, 70, 150, 20 }, true); input_room.multi_lines(false);


		nana::button sub{ fm, nana::rectangle(185, 110, 40, 20), true };
		sub.caption("Add");
		sub.events().click([&] {
			std::string id, name, mac, building, num_floor, room;
			input_id.getline(0, id); input_name.getline(0, name); input_MAC.getline(0, mac); input_building.getline(0, building); input_num_floor.getline(0, num_floor); input_room.getline(0, room);
			if (!DB::is_mac(mac)) {
				popup("Add fail : Invalid MAC address", fm);
				fm.close(); return;
			}
			//std::cout << id << name << mac;
			
			if (database->exist("employees", "id", id.c_str()))
				popup("Add fail: Duplicate ID", fm);
			else if(database->exist("employees", "MAC_address", ("'" + mac + "'").c_str()))
				popup("Add fail: Duplicate MAC", fm);
			else {
				
				//name = nana::charset(name).to_bytes(nana::unicode::utf8); building = nana::charset(building).to_bytes(nana::unicode::utf8); room = nana::charset(room).to_bytes(nana::unicode::utf8);
				std::string bid = database->search("building", "bid", "bname", ("'" + building+ "'").c_str()), rid;
				if (bid.size() == 0) {
					popup("Add fail: No such a building", fm);
					fm.close(); return;
				}
				bid.pop_back();
				auto max_floor = std::stoi(database->search("building", "max_floor", "bname", ("'" + building + "'").c_str()));
				//max_floor.pop_back();
				if (std::stoi(num_floor) > max_floor) {
					popup("Add fail: Floor error", fm);
					fm.close(); return;
				}
				auto tmp = database->search_eql("map", 3, bid.c_str(), "bid", num_floor.c_str(), "num_floor", ("'" + room + "'").c_str(), "rname");
				if (tmp.size() == 0) {
					popup("Add fail: Room error", fm);
					fm.close(); return;
				}
				std::vector <std::string> tmp_vec = split(tmp, ' ');
				rid = tmp_vec[2];


				std::cout << "insert " << name << std::endl; //
				if (database->insert("employees", 6, id, "'" + name + "'", "'" + mac + "'", bid, num_floor, rid)){
					popup("Added Successfully", fm);
					while (id.size() < 3) id = "0" + id;
					emp_list.at(0).append({ id, name, mac , "X"});

					//init_list(&emp_list);
				}
				else
					popup("Add fail: Insert error", fm);
			}
			//
			fm.close();
		});
		nana::button not_sub{ fm, nana::rectangle(265, 110, 40, 20), true };
		not_sub.caption("Quit");
		not_sub.events().click([&] {

			fm.close();
		});



		fm.div("margin=10 vert <title><title2><>");
		fm["title"] << input_id_title << input_name_title << input_MAC_title;
		fm["title2"] << input_building_title << input_num_floor_title << input_room_title;
		fm.collocate();
		fm.modality();
	});
	//map
	nana::nested_form draw_form(main_view, nana::rectangle(505, 50, 490, 490), nana::appearance(false, false, false, true, false, false, false));
	draw_form.bgcolor(nana::color(255, 255, 255));

	nana::drawing dw{ draw_form };

	nana::combox com_room(main_view, nana::rectangle(780, 12, 100, 20), false);
	com_room.caption("Room");

	nana::combox com_floor(main_view, nana::rectangle(680, 12, 50, 20), false);
	com_floor.caption("Floor");

	nana::combox com_building(main_view, nana::rectangle(560, 12, 100, 20), true);
	com_building.caption("Building");
	for (auto i : list_building)
		com_building.push_back(nana::charset(i.bname).to_bytes(nana::unicode::utf8));
	/*com_building.events().selected([&] {
		auto pos = com_building.option();
		for (auto i : list_building[pos].map_list) {
			com_floor.push_back(std::to_string(i.num_floor));
		}
		com_floor.show();
	});*/

	com_building.events().selected([&] {
		//com_bul_ch = true;
		draw_form.hide();
		com_room.hide();
		com_floor.clear();

		auto pos = com_building.option();
		for (auto i : list_building[pos].map_list) {
			com_floor.push_back(std::to_string(i.num_floor));
		}
		com_floor.show();

	});
	bool com_floor_selected = false;
	com_floor.events().selected([&] {
		com_floor_selected = true;

		com_room.clear();
		//com_room.hide();
		draw_form.hide();

		short bid = com_building.option(), num_floor = com_floor.option();
		int size_x = list_building[bid].size_x, size_y = list_building[bid].size_y;
		double per = (size_y > size_x) ? 480.0 / size_y : 480.0 / size_x;

		for (auto i : list_building[bid].map_list[num_floor].room_list)
			com_room.push_back(nana::charset(i.rname).to_bytes(nana::unicode::utf8));
		dw.draw([&](nana::paint::graphics& map) {
			if (com_floor_selected) {
				for (auto i : list_building[bid].ap_list) {

				}
				for (auto i : list_building[bid].map_list[num_floor].room_list) {
					double x, y;
					if (size_x > size_y) {
						x = i.point[0].x * per + 7;
						y = (250 - size_y * per) + i.point[0].y * per;
					}
					else {
						x = (250 - size_x * per) + i.point[0].x * per;
						y = i.point[0].y * per + 5;
					}
					double next_x, next_y;
					for (int j = 1; j < i.point.size(); j++) {
						if (size_x > size_y) {
							next_x = i.point[j].x * per + 7;
							next_y = (250 - size_y * per) + i.point[j].y * per;
						}
						else {
							next_x = (250 - size_x * per) + i.point[j].x * per;
							next_y = i.point[j].y * per + 7;
						}
						map.line(nana::point(x, y), nana::point(next_x, next_y), nana::colors::black);
						x = next_x; y = next_y;
					}
					if (size_x > size_y)
						map.line(nana::point(x, y), nana::point(i.point[0].x * per + 7, (250 - size_y * per) + i.point[0].y * per), nana::colors::black);
					else
						map.line(nana::point(x, y), nana::point((250 - size_x * per) + i.point[0].x * per, y = i.point[0].y * per + 7), nana::colors::black);
				}
			}

		});

		draw_form.show();
		com_floor_selected = false;
		com_room.show();
	});

	//list_building[bid].map_list[num_floor].room_list;
	com_room.events().selected([&] {
		short bid = com_building.option(), num_floor = com_floor.option();
		auto room_ptr = &list_building[bid].map_list[num_floor].room_list[com_room.option()];
		//popup room info
		nana::form fm(main_view);
		fm.caption(nana::charset(("Empolyees in " + room_ptr->rname)).to_bytes(nana::unicode::utf8)); // nana::charset(("Empolyees in " + room_ptr->rname)).to_bytes(nana::unicode::utf8)
		fm.size({ 300,400 });
		std::map <int, std::string> tmp_map(clnt_list);
		nana::listbox tmp_list(fm);
		tmp_list.append_header("name");
		tmp_list.append_header("Absence");
		tmp_list.append("Office Staff");
		tmp_list.append("Outer Staff"); 
		std::vector <std::string> line_vector =
			split(database->search_eql("employees", 3, std::to_string(bid).c_str(), "bid", std::to_string(num_floor + 1).c_str(), "num_floor", std::to_string(room_ptr->rid).c_str(), "rid"), '\n'), patch_row;
		for (auto i : line_vector) {
			patch_row = split(i, ' ');
			auto tmp_iter = tmp_map.find(std::stoi(patch_row[0]));
			if (tmp_iter == tmp_map.end()) 
				tmp_list.at(1).append({ patch_row[1], "X" });
			else {
				tmp_list.at(1).append({ patch_row[1], "O" });
				tmp_map.erase(tmp_iter);
			}
		}
		std::string current_pos = list_building[bid].bname + " " + std::to_string(num_floor + 1) + "�� " + room_ptr->rname;
		for (auto i : tmp_map) {
			if (i.second == current_pos) {
				tmp_list.at(2).append({ database->search("employees", "name", "id", std::to_string(i.first).c_str()), "O" });
			}
		}
		fm.div("margin=[5, 5] <list>");
		fm["list"] << tmp_list;
		fm.collocate();
		fm.modality();
	});
	com_building.show();
	//com_floor.show();
	//com_room.show();

	main_view.div("<vert margin=[5, 10] < weight=7% <list_title>> <<list> <>> >");
	main_view["list_title"] << list_title << map_title;
	//fm["btns"] << quit;
	main_view["list"] << emp_list;

	main_view.collocate();
	main_view.show();
	nana::exec();
}





//delete
	/*nana::button db_del{ main_view, nana::rectangle(360, 10, 40, 20), true };
	db_del.caption("Delete");
	db_del.events().click([&] {
		nana::form fm(main_view);
		fm.size({ 190,100 });

		nana::label input_id_title{ fm,"<center size=10>ID:</>", true };
		input_id_title.format(true);
		nana::textbox input_id(fm, nana::rectangle{ 10, 30, 150, 20 }, true); input_id.multi_lines(false);

		nana::button sub{ fm, nana::rectangle(35, 60, 40, 20), true };
		sub.caption("Delete");
		sub.events().click([&] {
			std::string id;
			input_id.getline(0, id);
			//std::cout << id << name << mac;

			if (database->exist("employees", "id", id.c_str())) {
				if (database->del("employees", "id", id.c_str())) {

					popup("Deleted successfully", fm);
				}

				else
					popup("Delete fail: Insert error", fm);
			}
			else {
				popup("Delete fail: No ID in table", fm);
			}

			//
			fm.close();
		});

		fm.div("margin=10 vert <title><><>");
		fm["title"] << input_id_title;
		fm.collocate();
		fm.modality();
	});*/