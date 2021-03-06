//============================================================================
// Name        : iMServer.cpp
// Author      : yaocoder
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <signal.h>
#include "defines.h"
#include "init_configure.h"
#include "master_thread.h"
#include "redis_conn_pool.h"
#include "global_settings.h"
#include "../public/utils.h"
#include "../public/config_file.h"

static void InitConfigure();
static void SettingsAndPrint();
static void InitRedis();
static void Run();
static void SigUsr(int signo);

int main(int argc, char **argv)
{

	/* process arguments */
	int c;
	std::string version = std::string("1.0.387");
	while (-1 != (c = getopt(argc, argv, "v" /* 获取程序版本号，配合svn */
	)))
	{
		switch (c)
		{
		case 'v':
			printf("The version is %s\n", version.c_str());
			return EXIT_SUCCESS;
		default:
			break;
		}
	}

	InitConfigure();

	SettingsAndPrint();

	if(signal(SIGUSR1, SigUsr) == SIG_ERR)
	{
		LOG4CXX_FATAL(g_logger, "Configure signal failed.");
		exit(EXIT_FAILURE);
	}

	InitRedis();

	if (daemon(1, 0) == -1)
	{
		LOG4CXX_FATAL(g_logger, "daemon failed.");
	}

	Run();

	return EXIT_SUCCESS;
}

void Run()
{
	CMasterThread masterThread;
	if(!masterThread.InitMasterThread())
	{
		LOG4CXX_FATAL(g_logger, "InitMasterThread failed.");
		exit(EXIT_FAILURE);
	}

	masterThread.Run();
}

void SigUsr(int signo)
{
	if(signo == SIGUSR1)
	{
		/* 重新加载应用配置文件（仅仅是连接超时时间），log4cxx日志配置文件*/
		InitConfigure();
		SettingsAndPrint();
		LOG4CXX_INFO(g_logger, "reload configure.");
		return;
	}
}

void InitConfigure()
{
	CInitConfig initConfObj;
	initConfObj.SetConfigFilePath(std::string("/home/accountSys/conf/"));
	std::string project_name = "imServer";
	initConfObj.InitLog4cxx(project_name);
	if (!initConfObj.LoadConfiguration(project_name))
	{
		LOG4CXX_FATAL(g_logger, "LoadConfiguration failed.");
		exit(EXIT_FAILURE);
	}
}

void InitRedis()
{
	RedisConnInfo redisConnInfo;
	redisConnInfo.max_conn_num = 4;
	redisConnInfo.ip = utils::G<ConfigFile>().read<std::string> ("redis.ip", "127.0.0.1");
	redisConnInfo.port = utils::G<ConfigFile>().read<int> ("redis.port", 6379);
	if (!CRedisConnPool::GetInstance()->Init(redisConnInfo))
	{
		LOG4CXX_FATAL(g_logger, "Init redisConnPool failed.");
		exit(EXIT_FAILURE);
	}
}

void SettingsAndPrint()
{
	utils::G<CGlobalSettings>().remote_listen_port_ = utils::G<ConfigFile>().read<int> ("imServer.remote.listen.port", 12004);
	utils::G<CGlobalSettings>().thread_num_ 		= utils::G<ConfigFile>().read<int> ("imServer.worker.thread.num", 4);
	utils::G<CGlobalSettings>().relay_server_ip_ 	= utils::G<ConfigFile>().read<std::string> ("imRelay.server.ip","127.0.0.1");
	utils::G<CGlobalSettings>().relay_server_port_ 	= utils::G<ConfigFile>().read<int> ("imRelay.server.listen.port", 6111);
	utils::G<CGlobalSettings>().bind_port_ 			= utils::G<ConfigFile>().read<int> ("imServer.bind.port", 7001);
	utils::G<CGlobalSettings>().client_heartbeat_timeout_ = utils::G<ConfigFile>().read<int>("imServer.client.timeout.s", 70);

	LOG4CXX_INFO(g_logger, "******imServer.remote.listen.port = " << utils::G<CGlobalSettings>().remote_listen_port_ << "******");
	LOG4CXX_INFO(g_logger, "******imServer.worker.thread.num = "  << utils::G<CGlobalSettings>().thread_num_ << "******");
	LOG4CXX_INFO(g_logger, "******imRelay.server.ip = " 		  << utils::G<CGlobalSettings>().relay_server_ip_ << "******");
	LOG4CXX_INFO(g_logger, "******imRelay.server.listen.port = "  << utils::G<CGlobalSettings>().relay_server_port_ << "******");
	LOG4CXX_INFO(g_logger, "******imServer.bind.port = " 		  << utils::G<CGlobalSettings>().bind_port_ << "******");
	LOG4CXX_INFO(g_logger, "******imServer.client.timeout.s = "   << utils::G<CGlobalSettings>().client_heartbeat_timeout_ << "******");
}

