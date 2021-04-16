/******************************************************************************
 * 本测试端口提供TTCN-3  MQTT通信功能
 * MQTT发布订阅函数调用paho c mqt接口
 * 
 * Contributors:
 *   ChangfengLi
 *
 ******************************************************************************/

#ifndef MQTTasp__PT_HH
#define MQTTasp__PT_HH

#include  <TTCN3.hh>
#include  "MQTTasp_Types.hh"
#include "MQTTClient.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace MQTTasp__Types{//端口数据类型申明
	class MQTT__Data;
}

namespace MQTTasp__PortType {

class MQTTasp__PT_PROVIDER : public PORT {
public:
	MQTTasp__PT_PROVIDER(const char *par_port_name = NULL);
	~MQTTasp__PT_PROVIDER();

	void set_parameter(const char *parameter_name,const char *parameter_value);
	void Event_Handler(const fd_set *read_fds,const fd_set *write_fds, const fd_set *error_fds,
		double time_since_last_call);

//MQTT回调函数
	static void delivered(void *context, MQTTClient_deliveryToken dt);
	static int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);//mqtt sub
	static void connlost(void *context, char *cause);
	volatile MQTTClient_deliveryToken deliveredtoken;

private:
//MQTT客户端句柄
	MQTTClient client;
    MQTTClient_connectOptions conn_opts;
    MQTTClient_message pubmsg;
    MQTTClient_deliveryToken token;
    int rc;

//UDP客户端句柄
	int target_fd;
	struct sockaddr_in localAddr;

//动态配置参数
	char BrokerAddress[250];
	char PubTopic[250],SubTopic[250];

protected:
	void user_map(const char *system_port);
	void user_unmap(const char *system_port);

	void user_start();
	void user_stop();

	void outgoing_send(const CHARSTRING& send_par, FLOAT* timestamp_redirect);
	void outgoing_send(const MQTTasp__Types::MQTT__Data& send_par, FLOAT* timestamp_redirect);
	virtual void incoming_message(const CHARSTRING& incoming_par, const FLOAT& timestamp = FLOAT()) = 0;
	void setUpSocket();
	void closeDownSocket();

};

} /* end of namespace */

#endif
