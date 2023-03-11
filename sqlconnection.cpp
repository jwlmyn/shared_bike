#include "sqlconnection.h"
#include <string.h>

MysqlConnection::MysqlConnection() :mysql_(NULL)
{
	mysql_ = (MYSQL*)malloc(sizeof(MYSQL));
}

MysqlConnection::~MysqlConnection()
{
	if (mysql_ != NULL)
	{
		mysql_close(mysql_);
		free(mysql_);
		mysql_ = NULL;
	}
	return;
}

bool MysqlConnection::Init(const char* szHost, int nPort, const char* szUser, const char* szPasswd, const char* szDb)
{
	LOG_INFO("enter Init\n");
	if ((mysql_ = mysql_init(mysql_)) == NULL)
	{
		LOG_ERROR("init mysql failed %s, %d", this->GetErrInfo(), errno);
		return false;
	}

	char cAuto = 1;
	if (mysql_options(mysql_, MYSQL_OPT_RECONNECT, &cAuto) != 0)//³É¹¦·µ»Ø0
	{
		LOG_ERROR("mysql_options MYSQL_OPT_RECONNECT failed.reason: %s\n",GetErrInfo());
	}

	if (mysql_real_connect(mysql_, szHost, szUser, szPasswd, szDb, nPort, NULL, 0) == NULL)
	{
		LOG_ERROR("connect mysql failed %s", this->GetErrInfo());
		return false;
	}

	return true;
}

bool MysqlConnection::Execute(const char* szSql)
{
	if (mysql_real_query(mysql_, szSql, strlen(szSql)) != 0)
	{
		if (mysql_errno(mysql_) == CR_SERVER_GONE_ERROR)
		{
			Reconnect();
		}
		return false;
	}
	return true;
}

bool MysqlConnection::Execute(const char* szSql, SqlRecordSet& recordSet)
{
	if (mysql_real_query(mysql_, szSql, strlen(szSql)) != 0)
	{
		if (mysql_errno(mysql_) == CR_SERVER_GONE_ERROR)
		{
			Reconnect();
		}
		return false;
	}
	MYSQL_RES* pRes = mysql_store_result(mysql_);
	if (!pRes)
	{
		return NULL;
	}
	recordSet.SetResult(pRes);

	return true;
}

const char* MysqlConnection::GetErrInfo()
{
	return mysql_error(mysql_);
}

void MysqlConnection::Reconnect()
{
	mysql_ping(mysql_);
}

int MysqlConnection::EscapeString(const char* pSrc, int nSrcLen, char* pDest)
{
	if (!mysql_)
	{
		return 0;
	}
	return mysql_real_escape_string(mysql_, pDest, pSrc, nSrcLen);
}