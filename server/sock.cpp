#include "sock.h"


SERVER *server;
//std::vector <CLIENT> client_list;
std::mutex client_list_mtx;


SERVER::SERVER() {
	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
		std::cout << "서버소켓 에러 : 소켓생성" << std::endl;
	}
}
//SERVER() = delete;
SERVER::SERVER(ULONG IP, u_short port) {
	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
		std::cout << "서버소켓 에러 : 소켓생성" << std::endl;
		//exit(-1);
	}

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(IP);
	addr.sin_port = htons(port);

	if (bind(sock, (sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
		std::cout << "서버소켓 에러 : 소켓 bind" << std::endl;
		//exit(-1);
	}
	if (listen(sock, SOMAXCONN) == SOCKET_ERROR) {
		std::cout << "서버소켓 에러 : 소켓 listen" << std::endl;
		//exit(-1);
	}
}
//SERVER::~SERVER() { closesocket(sock); }
void SERVER::close_server() {
	closesocket(sock);
	delete this;
}
/*
bool SERVER::set_sockaddr() {
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9999);
}*/
SOCKET SERVER::server_accept(sockaddr_in* clnt_addr, int* addrlen) {
	return accept(sock, (sockaddr*)clnt_addr, addrlen);
}




/*
void CLIENT::_handle() {
	char *msg = recv_msg(), *pos;
	if (msg == NULL) return;
	else {
		pos = strstr(msg, " ");
		*(pos)++ = '\0';
		MAC = msg;
		msg = pos;
		while (msg != NULL) {
			//리스트 파싱 디비랑 연산
		}
	}
}*/



void recv_handle(SOCKET clnt) {

	int recvcnt, total_cnt = 0;
	char buf[MAX_BUF_SIZE];
	std::string data;
	while (recvcnt = recv(clnt, buf, sizeof(buf) - 1, 0)) {

		buf[recvcnt] = '\0';
		if (buf[recvcnt - 1] == ';') {
			buf[recvcnt - 1] = '\0';
			data = data + buf;
			total_cnt += recvcnt;
			break;
		}
		data = data + buf;
		total_cnt += recvcnt;
	}

	if (data.size() <= 0)
		return;

	std::cout << "recv :" << std::endl << data << std::endl;
	std::string msg;
	if (calc(data, msg))
		send(clnt, msg.c_str(), msg.size(), 0);
	else
		std::cout << "error occur" << std::endl;
	closesocket(clnt);
}
int android_server() {

	sockaddr_in addr = { 0 };
	int addr_len = sizeof(addr);
	void recv_handle(SOCKET clnt);
	server = &SERVER(INADDR_ANY, (u_short)8888);
	std::thread clnt_thread;
	SOCKET tmp;
	while (1) {

		std::cout << "연결 기다림" << std::endl;
		//CLIENT clnt(server->server_accept(&addr, &addr_len), addr);
		tmp = server->server_accept(&addr, &addr_len);
		std::cout << "새 연결" << std::endl;
		/*int recvcnt;
		char buf[500];
		while (recvcnt = recv(clnt.sock, buf, sizeof(buf) - 1, 0))
		{
			buf[recvcnt] = '\0';
			printf("%s\n", buf);
		}*/


		//client_list_mtx.lock();
		//std::lock_guard<std::mutex> guard(client_list_mtx);
		//client_list.push_back(clnt);
		//client_list_mtx.unlock();

		clnt_thread = std::thread(&recv_handle, tmp);
		//clnt_thread = std::thread(&_handle, clnt);
		clnt_thread.detach();
		std::cout << "end " << std::endl;
		//std::lock_guard<std::mutex> guard(thread_list_mtx);
		//thread_list.push_back(clnt_thread);
		//thread_list_mtx.unlock();

	}

	std::cout << "소켓핸들 종료" << std::endl;
	return 0;
}
