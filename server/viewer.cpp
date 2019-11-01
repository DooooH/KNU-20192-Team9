
#include "viewer.h"

nana::form *main_view_ptr;


std::vector<std::string> split(std::string str, char delimiter) {
	std::vector<std::string> internal;
	std::stringstream ss(str);
	std::string temp;

	while (getline(ss, temp, delimiter)) {
		internal.push_back(temp);
	}

	return internal;
}

void init_list(nana::listbox *emp_list) {
	std::string tmp = database->get_all_data("employees");
	std::vector<std::string> line_vector = split(tmp, '\n'), patch_row;
	emp_list->clear();

	for (auto i : line_vector) {
		patch_row = split(i, ' ');

		emp_list->at(0).append({ patch_row[0] , patch_row[1], patch_row[2] });

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
	emp_list.append_header("id");
	emp_list.append_header("name");
	emp_list.append_header("MAC");
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
	nana::button db_insert{ main_view, nana::rectangle(300, 10, 40, 20), true };
	db_insert.caption("Add");
	db_insert.events().click([&] {
		nana::form fm(main_view);
		fm.size({ 490,100 });

		nana::label input_id_title{ fm,"<center size=10>ID:</>", true };
		input_id_title.format(true);
		nana::textbox input_id(fm, nana::rectangle{ 10, 30, 150, 20 }, true); input_id.multi_lines(false);

		nana::label input_name_title{ fm,"<center size=10> Name:</>", true };
		input_name_title.format(true);
		nana::textbox input_name(fm, nana::rectangle{ 170, 30, 150, 20 }, true); input_name.multi_lines(false);

		nana::label input_MAC_title{ fm,"<center size=10>  MAC address:</>", true };
		input_MAC_title.format(true);
		nana::textbox input_MAC(fm, nana::rectangle{ 330, 30, 150, 20 }, true); input_MAC.multi_lines(false);

		nana::button sub{ fm, nana::rectangle(185, 60, 40, 20), true };
		sub.caption("Add");
		sub.events().click([&] {
			std::string id, name, mac;
			input_id.getline(0, id); input_name.getline(0, name); input_MAC.getline(0, mac);
			if (!DB::is_mac(mac)) {
				popup("Modify fail : Invalid MAC address", fm);
				fm.close(); return;
			}
			name = "'" + name + "'";
			mac = "'" + mac + "'";
			//std::cout << id << name << mac;

			if (database->exist("employees", "id", id.c_str()))
				popup("Add fail: Duplicate ID", fm);
			else {
				if (database->insert("employees", 3, id, name, mac)) {
					popup("Added Successfully", fm);
					emp_list.at(0).append({ id, name, mac });
					//init_list(&emp_list);
				}
				else
					popup("Add fail: Insert error", fm);
			}
			//
			fm.close();
		});
		nana::button not_sub{ fm, nana::rectangle(265, 60, 40, 20), true };
		not_sub.caption("Quit");
		not_sub.events().click([&] {

			fm.close();
		});

		fm.div("margin=10 vert <title><><>");
		fm["title"] << input_id_title << input_name_title << input_MAC_title;

		fm.collocate();
		fm.modality();
	});


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

	main_view.div("<vert margin=[5, 5] < weight=7% <list_title>> <<list> <>> >");
	main_view["list_title"] << list_title << map_title;
	//fm["btns"] << quit;
	main_view["list"] << emp_list;

	main_view.collocate();
	main_view.show();
	nana::exec();
}