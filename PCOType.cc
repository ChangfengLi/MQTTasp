/******************************************************************************
 * Copyright (c) 2000-2021 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   Balasko, Jeno
 *   Lovassy, Arpad
 *   Szabo, Janos Zoltan – initial implementation
 *
 ******************************************************************************/
// This Test Port skeleton source file was generated by the
// TTCN-3 Compiler of the TTCN-3 Test Executor version CRL 113 200/4 R2A
// for Arpad Lovassy (earplov@esekilxxen1841) on Tue Jul 22 16:49:55 2014
// You may modify this file. Complete the body of empty functions and
// add your member functions here.

#include "PCOType.hh"
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

namespace MyExample {

PCOType::PCOType(const char *par_port_name)
	: PCOType_BASE(par_port_name),target_fd(-1)
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

PCOType::~PCOType()
{
	//puts("PCOType::~PCOType\n\n\n");
}

void PCOType::set_parameter(const char * parameter_name,
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

void PCOType::setUpSocket()
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

void PCOType::closeDownSocket()
{
  //log("entering UDPasp__PT::closeDownSocket()");
  close(target_fd);
  target_fd = -1;
  //log("entering UDPasp__PT::closeDownSocket()");
}

/*
unsigned long PCOType::getHostId(const char* hostName)
{
    //log("UDPasp__PT::getHostId called");
    unsigned long ipBrokerAddress = 0;

    if(strcmp(hostName, "255.255.255.255") == 0) {
      ipBrokerAddress = 0xffffffff;
    } else {
      in_addr_t addr = inet_addr(hostName);
      if (addr != (in_addr_t) - 1) {     // host name in XX:XX:XX:XX form
        ipBrokerAddress = addr;
      }
      else {                               // host name in domain.com form
        struct hostent* hptr;
        if ((hptr = gethostbyname(hostName)) == 0)
          TTCN_error("The host name %s is not valid.", hostName);
          ipBrokerAddress = *((unsigned long*)hptr->h_addr_list[0]);
      }
    }

   // log("Host name: %s, Host BrokerAddress: %u", (const char*)hostName, ipBrokerAddress);
    //log("UDPasp__PT::getHostId exited");

    return htonl ( ipBrokerAddress );
}
*/


void PCOType::Event_Handler(const fd_set *read_fds,
	const fd_set *write_fds, const fd_set *error_fds,
	double time_since_last_call)
{
	//puts("Begin of  PCOType::Event_Handler\n");//标记
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

/*void PCOType::Handle_Fd_Event(int fd, boolean is_readable,
	boolean is_writable, boolean is_error) {}

void PCOType::Handle_Fd_Event_Error(int )
{

}

void PCOType::Handle_Fd_Event_Writable(int )
{

}

void PCOType::Handle_Fd_Event_Readable(int )
{

}

void PCOType::Handle_Timeout(double time_since_last_call) {}*/

void PCOType::user_map(const char *system_port)
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

void PCOType::user_unmap(const char *system_port)
{
	//puts("Begin of MQTTasp::user_unmap\n");
	if ((rc = MQTTClient_disconnect(client, 10000)) != MQTTCLIENT_SUCCESS)
    	printf("Failed to disconnect, return code %d\n", rc);
    MQTTClient_destroy(&client);
	closeDownSocket();
	Uninstall_Handler();
}

void PCOType::user_start()
{

}

void PCOType::user_stop()
{

}

void PCOType::outgoing_send(const CHARSTRING& send_par)
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

void PCOType::outgoing_send(const MQTT__Data& send_par)
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

void PCOType::delivered(void *context, MQTTClient_deliveryToken dt)
{
    //printf("Message with token value %d delivery confirmed\n", dt);
    //deliveredtoken = dt;
}

int PCOType::msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
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

void PCOType::connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

} /* end of namespace */
