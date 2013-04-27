#ifndef __NETNAO
#define __NETNAO

#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
#include <alvalue/alvalue.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

using namespace std;

/**
\brief Externel module -- with own broker -- managing the android and roboter connectivities

The module is created and used during runtime of its main broker [AppToNAO_BROKER] in main.cpp 
*/
class NetNao : public AL::ALModule
{
	private:
		int sclient_tcp;	///< Socket of connected client
		int sserver_tcp;	///< Socket of the server
		int mode;
	public:
		string ip4;
		//Load the Module in the broker with its correspending name
		//just extends ALModule
		/**
		\brief Constructor
		
		Calls Constructor of AL::ALModule and registers all functions which should be 
		propagated to all local modules in the same broker and all external modules connected 
		to the broker of thiss module.
		
		\param broker specifies the broker this module belongs to
		\param name visible name of this module, via this name a proxy to this module can be opened 
		*/
		NetNao(boost::shared_ptr<AL::ALBroker> broker, 
		       const string &name);
	
		///Destructor
		virtual ~NetNao();
		/**
		\brief Will be called right after the constructor.
		*/
		virtual void init();
		//returns a socket 
		
		void writePipe(const int& writer, const AL::ALValue& buf, const int& len);
		
		/**
		\brief bind the server to specified port
		
		\param port to be bound
		\return returns the server socket hence #sserver_tcp
		*/
		int bindTcp(const string& port);	
		/**
		\brief Listen for incomming connections.
		
		Only one connection is allowed on incomming queue.
		This Call is blocking.
		
		\param sockServer returned server socket by bindTcp()
		*/
		void singleListen(const int& sockServer);
		/**
		\brief Supposed to be called after singleListen(), accepts the client.
		Connection is now established.
		
		\param sockServer returned server socket by bindTcp()
		\reuturn returns the client socket hence #sclient_tcp
		*/
		int acceptClient(const int& sockServer);
		/**
		\brief sends a string with given length to the connected client
		
		This call is blocking.
		
		\param sockClient client socket returned by acceptClient()
		\param buf buffer holding the string
		\param len length of the string
		\param indexStart specifies first charackter to be sent
		\return sucess: number charackters sent
		\return fail: -1
		*/
		int sendString(const int& sockClient, const AL::ALValue& buf, 
					 const int& len, const int& indexStart);
		
		/**
		\brief receives data with given max length from the connected client
		
		This call is blocking.
		
		\param sockClient client socket returned by acceptClient()
		\param buf buffer 
		\param len max length of the buffer
		\param indexStart specifies the index of the buffer to be written first
		\return sucess: number charackters received
		\return fail: -1 on invalid socket, SOCK_CLOSED or SOCK_LOST on resetted conection 
		*/
		int recvData(const int& sockClient, const boost::shared_ptr<char*>& buf, 
					 const unsigned int& len, const unsigned int& indexStart);

		
		/**
		\brief Disconnects the Client.
		
		\param sockClient client socket returned by acceptClient()
		*/
		void disconnect(const int& sockClient);
		/**
		\brief Closes server socket.
		
		\param sockClient client socket returned by acceptClient()
		*/
		void unbind(const int& sockServer);
		
		/**
		\return Currently connected client socket.
		*/
		int getClient_tcp();
		
		/**
		\return Current server socket.
		*/
		int getServer_tcp();
		
};


#endif
