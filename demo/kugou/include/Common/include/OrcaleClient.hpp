#pragma once
#include <iostream>
#include <occi/occi.h>
using namespace oracle::occi;
#ifdef _DEBUG
#pragma comment(lib,"oraocci12d.lib")
#else
#pragma comment(lib,"oraocci12.lib")
#endif // _DEBUG
#include <string>
class  OrcaleClient
{
private:
	std::string host;
	unsigned int port = 1521;
	std::string user;
	std::string pwd;
	std::string server_name;
	std::string charset;
	Environment* outEnv = NULL;
	std::string dbStr;
public:
	virtual ~OrcaleClient() {
		if (outEnv) {
			Environment::terminateEnvironment(outEnv);
		}
	}
	OrcaleClient(const std::string& host, unsigned int port, const std::string& user, const std::string& pwd, const std::string& server_name, const std::string& charset = "UTF8");
	void CloseConn(Connection* conn);
	Connection* OpenConn();
	bool ExecuteQuery(const std::string& sql, std::string& sqlResult);
	size_t ExecuteNoQuery(const std::string& sql);
};

inline 	Connection* OrcaleClient::OpenConn() {
	try
	{
		auto conn = outEnv->createConnection(user, pwd, dbStr);
		return conn;
	}
	catch (const std::exception& ex)
	{
		printf("%s\n", ex.what());
		return NULL;
	}
}
inline void  OrcaleClient::CloseConn(Connection* conn) {
	outEnv->terminateConnection(conn);
}
inline OrcaleClient::OrcaleClient(const std::string& host, unsigned int port, const std::string& user, const std::string& pwd, const std::string& server_name, const std::string& charset) {
	this->host = host;
	this->port = port;
	this->user = user;
	this->pwd = pwd;
	this->server_name = server_name;
	this->charset = charset;
	this->dbStr = host + ":" + std::to_string(port) + "/" + server_name;
	this->outEnv = Environment::createEnvironment("ZHS16GBK", this->charset);//编码问题
}
inline size_t OrcaleClient::ExecuteNoQuery(const std::string& sql) {
	size_t affRow = 0;
	auto conn = OpenConn();
	if (conn) {
		Statement* stmt = NULL;
		try {
			stmt = conn->createStatement();
			affRow = stmt->executeUpdate(sql);
		}
		catch (SQLException& ex)
		{
			printf("%s\n", ex.what());
		}
		if (stmt) {
			conn->terminateStatement(stmt);
		}
		CloseConn(conn);
	}
	return 0;
}
inline bool	OrcaleClient::ExecuteQuery(const std::string& sql, std::string& sqlResult) {
	bool ok = false;
	auto conn = OpenConn();
	if (conn) {
		std::vector<std::string> fields;
		Statement* stmt = NULL;
		ResultSet* rset = NULL;
		try {
			stmt = conn->createStatement();
			rset = stmt->executeQuery(sql);
			for (auto item : rset->getColumnListMetaData()) {
				std::string s = item.getString(MetaData::ATTR_NAME);
				fields.push_back(s);
			}
			sqlResult = "[";
			//获取每行的数据
			bool frist = true;
			size_t rowCount = 0;
			while (rset->next())//获取具体的数据
			{
				rowCount++;
				sqlResult.append("{");
				bool frist_ = true;
				for (size_t i = 0; i < fields.size(); i++)
				{
					sqlResult.append("\"" + fields[i] + "\":\"" + rset->getString(i + 1) + "\"");
					sqlResult.append(",");
				}
				sqlResult.erase(sqlResult.length() - 1, 1);
				sqlResult.append("},");
				if (frist) {
					frist = false;
				}
			}
			sqlResult.erase(sqlResult.length() - 1, 1);
			sqlResult.append("]");
			if (rowCount == 0) {
				sqlResult = "[]";
			}
			ok = true;
		}
		catch (SQLException& ex)
		{
			printf("%s\n", ex.what());
		}
		if (stmt && rset) {
			stmt->closeResultSet(rset);
			conn->terminateStatement(stmt);
		}
		CloseConn(conn);
	}
	return ok;
}