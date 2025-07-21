#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <memory>
#include <mutex>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>   // 先包含 winsock2.h，避免冲突
#include <windows.h>    // 再包含 windows.h
#include <ws2tcpip.h>   // ws2tcpip.h 依赖 winsock2.h，可以放在后面
#pragma comment(lib, "ws2_32.lib")
#endif

class Socket
{
private:
	SOCKET socket = NULL;
	sockaddr_in sockaddr = {};
public:
	static std::vector<std::string> GetIpByName(const std::string& hostname);
public:
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

