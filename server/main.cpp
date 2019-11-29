#include "main.h"

#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>


int main(){

	WSADATA data;
	::WSAStartup(MAKEWORD(2, 2), &data);
	std::thread thread_DB(&DB_handle);
	std::thread thread_android(&android_server);
	//pool.enqueue_job(&android_server);
	


	thread_DB.join();	
	thread_android.join();
	
	nana::form fm;
	nana::label lab{ fm, "asdf" };
	lab.format(true);
	nana::button btn{ fm, "Quit" };
	btn.events().click([&fm] {
		fm.close();
	});
	fm.div("vert <><<><weight=80% text><>><><weight=24<><button><>><>");
	fm["text"] << lab;
	fm["button"] << btn;
	fm.collocate();
	fm.show();
	nana::exec();
	//server.close_server();
	::WSACleanup();
	return 0;
}

