#include <alcommon/albroker.h>
#include <alcommon/albrokermanager.h>
#include <alcommon/altoolsmain.h>
#include "manager.h"


extern "C"
{
	int _createModule(boost::shared_ptr<AL::ALBroker> pBroker)
	{
		// init broker with the main broker instance
		// from the parent executable
		AL::ALBrokerManager::setInstance(pBroker->fBrokerManager.lock());
		AL::ALBrokerManager::getInstance()->addBroker(pBroker);
		// create module instances
		AL::ALModule::createModule<Manager>(pBroker, "RMManager");
		return 0;
    }
    
    int _closeModule()
    {
    	return 0;
    }
}
