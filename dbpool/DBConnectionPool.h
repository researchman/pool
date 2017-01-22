#pragma once
//#include <memory>
//#include <mutex>
#include <iostream>
#include <thread>
#include <list>
#include "MysqlConnection.h"

typedef std::list<std::shared_ptr<CMysqlConnection> > DBConnectionList;

class DBConnectionPool
{
private:
	DBConnectionPool();
	DBConnectionPool& operator =(const DBConnectionPool&);

	CMysqlConnection* CreateConnection();			//创建一个数据库连接
	int CloseConnection();	//销毁一个数据库连接
	int InitConnectionPool(int size);		//初始化数据库连接池
	int DestroyConnectionPool();		//销毁数据库连接池
	
private:
	//static std::mutex m_muLock;
	int m_nMinSize;
	int m_nMaxSize;

	DBConnectionList m_busyList;	//使用中的连接池
	DBConnectionList m_idleList;	//空闲连接池

	//数据库连接属性
	int m_nPort;
	std::string m_strOption;
	std::string m_strHost;
	std::string m_strUser;
	std::string m_strPwd;
	std::string m_strDBName;
	std::string m_strUnixSocket;
	unsigned long m_ulClientFlag;

	std::mutex m_muLock;

	std::thread m_pThread;	//此线程用于检查线程池中的线程的连接有效性，主要是检查空闲队列

public:
	//单例模式
	static std::shared_ptr<DBConnectionPool> GetInstance(){
		static std::mutex g_mutex;
		std::lock_guard<std::mutex> lg(g_mutex);
		static std::shared_ptr<DBConnectionPool> pInstance = std::make_shared<DBConnectionPool>();

		return pInstance;
	}
	~DBConnectionPool(void){}

	int Init(std::string option,std::string user,std::string password,std::string host,int port,std::string dbname);
	std::shared_ptr<CMysqlConnection> GetConnection();	//获得数据库连接
	int ReleaseConnection(std::shared_ptr<CMysqlConnection>& pConnection);	//将工作连接放回到空闲连接池中
	int AddConnection();		//增加一个数据库连接到空闲队列中
};

