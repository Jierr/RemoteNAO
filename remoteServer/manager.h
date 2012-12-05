#ifndef __MANAGER__
#define __MANAGER__

#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
//#include <boost/enable_shared_from_this.hpp>
#include <string>

using namespace std;

class Manager:public AL::ALModule
{

	public:
		static boost::weak_ptr<Manager> managerSingleton;
		
		Manager(boost::shared_ptr<AL::ALBroker> broker, const string& name);
		virtual ~Manager();
		virtual void init();
		void localRespond();		
		static boost::shared_ptr<Manager> getManager();
};

#endif
