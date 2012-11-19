#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "netNao.h"

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
		exit(2);	
	

	
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
}
