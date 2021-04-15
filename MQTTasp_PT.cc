/******************************************************************************
 * 本测试端口提供TTCN-3  MQTT通信功能
 * MQTT发布订阅函数调用paho c mqt接口
 * 
 * Contributors:
 *   ChangfengLi
 *
 ******************************************************************************/

#include "MQTTasp_PT.hh"
#include "memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "MQTTClient.h"

#include <stdio.h>


#define CLIENTID    "TTCN_MQTTasp"
#define QOS         1
#define TIMEOUT     10000L

namespace MQTTasp__PortType {

MQTTasp__PT_PROVIDER::MQTTasp__PT_PROVIDER(const char *par_port_name)
	: PORT(par_port_name),target_fd(-1)
{
	 conn_opts = MQTTClient_connectOptions_initializer;
	 pubmsg = MQTTClient_message_initializer;
	 localAddr.sin_family = AF_INET;
  	localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  	localAddr.sin_port = htons(8000);
	target_fd = -1;
	strcpy(BrokerAddress,"tcp://0.0.0.0:1883");
	strcpy(PubTopic,"TTCN2");
	strcpy(SubTopic,"TTCN1");
}

MQTTasp__PT_PROVIDER::~MQTTasp__PT_PROVIDER()
{
	//puts("MQTTasp__PT_PROVIDER::~MQTTasp__PT_PROVIDER\n\n\n");
}

void MQTTasp__PT_PROVIDER::set_parameter(const char * parameter_name,
	const char * parameter_value)
{
	if (!strcmp(parameter_name, "BrokerAddress")) {
		strcpy(BrokerAddress,(char*)parameter_value);
	}else if(!strcmp(parameter_name, "PubTopic")){
		strcpy(PubTopic,(char*)parameter_value);
	}else if(!strcmp(parameter_name, "SubTopic")){
		strcpy(SubTopic,(char*)parameter_value);
	}else
		TTCN_warning("UDPasp__PT::set_parameter(): Unsupported Test Port parameter: %s", parameter_name);
}

void MQTTasp__PT_PROVIDER::setUpSocket()
{
  //log("entering UDPasp__PT::setUpSocket()");

  /* socket creation */
  if((target_fd = socket(AF_INET,SOCK_DGRAM,0))<0) {
    TTCN_error("Cannot open socket \n");
  }
  
  //log("Binding port...");
  if(bind(target_fd, (struct sockaddr *) &localAddr, sizeof(localAddr))<0) {
    TTCN_error("Cannot bind port\n");
  }

  //log("leaving UDPasp__PT::setUpSocket()");
}

void MQTTasp__PT_PROVIDER::closeDownSocket()
{
  //log("entering UDPasp__PT::closeDownSocket()");
  close(target_fd);
  target_fd = -1;
  //log("entering UDPasp__PT::closeDownSocket()");
}

void MQTTasp__PT_PROVIDER::Event_Handler(const fd_set *read_fds,
	const fd_set *write_fds, const fd_set *error_fds,
	double time_since_last_call)
{
	//puts("Begin of  MQTTasp__PT_PROVIDER::Event_Handler\n");//标记
	unsigned char msg[65535];        // Allocate memory for possible messages
  	int msgLength;
  	struct sockaddr_in remoteAddr;
  	socklen_t addr_length = sizeof(remoteAddr);
	if ((msgLength = recvfrom(target_fd, (char*)msg, sizeof(msg), 0, (struct sockaddr*)&remoteAddr, &addr_length)) < 0)
		TTCN_error("Error when reading the received UDP PDU.");
	msg[msgLength] = '\0';  
	puts((char*)msg);
	incoming_message(CHARSTRING(msgLength, (char*)msg));
}

void MQTTasp__PT_PROVIDER::user_map(const char *system_port)
{
	//puts("Begin of MQTTasp::user_map\n");
	if ((rc = MQTTClient_create(&client, (char *)BrokerAddress, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS)
    {
         printf("Failed to create client, return code %d\n", rc);
         exit(EXIT_FAILURE);
    }

	if ((rc = MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to set callbacks, return code %d\n", rc);
        rc = EXIT_FAILURE;
    }

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

	if ((rc = MQTTClient_subscribe(client, SubTopic, QOS)) != MQTTCLIENT_SUCCESS)
    {
    	printf("Failed to subscribe, return code %d\n", rc);
    	rc = EXIT_FAILURE;
    }

	setUpSocket();
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(target_fd, &readfds);
    Install_Handler(&readfds, NULL, NULL, 0.0);
}

void MQTTasp__PT_PROVIDER::user_unmap(const char *system_port)
{
	//puts("Begin of MQTTasp::user_unmap\n");
	if ((rc = MQTTClient_disconnect(client, 10000)) != MQTTCLIENT_SUCCESS)
    	printf("Failed to disconnect, return code %d\n", rc);
    MQTTClient_destroy(&client);
	closeDownSocket();
	Uninstall_Handler();
}

void MQTTasp__PT_PROVIDER::user_start()
{

}

void MQTTasp__PT_PROVIDER::user_stop()
{

}

void MQTTasp__PT_PROVIDER::outgoing_send(const CHARSTRING& send_par)
{
	//puts((const char*)send_par);
	//incoming_message("Hello, TTCN-3!");//receive
	//fflush(stdout);
	char PAYLOAD[256];
	strcpy(PAYLOAD,(const char*)send_par);
	pubmsg.payload = PAYLOAD;
    pubmsg.payloadlen =  (int)strlen(PAYLOAD);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    if ((rc = MQTTClient_publishMessage(client, PubTopic, &pubmsg, &token)) != MQTTCLIENT_SUCCESS)
    {
         printf("Failed to publish message, return code %d\n", rc);
         exit(EXIT_FAILURE);
    }
}

void MQTTasp__PT_PROVIDER::outgoing_send(const MQTTasp__Types::MQTT__Data& send_par)
{
	//puts("outgoing_send(const MQTT__Data& send_par)");
	char PAYLOAD[256];
	strcpy(PAYLOAD,(const char*)send_par.data());
	pubmsg.payload = PAYLOAD;
    pubmsg.payloadlen =  (int)strlen(PAYLOAD);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
	if(send_par.pub().ispresent()){
		if ((rc = MQTTClient_publishMessage(client, (const char*)send_par.pub()(), &pubmsg, &token)) != MQTTCLIENT_SUCCESS)
    	{
         	printf("Failed to publish message, return code %d\n", rc);
         	exit(EXIT_FAILURE);
    	}
		if(send_par.sub().ispresent()){
			if ((rc = MQTTClient_subscribe(client, (const char*)send_par.sub()(), QOS)) != MQTTCLIENT_SUCCESS)
    		{
    			printf("Failed to subscribe, return code %d\n", rc);
    			rc = EXIT_FAILURE;
    		}
		}
	}
	else{
		if ((rc = MQTTClient_publishMessage(client, PubTopic, &pubmsg, &token)) != MQTTCLIENT_SUCCESS)
    	{
         	printf("Failed to publish message, return code %d\n", rc);
         	exit(EXIT_FAILURE);
    	}
		if(send_par.sub().ispresent()){
			if ((rc = MQTTClient_subscribe(client, (const char*)send_par.sub()(), QOS)) != MQTTCLIENT_SUCCESS)
    		{
    			printf("Failed to subscribe, return code %d\n", rc);
    			rc = EXIT_FAILURE;
    		}
		}
	}
}

void MQTTasp__PT_PROVIDER::delivered(void *context, MQTTClient_deliveryToken dt)
{
    //printf("Message with token value %d delivery confirmed\n", dt);
    //deliveredtoken = dt;
}

int MQTTasp__PT_PROVIDER::msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    //printf("Message arrived\n");
    //printf("     topic: %s\n", topicName);
    //printf("   message: %.*s\n", message->payloadlen, (char*)message->payload);

	int sock_fd;  
  
  	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);  
  	/*if(sock_fd < 0)  
  	{  
    	perror("socket");  
    	exit(1);  
  	}	  */
    
  	struct sockaddr_in addr_serv;  
  	int len;  
  	memset(&addr_serv, 0, sizeof(addr_serv));  
  	addr_serv.sin_family = AF_INET;  
  	addr_serv.sin_addr.s_addr =  htonl(INADDR_ANY);
  	addr_serv.sin_port = htons(8000);  
  	len = sizeof(addr_serv);  
    
  	char send_buf[256];   
	strcpy(send_buf,(char*)message->payload);
  	sendto(sock_fd, send_buf, strlen(send_buf), 0, (struct sockaddr *)&addr_serv, len);  
    
  /*if(send_num < 0)  
  {  
    perror("sendto error:");  
    exit(1);  
  }  */

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void MQTTasp__PT_PROVIDER::connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

} /* end of namespace */
