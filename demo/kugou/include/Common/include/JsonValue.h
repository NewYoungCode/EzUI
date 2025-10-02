#pragma once
#include <string>
#include <Windows.h>

#define USEJSONCPP 1 //是否使用JsonCpp库 

#if USEJSONCPP
#include <json/json.h>

#ifdef  _WIN64

#ifdef  _DEBUG
#pragma comment (lib,"X64/json_libd.lib")
#else
#pragma comment (lib,"X64/json_lib.lib")
#endif

#else

#ifdef  _DEBUG
#pragma comment (lib,"json_libd.lib")
#else
#pragma comment (lib,"json_lib.lib")
#endif

#endif


struct JsonValue :public Json::Value {
private:
	bool b = false;
	Json::Reader rd;
public:
	bool IsJson();
	//JObject(const Json::Value& right);
	JsonValue& operator = (const Json::Value& other);
	JsonValue(const std::string& jsonStr);
};
#endif