#pragma once
#include "General.h"
#if USEJSONCPP
#include <json/json.h>
#ifdef  _DEBUG
#pragma comment (lib,"json_libd.lib")
#else
#pragma comment (lib,"json_lib.lib")
#endif
struct JObject :public Json::Value {
private:
	bool b = false;
	Json::Reader rd;
public:
	bool IsJson();
	JObject(const Json::Value&right);
	JObject& operator = (const Json::Value &other);
	JObject(const std::string&jsonStr);
};
#endif