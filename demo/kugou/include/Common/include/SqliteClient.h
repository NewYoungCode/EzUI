#pragma once
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <winsock.h>
#include <vector>
#include <string>

#include "JsonCpp.hpp"

/*
sqlite的连接类
*/
class SqliteClient
{
private:
	std::string dbPath;
	void* db = nullptr;
public:
	bool OpenConn();
	void CloseConn();
	SqliteClient(const std::string& dbPath);

	/*执行查询*/
	bool ExecuteQuery(const std::string& sql, std::string& result);
	/*执行增删改*/
	size_t ExecuteNoQuery(const std::string& sql);

	size_t Insert(const std::string& tableName, const Json::Value& jv);
	size_t Update(const std::string& tableName, const Json::Value& jv, const std::string& whereText);
	bool ExecuteQuery(const std::string& sql, Json::Value& result);

	~SqliteClient();
};