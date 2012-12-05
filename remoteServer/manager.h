#ifndef __MANAGER__
#define __MANAGER__

#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
#include <string>

using namespace std;

class Manager: public AL::ALModule
{
	private:
	public:
		Manager(boost::shared_ptr<AL::ALBroker> broker, const string& name);
		virtual ~Manager();
		virtual void init();
		void localRespond();
};

#endif
