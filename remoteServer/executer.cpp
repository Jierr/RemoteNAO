#include <iostream>
#include "executer.h"
#include <alproxies/altexttospeechproxy.h>
#include <alproxies/alrobotposeproxy.h>
#include <alvalue/alvalue.h>
#include <qi/os.hpp>

using namespace std;
	
Executer::Executer(boost::shared_ptr<AL::ALBroker> broker, const string& name)
: AL::ALModule(broker, name)
{
	setModuleDescription("This Module manages the Data comming from the remoteServer module");
	
	functionName("setPosture", getName(), "set Posture of Robot");
	addParam("pos", "posture as int");
	BIND_METHOD(Executer::setPosture);
}

Executer::~Executer()
{
}

void Executer::init()
{

}

void Executer::executerRespond()
{
	AL::ALTextToSpeechProxy tts("127.0.0.1", 9559);
			tts.say(string("Executer"));
	
	
}


void Executer::setPosture(const int& pos)
{
	AL::ALRobotPoseProxy rr("127.0.0.1", 9559);
	AL::ALValue posStr = "";
	cout<< "In setPosture" << endl;
	//qi::os::sleep(4);
	cout<< "Current Posture: " << rr.getActualPoseAndTime().toString() << endl;
	
	AL::ALTextToSpeechProxy tts("127.0.0.1", 9559);
	//tts.say(rr.getPoseNames().toString());
	
	rr.exit();
	//posStr = rr.getPosNames();
	
/*	if (pos == 0)
		return;
	try
	{
		AL::ALProxy postureProxy = AL::ALProxy("ALRobotPosture", "nao.local", 9559);	
		switch(pos)
		{
			case 1: 
				postureProxy.gotoPosture("StandZero", 1.0);
				break;
			case 2:
				postureProxy.gotoPosture("Sit", 1.0);
				break;
			case default:
				break;			
		};	
	}
	catch(const AL::ALError& e)
	{
		cout<< "Error Posture": << e.what() << endl;
	}

*/

}



