#include "stdafx.h"
#include "MysqlConnection.h"

CMysqlConnection::CMysqlConnection(const std::string option, const std::string host, const std::string user,
								   const std::string pwd, const std::string dbName, unsigned int port, 
								   const std::string unixSocket, unsigned long clientFlag)
{
	m_bValid = false;

	m_strOption		= option;
	m_strHost		= host;
	m_strUser		= user;
	m_strPwd		= pwd;
	m_strDbName		= dbName;
	m_unPort		= port;
	m_strUnixSocket	= unixSocket;
	m_ulClientFlag	= clientFlag;

	m_pConnection = std::shared_ptr<MYSQL>(mysql_init(NULL));	

	if(Open() == 0){
		m_bValid = true;
	}
}

CMysqlConnection::~CMysqlConnection(void)
{
	Close();
}

int CMysqlConnection::SetOption(enum mysql_option opt,std::string option)
{
	if (mysql_options(m_pConnection.get(),opt,option.c_str()))
	{
		return 1;
	}
	return 0;
}

int CMysqlConnection::Open()
{
	HRESULT hr = ::CoInitialize(NULL);
	if (FAILED(hr))
	{
		return 1;
	}

	m_ulClientFlag |= CLIENT_MULTI_STATEMENTS;
	//CLIENT_MULTI_RESULTS;
	//CLIENT_MULTI_QUERIES

	if (!mysql_real_connect(
		m_pConnection.get(),
		m_strHost.c_str(),
		m_strUser.c_str(),
		m_strPwd.c_str(),
		m_strDbName.c_str(),
		m_unPort,
		m_strUnixSocket.c_str(),
		m_ulClientFlag
		))
	{
		//����ʧ�ܣ�mysql_real_connect����null
		return 1;
	}

	return 0;
}

int CMysqlConnection::Close()
{
	if (m_bValid)
	{
		mysql_close(m_pConnection.get());
		::CoUninitialize();
		m_bValid = false;

		return 0;
	}

	return 1;
}
MYSQL_RES* CMysqlConnection::SingleExecute(std::string sql)
{
	MYSQL_RES* pRes;

	int nError = mysql_query(m_pConnection.get(),sql.c_str());
	if (nError)
	{
		return nullptr;
	}
	pRes = mysql_use_result(m_pConnection.get());

	return pRes;
}
int CMysqlConnection::Execute(std::string sql)
{
	if (sql == "")
	{
		return 1;
	}

	int nError = mysql_query(m_pConnection.get(),sql.c_str());

	//ÿ������Ӧһ��pRes����Ҫ�ͷ�ÿһ����������������ݴ���
	//��Ȼͨ��mysql_error()���֪��Commands out of sync; you can't run this command now����
	MYSQL_RES* res;
	do 
	{
		res = mysql_use_result(m_pConnection.get());
		mysql_free_result(res);
	} while (mysql_next_result(m_pConnection.get()));

	if (nError)
	{
		return 1;
	}

	return 0;
}

bool CMysqlConnection::IsValid()
{
	return m_bValid;
}
