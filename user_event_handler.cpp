#include "user_event_handler.h"
#include "DispatchMsgService.h"
#include "eventtype.h"
#include "sqlconnection.h"
#include "iniconfig.h"
#include "user_service.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

UserEventHandler::UserEventHandler():iEventHandler("UserEventHandler")
{
	//未来需要实现订阅事件的处理
	DispatchMsgService::getInstance()->subscribe(EEVENTID_GET_MOBLIE_CODE_REQ, this);
	DispatchMsgService::getInstance()->subscribe(EEVENTID_LOGIN_REQ, this);
	thread_mutex_create(&pm_);
}

UserEventHandler::~UserEventHandler()
{
	//未来需要实现退订事件的处理
	DispatchMsgService::getInstance()->unsubscribe(EEVENTID_GET_MOBLIE_CODE_REQ, this);
	DispatchMsgService::getInstance()->unsubscribe(EEVENTID_LOGIN_REQ, this);
	thread_mutex_destroy(&pm_);
}

iEvent* UserEventHandler::handle(const iEvent* ev)
{
	if (ev == NULL)
	{
		//LOG_ERROR("input ev is NULL");
		printf("input ev is NULL");
	}

	u32 eid = ev->get_eid();

	if (eid == EEVENTID_GET_MOBLIE_CODE_REQ)
	{
		return handle_mobile_code_req((MobileCodeReqEv*)ev);
	}
	else if (eid == EEVENTID_LOGIN_REQ)
	{
		return handle_login_req((LoginReqEv*) ev);
	}
	else if (eid == EEVENTID_RECHARGE_REQ)
	{
		//return handle_recharge_req((RechargeEv*) ev);
	}
	else if (eid == EEVENTID_GET_ACCOUNT_BALANCE_REQ)
	{
		//return handle_get_account_balance_req((GetAccountBalanceEv*) ev);
	}
	else if (eid == EEVENTID_LIST_ACCOUNT_RECORDS_REQ)
	{
		//return handle_list_account_records_req((ListAccountRecordsReqEv*) ev);
	}

	return NULL;
}

MobileCodeRspEv* UserEventHandler::handle_mobile_code_req(MobileCodeReqEv* ev)
{
	i32 icode = 0;
	std::string mobile_ = ev->get_mobile();
	//LOG_DEBUG("try to get mobile phone %s validate code .",mobile_.c_str());
	printf("try to get mobile phone % s validate code .\n", mobile_.c_str());

	icode = code_gen();
	thread_mutex_lock(&pm_);
	m2c_[mobile_] = icode;
	thread_mutex_unlock(&pm_);
	printf("mobile : %s, code : %d\n", mobile_.c_str(), icode);

	return new MobileCodeRspEv(200, icode);
}

i32 UserEventHandler::code_gen()
{
	i32 code = 0;
	srand((unsigned int)time(NULL));

	code = (unsigned int)(rand() % (999999 - 100000) + 1000000);

	return code;
}

LoginResEv* UserEventHandler::handle_login_req(LoginReqEv* ev)
{
	LoginResEv* LoginEv = nullptr;
	std::string mobile = ev->get_mobile();
	i32 code = ev->get_icode();

	LOG_DEBUG("try to handle login ev, mobile =%s, code = %d,", mobile.c_str(), code);

	thread_mutex_lock(&pm_);
	auto iter = m2c_.find(mobile);

	if (((iter != m2c_.end()) && (code != iter->second))
		|| (iter == m2c_.end()))
	{
		LoginEv = new LoginResEv(ERRC_INVALID_DATA);
	}
	thread_mutex_unlock(&pm_);

	if (LoginEv) return LoginEv;

	//如果验证成功，则要判断用户在数据库是否存在，不存在则插入用户记录
	std::shared_ptr<MysqlConnection> mysqlconn(new MysqlConnection);

	st_env_config conf_args = Iniconfig::getInstance()->getconfig();

	if (!mysqlconn->Init(conf_args.db_ip.c_str(), conf_args.db_port,
		conf_args.db_user.c_str(), conf_args.db_pwd.c_str(), conf_args.db_name.c_str()))
	{
		LOG_ERROR("UserEventHandler::handle_login_req - Database init failed!\n");
		return new LoginResEv(ERRO_PROCCESS_FAILED);
	}

	UserService us(mysqlconn);
	bool result = false;

	if (!us.exist(mobile))
	{
		result = us.insert(mobile);
		if (!result)
		{
			LOG_ERROR("insert user(%s) to db failed.", mobile.c_str());
			return new LoginResEv(ERRO_PROCCESS_FAILED);
		}
	}

	return new LoginResEv(ERRC_SUCCESS);
}