#pragma once
#include "General.h"
namespace HttpUtility {
	// NOTE: This code came up with the following stackoverflow post:
	// https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c
	std::string base64_encode(const unsigned char *text, unsigned int text_len);
	std::string  base64_decode(const unsigned char *code, unsigned int code_len);
	//other...
	unsigned char ToHex(unsigned char x);
	unsigned char FromHex(unsigned char x);
	std::string UrlEncode(const std::string& str);
	std::string UrlDecode(const std::string& str);
	//MD5±‡¬Î
	std::string Md5EncodeFormFile(const std::string &filename);
	std::string Md5Encode(const std::string &strdata);
	//…Ÿ“ª∏ˆGZIP
};