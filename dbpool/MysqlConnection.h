#pragma once
#include <mysql.h>
#include <string>
#include <mutex>
#include <memory>

class CMysqlConnection
{
private:
	bool m_bValid;

	//数据库连接属性
	unsigned int m_unPort;
	unsigned long m_ulClientFlag;
	std::string m_strOption;
	std::string m_strHost;
	std::string m_strUser;
	std::string m_strPwd;
	std::string m_strDbName;
	std::string m_strUnixSocket;

	std::shared_ptr<MYSQL> m_pConnection;
	
public:
	CMysqlConnection(const std::string option, const std::string host, const std::string user,
		const std::string pwd, const std::string dbName, unsigned int port, 
		const std::string unixSocket, unsigned long clientFlag);
	~CMysqlConnection(void);

public:
	bool IsValid();

	int SetOption(enum mysql_option,std::string option);
	int Open();
	int Close();
	int Execute(std::string sql);
	MYSQL_RES* SingleExecute(std::string sql);

};

