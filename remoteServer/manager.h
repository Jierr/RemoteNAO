#ifndef __MANAGER__
#define __MANAGER__

#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
#include <alvalue/alvalue.h>
#include <althread/almutex.h>
#include <alproxies/almemoryproxy.h>
#include <alcommon/alproxy.h>
//#include <boost/enable_shared_from_this.hpp>
#include <string>


#include "executer.h"
#include "gen.h"
#include "eventlist.h"


using namespace std;



/**
\brief Manages 2 different ways to access the Executer module.

1. Access via proxy -- this is a functionality given by the framework --
2. Access via pointer to the module hence the instance of class Executer

\see Executer
*/
class AcessExec{
	
	public:	
		boost::shared_ptr<Executer> exec;
		AL::ALProxy pexec;
	
		AcessExec(boost::shared_ptr<AL::ALBroker> broker);
		~AcessExec();
};

/**
\brief Manages incomming comands and starts a thread to run the comand, if valid. 

The Manager module will be autoloaded during boot-time as part of the central naoqi-broker.
It receives the Input from the external [AppToNAO_BROKER] in main.cpp via the NetNao module.
It hands valid comands over to the Executer module.
*/
class Manager:public AL::ALModule
{


	private:
		AL::ALMemoryProxy mem; ///< Proxy to atomic managed memory of the framework
		AL::ALValue lastOp;
		boost::shared_ptr<AL::ALMutex> mutex; ///< reserved.
		AcessExec accessExec; ///< \see AcessExec
		boost::shared_ptr<EventList> eventList; ///< Pointer to instance of EventList, queueing valid comands.
		int pipeWrite;
		string ip4;
		
		/**
		\brief First step of parsing: Looks for valid comand specifier in given string.
		
		\param toParse String to be scanned.
		\param pos initial starting position and ending position after returning.
		\param ep is filled with the identified comand specifier TOKEN.\n
		       The TOKENs are defined in gen.h
		\return true, when params are expected\n
				false, otherwise
		*/
		bool fetch(const string& toParse, int& pos, event_params_t& ep);
		/**
		\brief Extracts the expected parameters to a given comand.
		
		\param toParse String to be scanned.
		\param pos Position -- Index -- to start scanning
		\param ep is filled with the corresponding param values, if valid.\n
			   ep will change the code specifier TOKEN to CODE_INVALID if the parameter is invalid.
		\param paramCount Current count of scanned parameters
		\return true, when more parameters or more input for the current parameter is expected.\n
				false, when no further input is expected.
		*/
		bool getParams(const string& toParse, int& pos, event_params_t& ep, int& paramCount);
		//void setEventParams(const );
		
	public:
		//always get a shared_ptr from this via lock()		
		boost::weak_ptr<Manager> managerSingleton;
		/**
		
		\brief Constructor
		
		Calls Constructor of AL::ALModule and registers all functions which should be 
		propagated to all local modules in the same broker and all external modules connected 
		to the broker of this module.
		Initializes #eventList for itself and Executer module
		
		\param broker specifies the broker this module belongs to
		\param name visible name of this module, via this name a proxy to this module can be opened 
		*/
		Manager(boost::shared_ptr<AL::ALBroker> broker, const string& name);
		
		///Destructor
		virtual ~Manager();
		
		/**
		\brief Will be called right after the constructor.
		*/
		virtual void init();
		/// Kind of a ping to this module, to test if still living.
		void localRespond();
			
		/**
		\brief decodes a string, it only processes one char at a time before returning
		
		The current decoding stage is managed via states, the stages are defined in gen.h
		STG_FETCH -> STG_PARAM -> STG_VALID
		It is called by the [AppToNAO_BROKER] via a proxy to the Manager module belonging to naoqi broker.
		
		\param toParse String to be scanned and translated.
		\return Returns current scanning position or -1, when the Connection is to be closed.
		*/
		int decode(const string& toParse);
		
		void addCom(const int& type, const int& ip1, const int& ip2, const string& sp, const int& prio);
		
		void initPipe(const int& writer);
		void initIp4(const string& ip);
		
		/**
		\brief Initialises the starting position of the robot and schedules valid events afterwards.
		
		It is called by the [AppToNAO_BROKER] via a proxy to the Manager module as a thread.
		This thread lives until the Manager module is killed.
		It runs parallel to the decoding and executes events -- valid comands -- as new threads.
		*/
		void runExecuter();	
		//boost::weak_ptr<Manager> getManager();
};

#endif
