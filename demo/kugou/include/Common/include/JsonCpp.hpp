#pragma once
#include <string>
#include <Windows.h>

#define USEJSONCPP 1 //是否使用JsonCpp库 

#if USEJSONCPP
#include <json/json.h>

#ifdef  _WIN64

#ifdef  _DEBUG
#pragma comment (lib,"x64/jsoncppd.lib")
#else
#pragma comment (lib,"x64/jsoncpp.lib")
#endif

#else

#ifdef  _DEBUG
#pragma comment (lib,"x84/jsoncppd.lib")
#else
#pragma comment (lib,"x84/jsoncpp.lib")
#endif

#endif

namespace Json {
	//字符串转换成json对象 失败则返回Json::nullValue
	inline Json::Value Parse(const std::string& jsonStr)
	{
		Json::Reader rd;
		Json::Value out;
		if (rd.parse(jsonStr, out)) {
			return out;
		}
		return Json::nullValue;
	}
};
#endif