
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

	emp_list->append_header("id");
	emp_list->append_header("name");
	emp_list->append_header("MAC");

	std::string tmp = database->get_all_data("employees");
	std::vector<std::string> line_vector = split(tmp, '\n'), patch_row;
	tmp.clear();


	for (auto i : line_vector) {
		patch_row = split(i, ' ');

		emp_list->at(0).append({ patch_row[0] , patch_row[1], patch_row[2] });

	}
	//emp_list.
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
	nana::label list_title{ main_view,"<center size=15>Current list:</>", true };
	nana::button quit{ main_view, nana::rectangle(950, 10, 40, 20), true };
	quit.caption("Quit");
	list_title.format(true);
	init_list(&emp_list);
	
	
	quit.events().click([&] {
		nana::API::exit();
		main_view.close();
	});



	main_view.div("<vert< weight=7% <list_title>> <list>>");
	main_view["list_title"] << list_title;
	//fm["btns"] << quit;
	main_view["list"] << emp_list;

	main_view.collocate();
	main_view.show();
	nana::exec();
}