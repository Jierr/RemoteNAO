#ifndef __Executer__
#define __Executer__

#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
//#include <boost/enable_shared_from_this.hpp>
#include <string>

#include "eventlist.h"

using namespace std;

class Executer:public AL::ALModule
{
	private:
		boost::shared_ptr<AL::ALMutex> mutex;
		boost::shared_ptr<AL::ALMutex> sync;
		boost::shared_ptr<EventList> eventList;
		state_t state;
		string mpose;
		AL::ALMemoryProxy mem;
	public:
		//always get a shared_ptr from this via lock()	
		
		Executer(boost::shared_ptr<AL::ALBroker> broker, const string& name);
		virtual ~Executer();
		virtual void init();
		void initEventList(boost::shared_ptr<EventList> eL);
		void process();
		int processConflicts(const Event& event);
		void setState(state_t s);
		state_t getState(int type = 0xFFFF);
		void executerRespond();	
		void setPosture(const int& pos);	
		void initWalk();
		void initSecure();
		void standToSit();
		void sitToStand();
		void walk(const Event& event);
		void callback();
		void cbPoseChanged(const string& eventName, const string& postureName, const string& subscriberIdentifier);
		void speak(const string& msg);
		void moveHead(const Event& event);
		void moveArm(const Event& event);
		void joints();
		void sendBatteryStatus();
		void sendState();
};

#endif
