#include "netNao.h"
#include "gen.h"

#include <iostream>
//#include <stdio.h>

using namespace std;


NetNao::NetNao(boost::shared_ptr<AL::ALBroker> broker, 
	const string &name)
:	ALModule(broker, name),
	mode(CONN_INACTIVE)
{
	setModuleDescription("This Module enables the setup of a Server for TCP Communication.");
	
	functionName("bindTcp", getName(), "Bind to a port for listening.");
	addParam("port", "specify Port");
	setReturn("int", "return the Server Socket");
	BIND_METHOD(NetNao::bindTcp);	

	functionName("singleListen", getName(), "Listen for a single Client to connect");
	addParam("sockServer", "Server Socket returned by bindTcp()");
	BIND_METHOD(NetNao::singleListen);

	functionName("acceptClient", getName(), "Accept the Client");
	addParam("sockServer", "Server Socket returned by bindTcp()");
	setReturn("int", "return the Client Socket");
	BIND_METHOD(NetNao::acceptClient);

	functionName("sendString", getName(), "send data");
	addParam("sockClient", "Client Socket returned by acceptClient()");
	addParam("buf", "Buffer for Data (Bytestream)");
	addParam("len", "length of data to stream");
	setReturn("int", "return the bytes sent");
	BIND_METHOD(NetNao::sendString);

	functionName("recvData", getName(), "recv data");
	addParam("sockClient", "Client Socket returned by acceptClient()");
	addParam("buf", "Buffer for Data (Bytestream)");
	addParam("len", "length of data to stream");
	setReturn("int", "return the bytes sent");
	BIND_METHOD(NetNao::recvData);	

	functionName("disconnect", getName(), "disconnect Client");
	addParam("sockClient", "Client Socket");
	BIND_METHOD(NetNao::disconnect);

	functionName("unbind", getName(), "unbind from port");
	addParam("sockServer", "Server Socket");
	BIND_METHOD(NetNao::unbind);
	
	
	functionName("getClient_tcp", getName(), "get client socket the module is connected to");
	BIND_METHOD(NetNao::getClient_tcp);
	
	
	functionName("getServer_tcp", getName(), "get server socket");
	BIND_METHOD(NetNao::getServer_tcp);
	
	functionName("writePipe", getName(), "recv data");
	addParam("writer", "Writing End-Filedescriptor of the pipe");
	addParam("buf", "");
	addParam("len", "length of the data in buf");
	BIND_METHOD(NetNao::writePipe);	
	
	sclient_tcp = 0;
	sserver_tcp= 0;
}

NetNao::~NetNao(){};
void NetNao::init()
{
	
};

void NetNao::writePipe(const int& writer, const AL::ALValue& buf, const int& len)
{
//	for (int i = 0; i < len; ++i)
//		cout<< "[NETNAO] " << i << endl;
	char* buffer = (char*)(const void*)(buf);
	
	write(writer, buffer, len);
}


int NetNao::bindTcp(const string& port)
{
	int status = 0;
	struct addrinfo hints;
	struct addrinfo *servinfo;
	int optval = 1;
	
	memset (&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	
	mode = CONN_INACTIVE;
	status = getaddrinfo(NULL, port.c_str(), &hints, &servinfo);
	sserver_tcp = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	setsockopt(sserver_tcp, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
	status = bind(sserver_tcp, servinfo->ai_addr, servinfo->ai_addrlen);
	
	freeaddrinfo(servinfo);
	
	return sserver_tcp;
}

void NetNao::singleListen(const int& sockServer)
{	
	if (sockServer)
		listen(sockServer, 1);
}

int NetNao::acceptClient(const int& sockServer)
{
	struct sockaddr_storage incomming;
	static char ip[IP_LEN] = {0,};
	socklen_t addr_size;
	unsigned char* i = 0;
	unsigned int addr;
	if (!sockServer)
		return 0;
	addr_size = sizeof(incomming);
	sclient_tcp = accept(sockServer, (struct sockaddr*)&incomming, &addr_size);
	if (sclient_tcp == -1)
		mode = CONN_INACTIVE;
	else
		mode = CONN_ACTIVE;	
	
	addr = ((((struct sockaddr_in*)(&incomming))->sin_addr).s_addr);
	i = (unsigned char*)&addr;
	sprintf(ip, "%u.%u.%u.%u", i[0], i[1], i[2], i[3]);
	//ip4 = (string)itoa(i[0]) + (string)"." + (string)itoa(i[1]) + (string)"." + (string)itoa(i[2]) + (string)"." + (string)itoa(i[3]) + (string);
	//inet_ntop(AF_INET, &(((struct sockaddr_in*)(&incomming))->sin_addr), ip, IP_LEN);
	
	ip4 = ip;
	cout<< "[ACCEPT] IP = " << ip << " = " << ntohl((((struct sockaddr_in*)(&incomming))->sin_addr).s_addr) <<  endl;
	return sclient_tcp;
}

void NetNao::disconnect(const int& sockClient)
{
	if(sockClient == sclient_tcp)
		sclient_tcp = 0;
	close(sockClient);
	mode = CONN_INACTIVE;
}


void NetNao::unbind(const int& sockServer)
{	
	if(sockServer == sserver_tcp)
		sserver_tcp = 0;
	close(sockServer);
	mode = CONN_INACTIVE;
}

int NetNao::sendString(const int& sockClient, const AL::ALValue& buf, 
					 const int& len, const int& indexStart)
{
	int result;
	string buffer = string(buf);
	cout<< "sending >" << &(buffer.c_str()[indexStart]) << "<" << endl;
	if (sockClient && (mode == CONN_ACTIVE))
		try
		{
			result = send(sockClient, &(buffer.c_str()[indexStart]), len, MSG_DONTWAIT);
			if (result == -1)
				mode = CONN_INACTIVE;
		}
		catch (const AL::ALError& e)
		{
			cout<< "ERROR [NetNao]<sendString>:" << endl << e.what() << endl;
		}
	else 
		result = -2;
	return result;
}

int NetNao::recvData(const int& sockClient, const boost::shared_ptr<char*>& buf, 
					 const unsigned int& len, const unsigned int& indexStart)
{
	int result = 0;
	if (sockClient && (mode == CONN_ACTIVE))
	{
		result = recv(sockClient, &((*buf.get())[indexStart]), len, 0);
		if (result <= 0)
			mode = CONN_INACTIVE;
	}
	else 
		result = -1;	
	
	if (result == 0)
		return SOCK_CLOSED;
	if (result == -1)
		return SOCK_LOST;
	
	return result;
}


int NetNao::getClient_tcp()
{
	return sclient_tcp;
}

int NetNao::getServer_tcp()
{
	return sserver_tcp;
}
