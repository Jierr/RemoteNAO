#include <iostream>
#include "manager.h"
#include "decoder.h"
#include "executer.h"


using namespace std;
	
Manager::Manager(boost::shared_ptr<AL::ALBroker> broker, const string& name)
: AL::ALModule(broker, name)
{
	setModuleDescription("This Module manages the Data comming from the remoteServer module");
	
	functionName("localRespond", getName(), "local Respond");
	BIND_METHOD(Manager::localRespond);
	
	functionName("runExecuter", getName(), "run Executer");
	BIND_METHOD(Manager::runExecuter);
	
	dec = AL::ALModule::createModule<Decoder>(broker, "RMDecoder");
	exec = AL::ALModule::createModule<Executer>(broker, "RMExecuter");
}

Manager::~Manager()
{
}

void Manager::init()
{

}

void Manager::localRespond()
{
	cout<< "RMManager was pinged at!" << endl;
	dec->decoderRespond();
	while(1);
}

void Manager::decode(const int& symbol)
{
	
}

void Manager::runExecuter()
{
	exec->executerRespond();
}
/*
boost::weak_ptr<Manager> Manager::getManager()
{
	static boost::shared_ptr<Manager> pManager(this, null_deleter());
	managerSingleton = pManager;
	return managerSingleton;
}
*/
	

