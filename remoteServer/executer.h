#ifndef __Executer__
#define __Executer__

#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
//#include <boost/enable_shared_from_this.hpp>
#include <string>

#include "eventlist.h"

using namespace std;


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
		boost::shared_ptr<AL::ALMutex> sync; 
		boost::shared_ptr<EventList> eventList; ///< contains events to be executed
		state_t state;	///< logical state of the robot, set through executed functions in process()
		state_t cbstate;	///< physical state of the robot, set through sensors, hence a callback --> cbPoseChanged()
		string mpose;	///< physical state of the robot as string, also retrieved through the callback --> cbPoseChanged()
		AL::ALMemoryProxy mem; ///< Proxy to the atomic memory management of the framework.
	public:
		//always get a shared_ptr from this via lock()	
		
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
		\brief Resolves event - conflicts and executes one fitting event. \n
			   Sets the #state of the roboter after the event has been executed.
		
		It is called as thread per event by Manager::runExecuter()
		*/
		void process();
		
		/**
		\brief Finds and manages conflicting events and/or states in the #eventList and adapts/resolves the current event.
		
		This function is called by process()
		
		\param event Current event to be executed.
		*/
		int processConflicts(const Event& event);
		/**
		\brief updates logical #state with \a s, also considers the physical state of the robo.
		
		\param s New state, defined as #naoStates 
		*/
		void setState(state_t s);
		
		/**
		\brief Retrieves the current logical or physical \a state.
		
		\param type Can be STATE_ABSOLUT, STATE_ALL, STATE_PARALLEL
		\return Returns the current \a state.
		
		\see naoStates
		*/
		state_t getState(int type = 0xFFFF);
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
