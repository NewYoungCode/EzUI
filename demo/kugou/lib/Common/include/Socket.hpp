#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <memory>

#ifdef _WIN32
#include <winsock.h>
#pragma comment(lib,"ws2_32.lib")
#endif
static WSADATA __wsadata__;
class Socket
{
private:
	SOCKET socket = NULL;
	sockaddr_in sockaddr;
public:
	//初始化套接字库
	static bool Init() {
		if (__wsadata__.wVersion == 0) {
			int code = !WSAStartup(MAKEWORD(2, 2), &__wsadata__);
			return code;
		}
		else if (__wsadata__.wVersion != 0) {
			return true;
		}
		return false;
	}
	//清理
	static void Cleanup() {
		::WSACleanup();
		__wsadata__.wVersion = 0;
	}
	static std::vector<std::string> GetIpByName(const std::string& hostname) {
		std::vector<std::string> ipList;
		char           szHostname[100];
		HOSTENT* pHostEnt;
		int             nAdapter = 0;
		struct       sockaddr_in   sAddr;
		Init();
		struct hostent* host = ::gethostbyname(hostname.c_str());
		for (int i = 0; host && host->h_addr_list[i]; i++)
		{
			char* ip = inet_ntoa(*(struct in_addr*)host->h_addr_list[i]);
			ipList.emplace_back(ip);
		}
		return ipList;
	}
	size_t Port = 0;
	std::string Address;
	int Receive(char* outBuf, size_t recvLen, int flags = 0) const;
	bool Connect(const std::string& ip, size_t port);
	bool Bind(const std::string& ip, size_t port);
	bool Listen(int backlog = 5);
	Socket Accep() const;
	int Write(const char* buff, int size)const;
	void Close() const;
	Socket();
	Socket(SOCKET socket);
	virtual	~Socket();
};
inline int Socket::Receive(char* outBuf, size_t recvLen, int flags) const
{
	return ::recv(socket, outBuf, recvLen, 0);
}
inline bool Socket::Connect(const std::string& ip, size_t port)
{
	sockaddr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
	sockaddr.sin_port = htons(port);
	//创建套接字
	socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (connect(socket, (SOCKADDR*)&sockaddr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		printf("%s\n", "Connection failed");
		return false;
	}
	else {
		return true;
	}
}
inline bool Socket::Bind(const std::string& ip, size_t port) {
	//TCP的

	socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socket == INVALID_SOCKET) {
		wprintf(L"socket function failed with error: %u\n", WSAGetLastError());
		return false;
	}
	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port for the socket that is being bound.
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());
	sockaddr.sin_port = htons(port);
	//----------------------
	// Bind the socket.
	if (bind(socket, (SOCKADDR*)&sockaddr, sizeof(sockaddr)) == SOCKET_ERROR) {
		wprintf(L"bind failed with error %u\n", WSAGetLastError());
		closesocket(socket);
		return false;
	}
	else
	{
		this->Port = port;
		this->Address = ip;
		return true;
	}
	return false;
}
inline bool Socket::Listen(int backlog) {
	return !::listen(socket, backlog);
}
inline Socket Socket::Accep() const {
	for (;;) {
		sockaddr_in  ClientAddr{0};
		int  AddrLen = sizeof(ClientAddr);
		SOCKET clientSocket = accept(socket, (LPSOCKADDR)&ClientAddr, &AddrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			printf("INVALID_SOCKET\n");
			return Socket(NULL);
		}
		Socket skt(clientSocket);
		skt.Port = ClientAddr.sin_port;
		char* c_address = new char[15]{ 0 };
		sprintf_s(c_address, 14, "%d.%d.%d.%d", ClientAddr.sin_addr.S_un.S_un_b.s_b1, ClientAddr.sin_addr.S_un.S_un_b.s_b2, ClientAddr.sin_addr.S_un.S_un_b.s_b3, ClientAddr.sin_addr.S_un.S_un_b.s_b4);
		skt.Address = c_address;
		delete[] c_address;
		return skt;
	}
	return NULL;
}
inline int Socket::Write(const char* buff, int size) const
{
	int totalCount = 0;
	const char* ptr = buff;
	for (; totalCount != size;) {
		int result = ::send(socket, buff, size - totalCount, 0);
		if (result == SOCKET_ERROR) {
			printf("send failed: %d\n", WSAGetLastError());
			break;
		}
		else {
			ptr += result;
			totalCount += result;
		}
	}
	return totalCount;
}
inline void Socket::Close() const {
	::closesocket(this->socket);
}
inline Socket::Socket()
{
	sockaddr.sin_family = AF_INET;
	Init();
}
inline Socket::Socket(SOCKET socket)
{
	this->socket = socket;
}
inline Socket::~Socket()
{

}

