#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/paint/graphics.hpp>

#include "db.h"

typedef struct employee : std::unary_function<employee, bool> {
	int id;
	std::string name, MAC_address;
	int bid, num_floor, rid; //-1 : ¾øÀ½
	employee(int id, std::string name, std::string MAC_address, int bid, int num_floor, int rid)
		: name(name), MAC_address(MAC_address), bid(bid), num_floor(num_floor), rid(rid) { }
	bool operator()(employee const& m) const {
		return id == m.id;
	}
}employee;

void init_view();
void update_absence(int clnt_id, std::string pos);

