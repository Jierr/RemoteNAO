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

	functionName("sendData", getName(), "send data");
	addParam("sockClient", "Client Socket returned by acceptClient()");
	addParam("buf", "Buffer for Data (Bytestream)");
	addParam("len", "length of data to stream");
	setReturn("int", "return the bytes sent");
	BIND_METHOD(NetNao::sendData);

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
}

NetNao::~NetNao(){};
void NetNao::init(){};


int NetNao::bindTcp(const string& port)
{
	int status = 0;
	int sserver = 0;
	struct addrinfo hints;
	struct addrinfo *servinfo;
	int optval = 1;

	memset (&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	status = getaddrinfo(NULL, port.c_str(), &hints, &servinfo);
	sserver = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	setsockopt(sserver, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
	status = bind(sserver, servinfo->ai_addr, servinfo->ai_addrlen);	
	freeaddrinfo(servinfo);

	return sserver;
}

void NetNao::singleListen(const int& sockServer)
{	
	listen(sockServer, 1);
}

int NetNao::acceptClient(const int& sockServer)
{	
	int sclient = 0;
	struct sockaddr_storage incomming;
	socklen_t addr_size;

	addr_size = sizeof(incomming);
	sclient = accept(sockServer, (struct sockaddr*)&incomming, &addr_size);
	return sclient;
}

void NetNao::disconnect(const int& sockClient)
{
	close(sockClient);
}


void NetNao::unbind(const int& sockServer)
{
	close(sockServer);
}

int NetNao::sendData(const int& sockClient, const boost::shared_ptr<char*>& buf, 
					 const unsigned int& len, const unsigned int& indexStart)
{
	return send(sockClient,  &((*buf.get())[indexStart]), len, 0);
}

//shared_ptr<scoped_ptr<Resource>>
int NetNao::recvData(const int& sockClient, const boost::shared_ptr<char*>& buf, 
					 const unsigned int& len, const unsigned int& indexStart)
{
	int result = 0;
	//char* nbuf = new char[len];
	//memcpy(nbuf, buf.get(), len);
	result = recv(sockClient, &((*buf.get())[indexStart]), len, 0);	
	//boost::shared_ptr<char> change(nbuf);
	return result;
	

	//delete[] nbuf;

}
