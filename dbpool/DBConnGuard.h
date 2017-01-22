#pragma once
#include "MysqlConnection.h"
#include "DBConnectionPool.h"

class CDBConnGuard
{
public:
	CDBConnGuard(std::shared_ptr<CMysqlConnection>& spConn){
		spConn = DBConnectionPool::GetInstance()->GetConnection();
		m_pGuardConn = spConn;
	}
	~CDBConnGuard(void){
		DBConnectionPool::GetInstance()->ReleaseConnection(m_pGuardConn);
	}

private:
	std::shared_ptr<CMysqlConnection> m_pGuardConn;
};

