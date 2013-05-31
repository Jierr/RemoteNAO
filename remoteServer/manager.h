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
\brief Manages 2 different ways to access the Executer module. \n\n

1. Access via proxy -- this is a functionality given by the framework -- \n
2. Access via pointer to the module hence the instance of class Executer \n

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
		boost::shared_ptr<Behavelist> blist; ///< List holding all beheaviours
		int pipeWrite; ///< file descriptor piping to the cam module 
		int threadcount; ///< currently running threads
		string ip4; ///< ip of nao
		string cbip; ///< ip of the control module (localhost default)
		unsigned short cbport; ///< port of the control module (used for interprocess communication of the naoqi framework)
		int stateAbs; ///< absolute logical state of the nao
		int inTransition; ///< identifies if nao is in Transition
		int blockGen; ///< deprecated
		bool block[NUM_CODES]; ///< array of blocked absolute (atomic) Events of class Manager
		bool parblock[NUM_CODES]; ///< array of blocked parallel Events of class Manager
		event_params_t absTransition[NUM_ABS_STATES][NUM_ABS_STATES]; ///< holds state transitions, for identifieing valid transitions and resolving invalid ones
		int genAllowed[NUM_GEN_STATES][NUM_ABS_STATES]; ///< holds allowed parallel Events to a running transition
	
//		/**
//		\brief First step of parsing: Looks for valid comand specifier in given string.
//		
//		\param toParse String to be scanned.
//		\param pos initial starting position and ending position after returning.
//		\param ep is filled with the identified comand specifier TOKEN.\n
//		       The TOKENs are defined in gen.h
//		\return true, when params are expected\n
//				false, otherwise
//		*/
//		bool fetch(const string& toParse, int& pos, event_params_t& ep);
//		/**
//		\brief Extracts the expected parameters to a given comand.
//		
//		\param toParse String to be scanned.
//		\param pos Position -- Index -- to start scanning
//		\param ep is filled with the corresponding param values, if valid.\n
//			   ep will change the code specifier TOKEN to CODE_INVALID if the parameter is invalid.
//		\param paramCount Current count of scanned parameters
//		\return true, when more parameters or more input for the current parameter is expected.\n
//				false, when no further input is expected.
//		*/
//		bool getParams(const string& toParse, int& pos, event_params_t& ep, int& paramCount);
//		//void setEventParams(const );

		
		/**
		\brief Initializes #absTransition
		*/
		void initAbsTransition();
		/**
		\brief Initializes #genAllowed
		*/
		void initGenAllowed();
		
		/**
		\brief Initializes blist with all current none standart behaviors found on the robotPose \n
		       also analyses them for additional transition information 
		*/
		void initblist();
		
		/**
		\brief fills ep for the specified transition from -> to
		
		\param from state, defined in enum state_abs in gen.h
		\param to state, defined in enum state_abs in gen.h
		
		\return 0, transition is valid, ep holds a fully specified transition \n
				1, transition is valid, ep does not hold an Event (Events needing additional parameters)
		        -1, from or to is out of bound (doesn't actually exist) \n
		        -2, transition is invalid
		        
		*/
		int retrieveTrans(const int& from, const int& to, event_params_t* ep);
		
		/**
		\brief returns return #genAllowed[gen][abs]
		
		\param gen one of the specified parallel Events specified in enum state_gen in gen.h
		\param abs current state, specified in enum state_abs in gen.h
		
		\return -1, from or to is out of bound \n
				0, is not allowed during that state \n
				1, is allowd during that state 
		*/
		int isGenAllowed(const int& gen, const int& abs);
		
		/**
		\brief Analyses scheduled Event for execution at the current state (#stateAbs)
		
		\return 0, absolute event has to be resolved to be executed
				1, no conflict \n
				-1, absolute event currently blocked \n
				-2, parallel event currently blocked \n
				-3, parallel event is not allowed in current state
		*/
		int processConflicts(Event* event);
		
		/**
		\brief Resolves a non valid transition from -> to and creates a sequence of new transitions, 
			   which finaly result in \a to these are added at the beginning of the EventList 
			   and will be executed immediatly.
		
		\return -1, no resolved transitons sequence could be found \n
		         0, resolved transition sequence has been added to the EventList
		*/
		int resolveConflict(Event* event, const int& from, const int& to);
		
		/**
		\brief Uses the return value of processConflicts()
		
		Uses the return value of processConflicts()
		
		0, use resolveConflict()
		1, do nothing
		-1, do nothing
		-2, do nothing
		-3, discard parallel event
		*/		
		int adaptEventList(const int& confresult, Event* event);
		
		/**
		\brief blocks parallel and absolute Events
		
		\param code code of the Event to be started \n
		            must be one of the enum codes \see gen.h
		            
		\return deprecated, useless
		*/
		int blockfor(const int& code);
		
		/**
		\brief check if the event is currently blocked
		
		\param code must be one of the enum codes \see gen.h

		*/
		bool isblocked(const int& code);
		
		/**
		\brief Translation of #isblocked(const int& code) for Comand Specfier TOKENs in gen.h
		*/
		bool isblocked(Event* event);
		
		/// Thread creation and allocating management.
		int new_thread(struct exec_arg* targ[MAX_THREADS], Event* event, boost::shared_ptr<EventList> eL);
		/// Thread creation and allocating management.
		int delete_thread(struct exec_arg* targ[MAX_THREADS], int t);
		/// Thread creation and allocating management.
		int free_done_threads(struct exec_arg* targ[MAX_THREADS]);
		/// Thread creation and allocating management.
		int catch_dangling_threads(struct exec_arg* targ[MAX_THREADS]);
		
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
		
		/// sets #cbip and #cbport 
		void setCB(const string& bip, const int& bport);
			
		/**
		\brief decodes a string, it only processes one char at a time before returning
		
		The current decoding stage is managed via states, the stages are defined in gen.h
		STG_FETCH -> STG_PARAM -> STG_VALID
		It is called by the [AppToNAO_BROKER] via a proxy to the Manager module belonging to naoqi broker.
		
		\param toParse String to be scanned and translated.
		\return Returns current scanning position or -1, when the Connection is to be closed.
		*/
//		int decode(const string& toParse);
		
		
		/**
		\brief adds an Event to Eventlist
		
		prio = 0 -> add to the beginning \n
		prio = 1 -> add to the ending \n\n
		
		parameters correspond to the members of event_params_t
		*/
		void addCom(const int& type, const int& ip1, const int& ip2, const float& fp, const string& sp, const int& prio);
		
		void initPipe(const int& writer);
		void initIp4(const string& ip);
		
		/**
		\brief Initialises the starting position of the robot and schedules valid events afterwards.
		
		It is called by the [NET_BROKER] via a proxy to the Manager module as a thread.
		This thread lives until the Manager module is killed.
		It runs parallel to the decoding and executes events -- valid comands -- as new threads.
		*/
		void runExecuter();	
		//boost::weak_ptr<Manager> getManager();
};

#endif
