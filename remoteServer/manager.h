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


#include "decoder.h"
#include "executer.h"
#include "gen.h"
#include "eventlist.h"


using namespace std;




class AcessExec{
	
	public:	
		boost::shared_ptr<Executer> exec;
		AL::ALProxy pexec;
	
		AcessExec(boost::shared_ptr<AL::ALBroker> broker);
		~AcessExec();
};

class Manager:public AL::ALModule
{


	private:
		/*struct null_deleter{
			void operator()(void const*) const{}
		};*/
		AL::ALMemoryProxy mem;
		AL::ALValue lastOp;
		boost::shared_ptr<AL::ALMutex> mutex;
		AcessExec accessExec;
		boost::shared_ptr<EventList> eventList;
		
		bool fetch(const string& toParse, int& pos, event_params_t& ep);
		bool getParams(const string& toParse, int& pos, event_params_t& ep, int& paramCount);
		//void setEventParams(const );
		
	public:
		//always get a shared_ptr from this via lock()		
		boost::weak_ptr<Manager> managerSingleton;
		
		Manager(boost::shared_ptr<AL::ALBroker> broker, const string& name);
		virtual ~Manager();
		virtual void init();
		void localRespond();	
		int decode(const string& toParse);
		void runExecuter();	
		//boost::weak_ptr<Manager> getManager();
};

#endif