#ifndef BRKS_COMMON_DATASEVER_SQLTABLES_H4_
#define BRKS_COMMON_DATASEVER_SQLTABLES_H_

#include "sqlconnection.h"
#include <memory>
#include "glo_def.h"

class SqlTables
{
public:
	SqlTables(std::shared_ptr<MysqlConnection> sqlConn) : sqlconn_(sqlConn)
	{
	}

	bool CreateUserInfo()
	{
        const char* pUserInfoTable = " \
                                   CREATE TABLE IF NOT EXISTS userinfo( \
                                   id           int(16)             NOT NULL primary key auto_increment, \
                                   mobile       varchar(16)         NOT NULL default '1300000000', \
                                   username     varchar(128)        NOT NULL default '', \
                                   verify       int(4)              NOT NULL default '0', \
                                   registertm   timestamp           NOT NULL default CURRENT_TIMESTAMP, \
                                   money        int(4)              NOT NULL default 0, \
                                   INDEX        mobile_index(mobile) \
                                   )";

        if (!sqlconn_->Execute(pUserInfoTable))
        {
            LOG_ERROR("create table userinfo table failed. error msg:%s", sqlconn_->GetErrInfo());
            return false;
        }

        return true;
	}

    bool CreateBikeTable()
    {
        const char* pBikeInfoTable = " \
                                   CREATE TABLE IF NOT EXISTS bikeinfo( \
                                   id           int               NOT NULL primary key auto_increment, \
                                   devno        int               NOT NULL, \
                                   status       tinyint(1)        NOT NULL default 0, \
                                   trouble      int               NOT NULL default 0, \
                                   tmsg         varchar(256)      NOT NULL default '', \
                                   latitude     double(10,6)      NOT NULL default 0, \
                                   longitude    double(10,6)      NOT NULL default 0, \
                                   unique(devno)   \
                                   )";

        if (!sqlconn_->Execute(pBikeInfoTable))
        {
            LOG_ERROR("create table bikeinfo table failed. error msg:%s", sqlconn_->GetErrInfo());
            return false;
        }

    }

private:
	std::shared_ptr<MysqlConnection> sqlconn_;
};






#endif 

