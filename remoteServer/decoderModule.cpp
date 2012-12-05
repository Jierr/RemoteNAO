#include <alcommon/albroker.h>
#include <alcommon/albrokermanager.h>
#include <alcommon/altoolsmain.h>
#include "decoder.h"


extern "C"
{
	int _createModule(boost::shared_ptr<AL::ALBroker> pBroker)
	{
		// init broker with the main broker instance
		// from the parent executable
		AL::ALBrokerManager::setInstance(pBroker->fBrokerManager.lock());
		AL::ALBrokerManager::getInstance()->addBroker(pBroker);
		// create module instances
		AL::ALModule::createModule<Decoder>(pBroker, "RMDecoder");
		return 0;
    }
    
    int _closeModule()
    {
    	return 0;
    }
}
