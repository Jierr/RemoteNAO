#ifndef __NETNAO
#define __NETNAO

#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

using namespace std;

class NetNao : public AL::ALModule
{
	public:
		//Load the Module in the broker with its correspending name
		//just extends ALModule
		NetNao(boost::shared_ptr<AL::ALBroker> broker, 
		       const string &name);
	
		virtual ~NetNao();
		virtual void init();
		//returns a socket 
		int bindTcp(const string& port);	
		void singleListen(const int& sockServer);
		int acceptClient(const int& sockServer);
		int sendData(const int& sockClient, const char* const &buf, const unsigned int& len);
		int recvData(const int& sockClient, boost::shared_ptr<char*> const& buf, const unsigned int& len);
		
		void disconnect(const int& sockClient);
		void unbind(const int& sockServer);
		
};

#endif
