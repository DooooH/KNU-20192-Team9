#pragma once
#include <iostream>
#include <WinSock2.h>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include "db.h"
#pragma comment (lib , "ws2_32.lib")
#define MAX_BUF_SIZE 4096

class SERVER {
private:
	std::string MAC;
	std::string IP;
	SOCKET sock;
	sockaddr_in addr;
public:

	SERVER();
	//SERVER() = delete;
	SERVER(ULONG IP, u_short port);
	//~SERVER();
	void close_server();
	/*
	bool set_sockaddr();*/
	SOCKET server_accept(sockaddr_in* clnt_addr, int* addrlen);

};

class CLIENT {
private:
	std::string MAC;
	ULONG IP;
	
	sockaddr_in addr;
	//std::thread::id thread_id;

public:
	SOCKET sock;
	CLIENT();
	CLIENT(SOCKET sock, sockaddr_in addr);
	//CLIENT(SOCKET sock, sockaddr_in addr, std::thread::id thread_id);
	~CLIENT();
	void close_sock();
	bool operator==(CLIENT other);
	size_t send_msg(char *msg);
	int recv_msg(char (*buf)[MAX_BUF_SIZE]);
	/*
	void _handle();*/
};

int android_server();

extern SERVER *server;
extern std::vector <CLIENT> client_list;
extern std::mutex client_list_mtx;