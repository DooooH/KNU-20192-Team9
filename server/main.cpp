
#include "main.h"



int main()
{
	WSADATA data;
	::WSAStartup(MAKEWORD(2, 2), &data);
	std::thread thread_DB(&DB_handle);

	//std::thread thread_android(&android_server);
	android_server();
	//pool.enqueue_job(&android_server);

	//win_init()
	//thread_android.join();
	thread_DB.join();
	


	//server.close_server();
	::WSACleanup();
	return 0;
	
}

