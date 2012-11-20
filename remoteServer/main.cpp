#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "netNao.h"

#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
#include <alcommon/albrokermanager.h>
#include <alerror/alerror.h>
#include <alproxies/altexttospeechproxy.h>
#include <qi/os.hpp>


using namespace std;


int main(int argc, char* argv[])
{
	int pport = 9559;
	string pip = "127.0.0.1";

	if(argc > 2)
	{
		if(string(argv[1]) == "--pip")
		{
			pip = argv[2];
			if (argc > 4)
			{
				if(string(argv[3]) == "--pport")
					pport = atoi(argv[4]);
			} 
		}
		else if (string(argv[1]) == "--pport")
		{
			pport = atoi(argv[2]);
			if (argc > 4)
			{
				if(string(argv[3]) == "--pip")
					pip = argv[4];
			} 
		}
	}
	else 
	{
		pport = 9559;
		pip = "127.0.0.1";
	}	


	//for SOAP serializations of floats
	setlocale(LC_NUMERIC, "C");	
	
	const string brokerName = "AppToNAO_BROKER";
	//assign open port via port 0 from os
	//will be the port of the newly created broker
	int brokerPort = 50000;
	//ANYIP
	const string brokerIp = "0.0.0.0";
	boost::shared_ptr<AL::ALBroker> broker;
	try
	{
		broker = AL::ALBroker::createBroker(
			brokerName,
			brokerIp,
			brokerPort,
			pip,
			pport);
	}
	catch(...)
	{
		AL::ALBrokerManager::getInstance()->killAllBroker();
		AL::ALBrokerManager::kill();
	}
	
	//kills old BrokerManager Singleton and replaces it with a new one
	AL::ALBrokerManager::setInstance(broker->fBrokerManager.lock());
	AL::ALBrokerManager::getInstance()->addBroker(broker);

	static boost::shared_ptr<NetNao> net = \
		AL::ALModule::createModule<NetNao>(broker, "NetNao");
	
	//NetNao* net = new NetNao(broker, "NetNao");
	string port = "32768";
	int sserver = 0;
	int sclient = 0;
	char buf[255] = {0,};
	unsigned int bytesRead = 0; 
	const std::string msg = "Hello there, everything is initialized.";

	sserver = net->bindTcp(port);
	while (1)
	{
		net->singleListen(sserver);
		sclient = net->acceptClient(sserver);
		strcpy(buf, "[Remote NAO] Willkommen!\r\n");
		cout<< "Sende Daten\r\n";
		//send(sclient, buf, strlen(buf), 0);
		net->sendData(sclient, (const char*)buf, strlen(buf));
		
		cout<< "Sende Daten beendet\r\n";
		try 
		{
			AL::ALTextToSpeechProxy tts(pip, pport);
			tts.say(msg);
		}
		catch(const AL::ALError& e)
		{
			cerr<< "EXCEPTION: " << e.what() << endl;
		}
		while(buf[0] != '#')
		{
			//bytesRead = recv(sclient, buf, 1, 0);
			cout<< "receive...\r\n";
			bytesRead = 0;
			do
			{
				boost::shared_ptr<char*> buffer(new char*(&buf[bytesRead]));
				bytesRead += net->recvData(sclient, buffer, 1);
				cout<< "receive done: [" << bytesRead << "] = " << buf[bytesRead-1] << endl;	
			}
			while (buf[bytesRead-1] != '+');

			buf[bytesRead-1] = 0;			
			try 
			{
				AL::ALTextToSpeechProxy tts(pip, pport);
				const string str = string(buf);
				tts.say(str);
			}
			catch(const AL::ALError& e)
			{
				cerr<< "EXCEPTION: " << e.what() << endl;
			}			
		}
		net->disconnect(sclient);
		net->unbind(sserver);
		exit(0);
	}
	
	
	//delete net;

	while(1)
		qi::os::sleep(1);

	



/*	
	char msg[255]={0,};
	int status = 0;
	struct in_addr ip;
	char ipStr[20]= {0,};
	unsigned short int port = 0;
	struct addrinfo hints;
	struct addrinfo *servinfo;
	struct sockaddr_storage incomming;
	socklen_t addr_size;

	int sserver = 0;
	int sclient = 0;
	int optval = 1;

	memset (&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	status = getaddrinfo(NULL, "32768", &hints, &servinfo);
//	printf("IP Address for host:\r\n");
	ip = ((struct sockaddr_in*)servinfo->ai_addr)->sin_addr;
	port = ((struct sockaddr_in*)servinfo->ai_addr)->sin_port;
	inet_ntop(servinfo->ai_family, &ip, ipStr, 20);
//	printf("%s:%d\r\n", ipStr, ntohs(port));
//	printf("Get a Socket...\r\n");
	sserver = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	setsockopt(sserver, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
//	printf("Bind to Port...\r\n");
	bind(sserver, servinfo->ai_addr, servinfo->ai_addrlen);

	while (1)
	{
//		printf("Listen...\r\n");
		listen(sserver, 1);
		addr_size = sizeof(incomming);
		sclient = accept(sserver, (struct sockaddr*)&incomming, &addr_size);
//		printf("Verbindung akzeptiert...\r\n");
//		printf("Sende Message...\r\n");
		strcpy(msg, "[Remote Server NAO] Willkommen\r\n");
		send(sclient, msg, strlen(msg),0);
		strcpy(msg, "[Remote Server NAO] Tschueß\r\n");
		send(sclient, msg, strlen(msg),0);
		close(sclient);
	}
	close(sserver);
	freeaddrinfo(servinfo);
	return 0;
*/
}
