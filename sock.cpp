#include "sock.h"


SERVER *server;
std::vector <CLIENT> client_list;
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


///////////////////
CLIENT::CLIENT() { this->sock = NULL; this->addr = { 0 }; }
CLIENT::CLIENT(SOCKET sock, sockaddr_in addr) {
	this->sock = sock; this->addr = addr; this->IP = addr.sin_addr.s_addr;
}
/*
CLIENT::CLIENT(SOCKET sock, sockaddr_in addr, std::thread::id thread_id) {
	this->sock = sock; this->addr = addr; this->IP = addr.sin_addr.s_addr;  this->thread_id = thread_id;
}*/
CLIENT::~CLIENT() { closesocket(sock); }
void CLIENT::close_sock() {
	closesocket(sock);
	delete this;
}
bool CLIENT::operator==(CLIENT other) { return this == &other; }
size_t CLIENT::send_msg(char *msg) { return send(this->sock, msg, strlen(msg), NULL); }
int CLIENT::recv_msg(char *buf) {
	
	int count;
	if ((count = recv(this->sock, buf, MAX_BUF_SIZE, NULL)) == 0)
		return NULL;
	buf[count] = '\0';
	return count;
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



void _handle(CLIENT clnt) {
	//std::cout << "asdf";

	/*
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
	for (std::vector<CLIENT>::iterator iter = client_list.begin(), eiter = client_list.end(); iter != eiter; iter++) {
		if (*iter == clnt) {
			client_list.erase(iter);
			clnt.close_sock();
			break;
		}
	}*/
	char tmp[2048];
	while (1) {
		clnt.recv_msg(tmp);
		std::cout << tmp << std::endl;
	}
}

int android_server() {
	SOCKET tmp;
	sockaddr_in addr = { 0 };
	int addr_len = sizeof(addr);

	server = &SERVER(INADDR_ANY, (u_short)9999);
	
	while (1) {
		std::thread clnt_thread;
		std::cout << "연결 기다림" << std::endl;
		tmp = server->server_accept(&addr, &addr_len);
		std::cout << "새 연결" << std::endl;


		//tmp = accept(server->sock, (sockaddr*)&addr, &addr_len);
		CLIENT clnt(tmp, addr);
		
		std::lock_guard<std::mutex> guard(client_list_mtx);
		client_list.push_back(clnt);
		client_list_mtx.unlock();
		clnt_thread = std::thread(&_handle, clnt);
		clnt_thread.detach();
		//std::lock_guard<std::mutex> guard(thread_list_mtx);
		//thread_list.push_back(clnt_thread);
		//thread_list_mtx.unlock();

	}
	std::cout << "소켓핸들 종료" << std::endl;
	return 0;
}
