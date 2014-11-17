#include "pch.h"
#include "Network.h"

using namespace std;
using namespace cocos2d;

static Network *instance = nullptr;

Network::Network() :
	trd(nullptr){
}
Network::~Network(){
}

Network *Network::create(){
	instance = new Network();

	if(instance)
		return instance;
	CC_SAFE_DELETE(instance);
	return nullptr;
}
Network *Network::getInstance(){
	return instance;
}

int Network::recv(
	void *dst, int len){

	int total = 0;

	while(total != len){
		int received =
			::recv(sock, (char*)dst+total, len-total, 0);

		if(received > 0)
			total += received;
		else break;
	}

	return total;
}
int Network::send(
	void *src, int len){

	return ::send(sock, (char*)src, len, 0);
}

void Network::worker(
	const std::string &host, int port,
	OpenHandler handler){

	WSADATA wsa;
	struct hostent* hoste;
	struct sockaddr_in hostAddr;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0){
		throwTaskToGameThread(std::bind(handler, false));
		return;
	}

	/* query host's address */
	if((hoste = gethostbyname(host.c_str())) == 0){
		throwTaskToGameThread(std::bind(handler, false));
		return;
	}

	memset(&hostAddr, 0, sizeof(hostAddr));
	hostAddr.sin_family = AF_INET;
	hostAddr.sin_addr.s_addr =
		*(unsigned long*)hoste->h_addr_list[0];
	hostAddr.sin_port = htons(port);

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock == INVALID_SOCKET){
		throwTaskToGameThread(std::bind(handler, false));
		return;
	}

	/* connect */
	if(connect(sock, (SOCKADDR*)&hostAddr, sizeof(hostAddr)) == SOCKET_ERROR){
		throwTaskToGameThread(std::bind(handler, false));
		return;
	}

	throwTaskToGameThread(std::bind(handler, true));
	recvLoop();
}
void Network::recvLoop(){
	while(true){
		int len;
		packet_header header;
		size_t headerSize = sizeof(header);

		len = recv(&header, headerSize);
		if(len != headerSize){
			cocos2d::log("recv error");
			return;
		}
		
		void *packet = malloc(header.size);
		memcpy(packet, &header, headerSize);
		len = recv((char*)packet+headerSize,
			header.size - headerSize);
		if(len != header.size - headerSize){
			cocos2d::log("recv error");
			return;
		}

		auto pair = handlers.find(header.id);
		if(pair == handlers.end())
			cocos2d::log("unhandled packet");
		else{
			throwTaskToGameThread(
				std::bind(pair->second, (packet_header*)packet));
		}
	}
}

void Network::open(
	const std::string &host, int port,
	OpenHandler handler){

	trd = new thread(
		std::bind(&Network::worker, this, host,port, handler));
}
void Network::close(){
	CC_ASSERT(trd != nullptr);

	CC_SAFE_DELETE(trd);
}

void Network::throwTaskToGameThread(
	std::function<void()> task){

	Director::getInstance()->getScheduler()
		->performFunctionInCocosThread(task);
}