#include <iostream>
#include <alproxies/almemoryproxy.h>
#include <alvalue/alvalue.h>
#include <alproxies/altexttospeechproxy.h>
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
	
	mem = AL::ALMemoryProxy(broker);
	//mem.setDescription(string("lastOp"), string("Last Operation"));	
	lastOp = 42;	
	mem.insertData("lastOp", lastOp);
	lastOp = mem.getData("lastOp");
	cout<< "lastOp Constructor: " << (int&)lastOp << endl;
	
	cout<< "Constructor!" << endl;
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
	cout<< "Value of lastOp: ";
	//lastOp = mem.getData("lastOp");
	cout<< (int&)lastOp << endl;
	dec->decoderRespond();
}

void Manager::decode(const int& symbol)
{
	
}

void Manager::runExecuter()
{
	AL::ALValue posVal = mem.getData("robotPose");
	cout<< "ROBOT POSE: " << posVal.toString() << endl;
	//mem.insertData("robotPoseChanged", string("Sit"));
	mem.insertData("robotPose", 2);
	posVal = mem.getData("robotPose");
	cout<< "ROBOT POSE: " << posVal.toString() << endl;
	//mem.insertData("robotPose", 5.0f);
	//mem.insertData("robotPose", 3.0f);
	exec->executerRespond();
	AL::ALValue post;
	exec->setPosture((int&)post);
	//while(1)
	//{
		/*post = mem.getData("lastOp");
		exec->setPosture((int&)post);
		post = 0;
		mem.insertData("lastOp", post);*/
	//}
}
/*
boost::weak_ptr<Manager> Manager::getManager()
{
	static boost::shared_ptr<Manager> pManager(this, null_deleter());
	managerSingleton = pManager;
	return managerSingleton;
}
*/
	

