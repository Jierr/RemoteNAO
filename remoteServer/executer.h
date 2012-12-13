#ifndef __Executer__
#define __Executer__

#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
//#include <boost/enable_shared_from_this.hpp>
#include <string>

using namespace std;

class Executer:public AL::ALModule
{
	private:
	public:
		//always get a shared_ptr from this via lock()	
		
		Executer(boost::shared_ptr<AL::ALBroker> broker, const string& name);
		virtual ~Executer();
		virtual void init();
		void executerRespond();	
		void setPosture(const int& pos);	
		void initWalk();
		void initSecure();
		void walk(const int& x);
		void speak(const string& msg);
};

#endif
