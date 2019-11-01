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

#include "db.h"

void init_view();

extern nana::form *main_view_ptr;
