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

	CMysqlConnection* CreateConnection();			//����һ�����ݿ�����
	int CloseConnection();	//����һ�����ݿ�����
	int InitConnectionPool(int size);		//��ʼ�����ݿ����ӳ�
	int DestroyConnectionPool();		//�������ݿ����ӳ�
	
private:
	//static std::mutex m_muLock;
	int m_nMinSize;
	int m_nMaxSize;

	DBConnectionList m_busyList;	//ʹ���е����ӳ�
	DBConnectionList m_idleList;	//�������ӳ�

	//���ݿ���������
	int m_nPort;
	std::string m_strOption;
	std::string m_strHost;
	std::string m_strUser;
	std::string m_strPwd;
	std::string m_strDBName;
	std::string m_strUnixSocket;
	unsigned long m_ulClientFlag;

	std::mutex m_muLock;

	std::thread m_pThread;	//���߳����ڼ���̳߳��е��̵߳�������Ч�ԣ���Ҫ�Ǽ����ж���

public:
	//����ģʽ
	static std::shared_ptr<DBConnectionPool> GetInstance(){
		static std::mutex g_mutex;
		std::lock_guard<std::mutex> lg(g_mutex);
		static std::shared_ptr<DBConnectionPool> pInstance = std::make_shared<DBConnectionPool>();

		return pInstance;
	}
	~DBConnectionPool(void){}

	int Init(std::string option,std::string user,std::string password,std::string host,int port,std::string dbname);
	std::shared_ptr<CMysqlConnection> GetConnection();	//������ݿ�����
	int ReleaseConnection(std::shared_ptr<CMysqlConnection>& pConnection);	//���������ӷŻص��������ӳ���
	int AddConnection();		//����һ�����ݿ����ӵ����ж�����
};

