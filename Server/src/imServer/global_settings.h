/*
 * global_settings.h
 *
 *  Created on: Mar 12, 2013
 *      Author: yaowei
 */

#ifndef GLOBAL_SETTINGS_H_
#define GLOBAL_SETTINGS_H_
#include <string>

class CGlobalSettings
{
public:
	int remote_listen_port_;
	unsigned int thread_num_;

	std::string relay_server_ip_;
	int relay_server_port_;

	std::string redis_ip_;
	int redis_port_;

	int bind_port_;

	int client_heartbeat_timeout_;
};

#endif /* GLOBAL_SETTINGS_H_ */
