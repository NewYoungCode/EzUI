#pragma once
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <winsock.h>
#include <mysql/mysql.h>
#pragma comment(lib,"libmysql.lib")
#include <vector>
#include <string>
/*
mysql的连接类
*/
class MySqlClient
{
private:
	MYSQL mysql;
	std::string host;
	std::string user;
	std::string pwd;
	std::string database;
	std::string charset;
	unsigned int port = 3306;

public:
	bool OpenConn();
	void CloseConn();
	MySqlClient(const std::string& host, unsigned int port, const std::string& user, const std::string& pwd, const std::string& database, const std::string& charset = "utf8");
	/*执行查询*/
	bool  ExecuteQuery(const std::string& sql, std::string& result);
	/*执行增删改*/
	size_t ExecuteNoQuery(const std::string& sql);
	virtual ~MySqlClient();
};

inline bool MySqlClient::OpenConn()
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

inline void MySqlClient::CloseConn()
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

inline bool MySqlClient::ExecuteQuery(const std::string& sql, std::string& resultx)
{
	if (!OpenConn()) {
		return false;
	}
	std::string charset_ = "set names ";
	charset_.append(charset);
	mysql_query(&mysql, charset_.c_str());
	mysql_query(&mysql, sql.c_str());
	MYSQL_RES* result = NULL;
	result = mysql_store_result(&mysql);
	std::vector<std::string> fields;
	//查询失败
	if (!result) {
		mysql_free_result(result);
		CloseConn();
		resultx = "[]";
		return false;
	}
	//获取列名
	size_t filedCount = mysql_num_fields(result);
	for (size_t i = 0; i < filedCount; i++)
	{
		std::string name = mysql_fetch_field(result)->name;
		fields.push_back(name);
		//std::cout << name << std::endl;
	}
	std::string jsonArr = "[";
	//获取每行的数据
	MYSQL_ROW sql_row;
	bool frist = true;
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
		if (frist) {
			frist = false;
		}
	}
	jsonArr.erase(jsonArr.length() - 1, 1);
	jsonArr.append("]");
	//释放
	mysql_free_result(result);
	CloseConn();
	resultx = jsonArr;
	if (rowCount == 0) {
		resultx = "[]";
	}
	return true;
}

inline size_t MySqlClient::ExecuteNoQuery(const std::string& sql)
{
	if (!OpenConn()) {
		return 0;
	}
	std::string charset_ = "set names ";
	charset_.append(charset);
	mysql_query(&mysql, charset_.c_str());
	mysql_query(&mysql, sql.c_str());
	size_t affected_row = mysql_affected_rows(&mysql);
	CloseConn();
	return affected_row;
}

inline MySqlClient::~MySqlClient()
{
	mysql_close(&mysql);
}
