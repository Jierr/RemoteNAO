#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "netNao.h"
#include "manager.h"
#include "gen.h"

#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
#include <alcommon/albrokermanager.h>
#include <alerror/alerror.h>
#include <alcommon/alproxy.h>
#include <alproxies/altexttospeechproxy.h>
#include <qi/os.hpp>


using namespace std;


int main(int argc, char* argv[])
{
	int pport = MB_PORT;
	string pip = MB_IP;

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
		pport = MB_PORT;
		pip = MB_IP;
	}	


	//for SOAP serializations of floats
	//The call to setlocale is very important. Due to SOAP issues, you must make
	//sure your client and your server are using the same LC_NUMERIC settings
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
		//Reset the ALBrokerManager singleton
		AL::ALBrokerManager::kill();
	}
	
	//kills old BrokerManager Singleton and replaces it with a new one
	//fBrokerManager is weak pointer and converted to shared ptr via lock
	//lock additionally checks if there is one reference existing to AlBM
	AL::ALBrokerManager::setInstance(broker->fBrokerManager.lock());
	//Add Broker to the map!
	AL::ALBrokerManager::getInstance()->addBroker(broker);

	cout<< "connect to Broker naoqi" << endl;
	static boost::shared_ptr<NetNao> net = \
		AL::ALModule::createModule<NetNao>(broker, "RMNetNao");	
	cout<< "connected" << endl;
	
	//NetNao* net = new NetNao(broker, "NetNao");
	string port = RM_PORT;
	int taskID = 0;
	int sserver = 0;
	int sclient = 0;
	char buf[255] = {0,};
	char last = 0;
	unsigned int bytesRead = 0; 
	unsigned int bytesSent = 0;
	unsigned int len;
	
	// connect to local module RMManager
	AL::ALProxy proxyManager = AL::ALProxy(string("RMManager"), pip, MB_PORT);
	
	taskID = proxyManager.pCall(string("localRespond")); //callVoid("localRespond");
	cout<<"ID of Thread = " << taskID << endl;
	if (!proxyManager.wait(taskID, 0))
		cout<< "localRespond wurde abgeschlossen" << endl;
	else
		cout<< "localRespond timed out!" << endl;


	taskID = proxyManager.pCall<int>(string("decode"), INIT_WALK); //callVoid("runExecuter");
	cout<<"ID of Thread = " << taskID << endl;
	if (!proxyManager.wait(taskID, 0))
		cout<< "decode wurde abgeschlossen" << endl;
	else
		cout<< "decode timed out!" << endl;
		
				
	taskID = proxyManager.pCall(string("runExecuter")); //callVoid("runExecuter");
	cout<<"ID of Thread = " << taskID << endl;
	if (!proxyManager.wait(taskID, 0))
		cout<< "runExecuter wurde abgeschlossen" << endl;
	else
		cout<< "runExecuter timed out!" << endl;
		

	proxyManager.destroyConnection();
	
	/*AL::ALProxy proxyExecuter = AL::ALProxy(string("RMExecuter"), pip, 9559);
	proxyExecuter.callVoid<int>(string("setPosture"), 0);
	proxyExecuter.destroyConnection();*/
	
	const std::string msg = "Hello there, everything is initialized.";
	boost::shared_ptr<char*> buffer(new char*(buf));
	sserver = net->bindTcp(port);
	while(last != '-')
	{
		net->singleListen(sserver);
		sclient = net->acceptClient(sserver);
		strcpy(buf, "[Remote NAO] Willkommen!\r\n");
		cout<< "Sende Daten\r\n";
		//send(sclient, buf, strlen(buf), 0);
		
		bytesSent = 0;
		len = strlen(buf);
		do 
		{
			bytesSent += net->sendData(sclient, buffer, len, 0);
		}
		while (bytesSent < len);
		
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
		buf[0] = 0;
		
		while((last != '#') && (last != '-'))
		{
			//bytesRead = recv(sclient, buf, 1, 0);
			cout<< "receive...\r\n";
			bytesRead = 0;
			do
			{
				bytesRead += net->recvData(sclient, buffer, 1, bytesRead);
				cout<< "receive done: [" << bytesRead-1 << "] = " << buf[bytesRead-1] << endl;	
			}
			while (buf[bytesRead-1] != '+');
			last = buf[bytesRead-2];

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
	}
	
	net->unbind(sserver);
	broker->shutdown();
	//AL::ALBrokerManager::removeBroker(broker);
	
	exit(0);
	
	
	
	//delete net;

	/*while(1)
		qi::os::sleep(1);
	*/
	



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
