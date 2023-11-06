#pragma once
#include <iostream>
#include <string>
#include "Socket.hpp"

class HttpClient {
	Socket s;
public:
	HttpClient() {};
	virtual ~HttpClient() {};
	int Get(const std::string& _url, std::string& respone) {
		//http://127.0.0.1/getlog?file=2023-03-09.log
		std::string url = _url;

		size_t pos1 = url.find("http://");
		if (pos1 != size_t(-1)) {
			url = "http://" + url;
		}
		s.Bind("", 80);
	}

};