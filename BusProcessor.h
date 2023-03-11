#ifndef BRKS_BUS_MAIN_H_
#define BRKS_BUS_MAIN_H_

#include "user_event_handler.h"
#include "sqlconnection.h"

class BusinessProcessor
{
public:
	BusinessProcessor(std::shared_ptr<MysqlConnection> conn);
	bool init();
	virtual ~BusinessProcessor();

private:
	std::shared_ptr<MysqlConnection> mysqlconn_;
	std::shared_ptr<UserEventHandler> ueh_;    //账户管理系统
};







#endif 

