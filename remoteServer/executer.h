#ifndef __Executer__
#define __Executer__

#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
//#include <boost/enable_shared_from_this.hpp>
#include <string>
#include "behavelist.h"
#include "eventlist.h"

using namespace std;

/**
\brief Structure, which holds arguments for one execution thread created by Manager::runExecuter(). \n
	   The created thread exectues the given Event
*/
struct exec_arg
{
	pthread_t id; ///< ID of the thread.
	int tnum; ///< thread number, identifies allocated space in an array for this structure.
	Event* event; ///< Event to be executed.
	boost::shared_ptr<EventList> eventList; ///< Access to the Eventlist, functions are atomic.
	boost::shared_ptr<AL::ALMutex> mutex; ///< mutex to create mutual executions between more than one of those threads.
};

/**
\brief Resolves Conflicts between Events and executes them

The Executer module will be autoloaded during boot-time as part of the central naoqi-broker.
It contains functions used by the Manager module to execute scheduled Events.
It provides parallelism of scheduled Events.

*/
class Executer:public AL::ALModule
{
	private:
		boost::shared_ptr<AL::ALMutex> mutex; ///< mutex to grant consistent data --> espec. used in Executer::process()
		boost::shared_ptr<AL::ALMutex> sync; ///< deprecated
		boost::shared_ptr<EventList> eventList; ///< contains events to be executed
		boost::shared_ptr<Behavelist> blist; ///< List holding all beheaviours 
		string mpose;	///< physical state of the robot as string, also retrieved through the callback --> cbPoseChanged()
		AL::ALMemoryProxy mem; ///< Proxy to the atomic memory management of the framework.
		
		int* stateAbs; ///< Pointer to logical state of the robot of class Manager
		int* inTransition; ///< Pointer to transition identifier of class Manager
		int* blockGen; ///< deprecated
		bool* block; ///< pointer to array of blocked absolute (atomic) Events of class Manager
		bool* parblock; ///< pointer to array of blocked parallel Events of class Manager
		
		
		/**
		\brief unblocks parallel and absolute Events
		
		\param code code of the currently finished Event \n
		            must be one of the enum codes \see gen.h
		            
		\return deprecated, useless
		*/
		int unblockfor(const int& code);
	public:
		string cbip; ///< ip of the Control Broker
		unsigned short cbport; ///< port of the Control Broker
		
		bool cbcall; ///< identifies the execution of a Executer::cbPoseChanged() callback
		bool cbinc; ///< identifies the first detection of STATE_UNKNOWN, and therefor an unsuspected behavior
		state_t cbstate;	///< physical state of the robot, set through sensors, hence a callback --> cbPoseChanged()
		static Executer* self; ///< needed in static method Executer::execute(), which is created as Thread
		
		/**
		\brief set up pointers to #stateAbs #inTransition #blockGen #block #parblock 
		
		This is done in Manager::runExecuter()
		*/
		void setStateMan(int* abs, int* itrans, int* gblock, bool* b, bool* pb);
		
		/**
		\brief Constructor
		
		Calls Constructor of AL::ALModule and registers all functions which should be 
		propagated to all local modules in the same broker and all external modules connected 
		to the broker of thiss module.
		Calls the constructor of the mutex's
		
		\param broker specifies the broker this module belongs to
		\param name visible name of this module, via this name a proxy to this module can be opened 
		*/
		Executer(boost::shared_ptr<AL::ALBroker> broker, const string& name);
		virtual ~Executer();
		
		/**
		\brief Will be called right after the constructor.
		*/
		virtual void init();
		
		/**
		\brief intializes #eventList with eL
		\param eL
		*/
		void initEventList(boost::shared_ptr<EventList> eL);
		
		/**
		\brief intializes #blist with bL
		\param bL
		*/
		void initBehavelist(boost::shared_ptr<Behavelist> bL);
		
		/**
		\brief Resolves event - conflicts and executes one fitting event. \n
			   Sets the #state of the roboter after the event has been executed.
		
		It is called as thread per event by Manager::runExecuter()
		*/
		//void process();
		
		/**
		\brief Finds and manages conflicting events and/or states in the #eventList and adapts/resolves the current event.
		
		This function is called by process()
		
		\param event Current event to be executed.
		*/
		int processConflicts(const Event& event);

		void executerRespond();	
		
		void initWalk(); ///< inits position to be "ready to walk".
		void initSecure(); ///< will set the roboter to a resting position with motors turned of.
		void killBehaviors(); ///< kills all running behaviours
		void killRemainingTasks(); ///< kill all remaining tasks
		void behave_stand(); ///< move roboter to standing position
		void behave_sit(); ///< move roboter to sitting position
		void behave_hello(); ///< lets the roboter wave and say hello
		void behave_dance(); ///< lets the roboter dance
		void behave_wipe(); ///< roboter wipes out the world, after that he wipes his forehead
		void behave_gen(const string& com); ///< executes a generic behavior with simplified name com
		
		/**
		\brief The roboter will walk.
		
		\param event holds the walking mode
		*/
		void walk(const Event& event);
		
		/**
		\brief Callback to retrieve the physical state of the robot.
		*/
		void cbPoseChanged(const string& eventName, const string& postureName, const string& subscriberIdentifier);
		
		/**
		\brief roboter speaks string given in msg
		
		\param msg
		*/
		void speak(const string& msg);
		/**
		\brief robot will move his head
		
		\param event holds the moving mode.
		*/
		void moveHead(const Event& event);
		/**
		\brief robot will move his arm
		
		\param event holds the moving mode.
		*/
		void moveArm(const Event& event);
		
		/**
		\brief sets exec_arg::event = 0, therefor marking the thread as done and ready for freeing allocated space
		
		This function is used in execute()
		
		\param aargs equal to the args parameter of method execute 
		*/
		static void mark_thread_done(struct exec_arg* aargs);
		
		/**
		\brief This function is used for creating an Execution Thread in Manager::runExecuter 
		
		\param args argument as pointer of struct exec_arg
		*/
		static void* execute(void* args);
		
		/**
		\brief send remaining battery power to Android App
		
		This function uses a proxy to NetNao in broker AppToNAO_BROKER
		*/
		void sendBatteryStatus();
		
		/**
		\brief send roboter state to Android App
		
		This function uses a proxy to NetNao in broker AppToNAO_BROKER
		*/
		void sendState();
};

#endif
