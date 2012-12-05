#include <iostream>
#include "manager.h"

using namespace std;
	
Manager::Manager(boost::shared_ptr<AL::ALBroker> broker, const string& name)
: AL::ALModule(broker, name)
{
	setModuleDescription("This Module manages the Data comming from the remoteServer module");
	
	functionName("localRespond", getName(), "local Respond");
	BIND_METHOD(Manager::localRespond);
}

Manager::~Manager()
{
}

void Manager::init()
{
	//managerSingleton = boost::shared_ptr;
}

void Manager::localRespond()
{
	cout<< "RMManager was pinged at!" << endl;
}

boost::shared_ptr<Manager> Manager::getManager()
{
	return managerSingleton.lock();
}

