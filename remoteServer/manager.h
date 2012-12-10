#ifndef __MANAGER__
#define __MANAGER__

#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
#include <alvalue/alvalue.h>
#include <alproxies/almemoryproxy.h>
//#include <boost/enable_shared_from_this.hpp>
#include <string>
#include "decoder.h"
#include "executer.h"

using namespace std;

class Manager:public AL::ALModule
{


	private:
		/*struct null_deleter{
			void operator()(void const*) const{}
		};*/
		AL::ALMemoryProxy mem;
		AL::ALValue lastOp;
		boost::shared_ptr<Decoder> dec;
		boost::shared_ptr<Executer> exec;
		
		string fetch(const string& untouched);
		bool getParams(const string& code, const string& touched, void**& params);
		
	public:
		//always get a shared_ptr from this via lock()		
		boost::weak_ptr<Manager> managerSingleton;
		
		Manager(boost::shared_ptr<AL::ALBroker> broker, const string& name);
		virtual ~Manager();
		virtual void init();
		void localRespond();	
		void decode(const string& toParse);
		void runExecuter();	
		//boost::weak_ptr<Manager> getManager();
};

#endif
