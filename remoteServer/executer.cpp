#include <iostream>
#include "executer.h"

using namespace std;
	
Executer::Executer(boost::shared_ptr<AL::ALBroker> broker, const string& name)
: AL::ALModule(broker, name)
{
	setModuleDescription("This Module manages the Data comming from the remoteServer module");
	
	functionName("executerRespond", getName(), "executer Respond");
	BIND_METHOD(Executer::executerRespond);
}

Executer::~Executer()
{
}

void Executer::init()
{

}

void Executer::executerRespond()
{
	cout<< "RMExecuter was pinged at!" << endl;
}



