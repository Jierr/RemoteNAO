#include "netNao.h"


NetNao::NetNao(boost::shared_ptr<AL::ALBroker> broker, 
	const string &name)
:	ALModule(broker, name)
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
	
	sclient_tcp = 0;
	sserver_tcp= 0;
}

NetNao::~NetNao(){};
void NetNao::init()
{
};


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
	socklen_t addr_size;
	
	if (!sockServer)
		return 0;
	addr_size = sizeof(incomming);
	sclient_tcp = accept(sockServer, (struct sockaddr*)&incomming, &addr_size);
	return sclient_tcp;
}

void NetNao::disconnect(const int& sockClient)
{
	if(sockClient == sclient_tcp)
		sclient_tcp = 0;
	close(sockClient);
}


void NetNao::unbind(const int& sockServer)
{	
	if(sockServer == sserver_tcp)
		sserver_tcp = 0;
	close(sockServer);
}

int NetNao::sendString(const int& sockClient, const AL::ALValue& buf, 
					 const int& len, const int& indexStart)
{
	int result;
	string buffer = string(buf);
	//result = send(sockClient,  &((*buf.get())[indexStart]), len, 0);
	if (sockClient)
		result = send(sockClient, &(buffer.c_str()[indexStart]), len, 0);
	else 
		result = -1;
	return result;
}

//shared_ptr<scoped_ptr<Resource>>
int NetNao::recvData(const int& sockClient, const boost::shared_ptr<char*>& buf, 
					 const unsigned int& len, const unsigned int& indexStart)
{
	int result = 0;
	//char* nbuf = new char[len];
	//memcpy(nbuf, buf.get(), len);
	if (sockClient)
		result = recv(sockClient, &((*buf.get())[indexStart]), len, 0);
	else 
		result = -1;	
	//boost::shared_ptr<char> change(nbuf);
	return result;
	

	//delete[] nbuf;
}


int NetNao::getClient_tcp()
{
	return sclient_tcp;
}

int NetNao::getServer_tcp()
{
	return sserver_tcp;
}
