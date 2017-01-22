#include "stdafx.h"
#include "DBConnectionPool.h"

DBConnectionPool::DBConnectionPool()
{
	m_nMinSize = 5;
	m_nMaxSize = 20;
}

/*
 * 创建一个数据库连接
*/
CMysqlConnection* DBConnectionPool::CreateConnection()
{
	CMysqlConnection* conn = nullptr;

	conn = new CMysqlConnection(
		m_strOption,
		m_strHost,
		m_strUser,
		m_strPwd,
		m_strDBName,
		m_nPort,
		m_strUnixSocket,
		m_ulClientFlag
		);

	if (conn->IsValid())
	{
		return conn;
	}

	return nullptr;
}

/*
 * 销毁一个数据库连接
*/
int DBConnectionPool::CloseConnection()
{
	std::shared_ptr<CMysqlConnection>&& sp = std::move(m_idleList.front());
	sp->Close();
	--m_nCurIdleSize;

	return m_nCurIdleSize;
}

int DBConnectionPool::InitConnectionPool(int size)
{
	//std::lock_guard<std::mutex> lg(m_muLock);

	int nSize = 0;
	//如果初始化池大小小于最小值，则取池规定的最小值
	//如果初始化池大小大于最大值，则取池规定的最大值
	//否则取合适的初始化池大小
	if (size < m_nMinSize ){
		nSize = m_nMinSize;
	}else if(size > m_nMaxSize){
		nSize = m_nMaxSize;
	}else{
		nSize = size;
	}

	for (int i=0;i<nSize;i++)
	{
		AddConnection();
	}

	return 0;
}

int DBConnectionPool::DestroyConnectionPool()
{
	for (DBConnectionList::iterator it = m_idleList.begin();it!=m_idleList.end();++it){
		std::shared_ptr<CMysqlConnection>&& sp = std::move(m_idleList.front());
		sp->Close();
	}

	return 0;
}

int DBConnectionPool::Init(std::string option,std::string user,std::string password,std::string host,int port,std::string dbname)
{
	m_strUser = user;
	m_strPwd  = password;
	m_strHost = host;
	m_nPort = port;
	m_strDBName = dbname;

	m_strOption = option;
	m_strUnixSocket = "";
	m_ulClientFlag = 0;

	//InitConnectionPool(size);

	return 0;
}

std::shared_ptr<CMysqlConnection> DBConnectionPool::GetConnection()
{
	std::lock_guard<std::mutex> lg(m_muLock);

	std::shared_ptr<CMysqlConnection> ret = nullptr;

	if (m_idleList.size() > 0)
	{
		ret = m_idleList.front();
		m_idleList.pop_front();
		m_busyList.push_back(ret);
	}else{
		std::cout<<"There is no idle connection to use."<<std::endl;
	}
	
	return ret;
}

int DBConnectionPool::ReleaseConnection(std::shared_ptr<CMysqlConnection>& pConnection)
{
	std::lock_guard<std::mutex> lg(m_muLock);
	if (pConnection!=nullptr)
	{
		std::shared_ptr<CMysqlConnection> sp = pConnection;
		m_busyList.remove(pConnection);
		m_idleList.push_back(sp);
	}
	int ret = m_idleList.size();

	return ret;
}

int DBConnectionPool::AddConnection()
{
	CMysqlConnection* conn = CreateConnection();
	if (conn == nullptr)
	{
		return 1;
	}
	std::shared_ptr<CMysqlConnection> sp(conn,[](CMysqlConnection* conn){
		delete conn;
	});
	m_idleList.push_back(std::move(sp));

	return m_idleList.size();
}

int DBConnectionPool::GetIdleConnection()
{
	std::lock_guard<std::mutex> lg(m_muLock);
	int ret = m_idleList.size();

	return ret;
}

int DBConnectionPool::GetBusyConnection()
{
	std::lock_guard<std::mutex> lg(m_muLock);
	int ret = m_busyList.size();

	return ret;
}

