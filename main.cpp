#include <iostream>
#include <unistd.h>
#include "bike.pb.h"
#include "event.h"
#include "events_def.h"
#include "user_event_handler.h"
#include "DispatchMsgService.h"
#include "NetworkInterface.h"
#include "iniconfig.h"
#include "Logger.h"
#include "sqlconnection.h"
#include "BusProcessor.h"
#include <memory>

using namespace std;

int main(int argc, char** argv)
{

	if (argc != 3)
	{
		printf("Please input shbk <config file path> <log file config>!\n");
		return -1;
	}

	if (!Logger::instance()->init(std::string(argv[2])))
	{
		fprintf(stderr, "init log module failed.\n");
		return -2;
	}

	Iniconfig *config=Iniconfig::getInstance();
	if (!config->loadfile(std::string(argv[1])))
	{
		//printf("load %s failed.\n", argv[1]);
		LOG_ERROR("load %s failed.", argv[1]);
		Logger::instance()->GetHandle()->error("load %s failed.", argv[1]);
		return -3;
	}

	st_env_config conf_args = config->getconfig();
	LOG_INFO("[database] ip: %s port: %d user: %s pwd: %s db: %s  [server] port: %d\n", conf_args.db_ip.c_str(), conf_args.db_port, \
		conf_args.db_user.c_str(), conf_args.db_pwd.c_str(), conf_args.db_name.c_str(), conf_args.svr_port);

	//MysqlConnection mysqlConn;
	std::shared_ptr<MysqlConnection> mysqlconn(new MysqlConnection);
	if (!mysqlconn->Init(conf_args.db_ip.c_str(), conf_args.db_port,
		conf_args.db_user.c_str(), conf_args.db_pwd.c_str(), conf_args.db_name.c_str()))
	{
		LOG_ERROR("Database init failed. exit!\n");
		return -4;
	}

	BusinessProcessor busPro(mysqlconn);
	busPro.init();
	//UserEventHandler ueh1;
	//ueh1.handle(&me);

	DispatchMsgService* DMS = DispatchMsgService::getInstance();
	DMS->open();

	NetworkInterface* NTIF = new NetworkInterface();
	NTIF->start(conf_args.svr_port);

	while (1 == 1)
	{
		NTIF->network_event_dispatch();
		sleep(1);
		LOG_DEBUG("network_event_dispatch ...\n");
	}

	/*
	MobileCodeReqEv* pmcre = new MobileCodeReqEv("18684518289");
	DMS->enqueue(pmcre);
	*/

	sleep(5);
	DMS->close();
	sleep(5);

	return 0;
}