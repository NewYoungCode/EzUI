#pragma once
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <winsock.h>
#include <mysql/mysql.h>
#pragma comment(lib,"libmysql.lib")
#include <vector>
#include <string>

#include "JsonValue.h"
/*
mysql的连接类
*/
class MySqlClient
{
public:
	//处理防注入的
	class PreparedStatement {
		struct Pair {
			char type;
			int value_int;//type 0
			std::string value_str;//type 1
		};
		std::string sql;
		std::vector<Pair> values;
		friend MySqlClient;
	public:
		PreparedStatement(const std::string& sql) :sql(sql) {
		}
		PreparedStatement& Replace(int value) {
			values.push_back({ 0,value });
			return *this;
		}
		PreparedStatement& Replace(const std::string& value) {
			values.push_back({ 1,0,value });
			return *this;
		}
	};

private:
	std::string host;
	std::string user;
	std::string pwd;
	std::string database;
	std::string charset;
	unsigned int port = 3306;
private:
	static size_t get_data(MYSQL_RES* result, std::string& jsonArr) {
		std::vector<std::string> fields;
		//获取列名
		size_t filedCount = mysql_num_fields(result);
		for (size_t i = 0; i < filedCount; i++)
		{
			std::string name = mysql_fetch_field(result)->name;
			fields.push_back(name);
			//std::cout << name << std::endl;
		}
		jsonArr = "[";
		//获取每行的数据
		MYSQL_ROW sql_row;
		size_t rowCount = 0;
		while (sql_row = mysql_fetch_row(result))//获取具体的数据
		{
			rowCount++;
			jsonArr.append("{");
			bool frist_ = true;
			for (size_t i = 0; i < filedCount; i++)
			{
				if (sql_row[i]) {
					jsonArr.append("\"" + fields[i] + "\":\"" + sql_row[i] + "\"");
				}
				else {
					jsonArr.append("\"" + fields[i] + "\":\"\"");
				}
				jsonArr.append(",");
			}
			jsonArr.erase(jsonArr.length() - 1, 1);
			jsonArr.append("},");
		}
		jsonArr.erase(jsonArr.length() - 1, 1);
		jsonArr.append("]");
		if (rowCount == 0) {
			jsonArr = "[]";
		}
		return rowCount;
	}
public:
	bool OpenConn(MYSQL& mysql);
	void CloseConn(MYSQL& mysql);
	MySqlClient(const std::string& host, unsigned int port, const std::string& user, const std::string& pwd, const std::string& database, const std::string& charset = "utf8mb4");
	/*执行查询*/
	bool  ExecuteQuery(const std::string& sql, std::string& result);
	/*执行增删改*/
	size_t ExecuteNoQuery(const std::string& sql);

	size_t Insert(const std::string& tableName, const Json::Value& jv);
	size_t Update(const std::string& tableName, const Json::Value& jv, const std::string& whereText);
	bool ExecuteQuery(const std::string& sql, Json::Value& result);
	bool ExecuteSTMT(const PreparedStatement& _stmt, std::string& result);
	virtual ~MySqlClient();
};

inline bool MySqlClient::OpenConn(MYSQL& mysql)
{
	const char* unix_socket = NULL;
	unsigned long client_flag = 0;
	mysql_init(&mysql);               //初始化
	if ((mysql_real_connect(&mysql, host.c_str(), user.c_str(), pwd.c_str(), database.c_str(), port, unix_socket, client_flag)) == NULL) //连接MySQL
	{
		//数据库打开失败
		printf("%s\n", mysql_error(&mysql));
		return false;
	}
	else {
		//数据库打开
		return true;
	}
}
inline void MySqlClient::CloseConn(MYSQL& mysql)
{
	mysql_close(&mysql);
}
inline MySqlClient::MySqlClient(const std::string& host, unsigned int port, const std::string& user, const std::string& pwd, const std::string& database, const std::string& charset)
{
	this->host = host;
	this->user = user;
	this->pwd = pwd;
	this->database = database;
	this->charset = charset;
	this->port = port;
}
inline bool MySqlClient::ExecuteSTMT(const PreparedStatement& _stmt, std::string& resultJson) {
	MYSQL mysql;
	if (!OpenConn(mysql)) {
		return false;
	}
	std::string charset_ = "set names ";
	charset_.append(charset);
	mysql_query(&mysql, charset_.c_str());

	MYSQL_BIND* binds = NULL;
	MYSQL_STMT* stmt = NULL;
	MYSQL_RES* result = NULL;
	bool ok = false;
	do
	{
		binds = new MYSQL_BIND[_stmt.values.size()];
		::memset(binds, 0, sizeof(MYSQL_BIND) * _stmt.values.size());
		int pos = 0;
		for (auto& it : _stmt.values) {
			if (it.type == 0) {
				binds[pos].buffer_type = MYSQL_TYPE_LONG;
				binds[pos].buffer = (char*)&it.value_int;
				binds[pos].is_null = 0;
			}
			else if (it.type == 1) {
				binds[pos].buffer_type = MYSQL_TYPE_STRING;
				binds[pos].buffer = (char*)it.value_str.c_str();
				binds[pos].buffer_length = it.value_str.size();
				binds[pos].is_null = 0;
			}
			++pos;
		}
		stmt = mysql_stmt_init(&mysql);
		if (mysql_stmt_prepare(stmt, _stmt.sql.c_str(), _stmt.sql.size()) != 0) {
			std::cerr << "Failed to mysql_stmt_prepare: " << mysql_stmt_error(stmt) << std::endl;
			break;
		}
		if (mysql_stmt_bind_param(stmt, binds) != 0) {
			std::cerr << "Failed to mysql_stmt_bind_param: " << mysql_stmt_error(stmt) << std::endl;
			break;
		}
		//执行
		if (mysql_stmt_execute(stmt) != 0) {
			std::cerr << "Failed to mysql_stmt_execute: " << mysql_stmt_error(stmt) << std::endl;
			break;
		}
		result = mysql_stmt_result_metadata(stmt);
		ok = true;
	} while (false);
	//获取结果
	if (result == NULL) {
		resultJson = "[]";
	}
	else {
		get_data(result, resultJson);
	}
	//释放
	CloseConn(mysql);
	delete[] binds;
	mysql_stmt_close(stmt);
	mysql_free_result(result);
	return ok;
}
inline bool MySqlClient::ExecuteQuery(const std::string& sql, std::string& resultx)
{
	MYSQL mysql;
	if (!OpenConn(mysql)) {
		return false;
	}
	std::string charset_ = "set names ";
	charset_.append(charset);
	mysql_query(&mysql, charset_.c_str());
	mysql_query(&mysql, sql.c_str());
	MYSQL_RES* result = NULL;
	result = mysql_store_result(&mysql);
	//查询失败
	if (!result) {
		mysql_free_result(result);
		CloseConn(mysql);
		resultx = "[]";
		return false;
	}
	get_data(result, resultx);
	//释放
	mysql_free_result(result);
	CloseConn(mysql);
	return true;
}
inline size_t MySqlClient::ExecuteNoQuery(const std::string& sql)
{
	MYSQL mysql;
	if (!OpenConn(mysql)) {
		return false;
	}
	std::string charset_ = "set names ";
	charset_.append(charset);
	mysql_query(&mysql, charset_.c_str());
	mysql_query(&mysql, sql.c_str());
	size_t affected_row = mysql_affected_rows(&mysql);
	CloseConn(mysql);
	return affected_row;
}
inline size_t MySqlClient::Update(const std::string& tableName, const Json::Value& jv, const std::string& whereText) {
	std::string sql = "update " + tableName + " set ";
	for (const auto& key : jv.getMemberNames()) {
		if (jv[key].isNumeric()) {
			sql += (key + "=" + jv[key].toString() + "");
		}
		else if (jv[key].isString()) {
			sql += (key + "='" + jv[key].asString() + "'");
		}
		else {
			sql += (key + "='" + jv[key].toString() + "'");
		}
		sql += ",";
	}
	sql.erase(sql.size() - 1, 1);
	sql += " " + whereText;
	return this->ExecuteNoQuery(sql);
}
inline size_t MySqlClient::Insert(const std::string& tableName, const Json::Value& jv) {
	std::string keys = "(";
	std::string values = "(";
	for (const auto& key : jv.getMemberNames()) {
		keys += "" + key + ",";
		if (jv[key].isNumeric()) {
			values += jv[key].toString() + ",";
		}
		else if (jv[key].isString()) {
			values += "'" + jv[key].asString() + "',";
		}
		else {
			values += "'" + jv[key].asString() + "',";
		}
	}
	keys.erase(keys.size() - 1, 1);
	values.erase(values.size() - 1, 1);
	keys += ")";
	values += ")";
	std::string sql = "insert into " + tableName + " " + keys + "values" + values;
	return this->ExecuteNoQuery(sql);
}
inline bool MySqlClient::ExecuteQuery(const std::string& sql, Json::Value& result)
{
	std::string str;
	auto ret = this->ExecuteQuery(sql, str);
	result = JsonValue(str);
	return ret;
}
inline MySqlClient::~MySqlClient()
{
}
