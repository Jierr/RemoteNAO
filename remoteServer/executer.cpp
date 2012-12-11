#include <iostream>
#include <vector>
#include <alproxies/altexttospeechproxy.h>
#include <alproxies/alrobotposeproxy.h>
#include <alproxies/almotionproxy.h>
#include <alvalue/alvalue.h>
#include <qi/os.hpp>
#include <almath/tools/almath.h>


#include "executer.h"
#include "gen.h"

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
	AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
			tts.say(string("Executer"));
	
	
}

void Executer::initWalk()
{
	try
	{
		AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
		AL::ALMotionProxy motion(MB_IP, MB_PORT);
		tts.say("init standing!");
		//motion.wakeUp();
		motion.walkInit();
		cout << motion.getSummary() << endl;
		tts.say("Standing!");
		//boost::shared_ptr<AL::ALMotionProxy> motion = getParentBroker()->getMotionProxy(); 
		//motion->walkInit(); 
	}
	catch(const AL::ALError& e)
	{
		cout<< "Error initWalk: " << e.what() << endl; 
	}
}


void Executer::initSecure()
{	try
	{
		AL::ALValue roboConfig;
		AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
		AL::ALMotionProxy motion(MB_IP, MB_PORT);
		tts.say("init secure!");
		
		roboConfig = motion.getRobotConfig();
		tts.say(roboConfig[1][0]);
		
		//motion.rest();
		motion.walkInit();
		
		//interpolate to full stiffness in 0.5 seconds
        motion.stiffnessInterpolation("Body", 1.0, 0.5);
        AL::ALValue names = "Body";
        float ftargetAngles[] = {
        						 	 0.0, 0.0,
		    						 80.0, 20.0, -80.0, -60.0, 0.0, 0.0,
		    						 80.0, -20.0, +80.0, +60.0, 0.0, 0.0,
		    						 0.0, 0.0, -40.0/2 - 0.0, 40.0, -40.0/2, -0.0,
		    						 0.0, -0.0, -40.0/2 - 0.0, 40.0, -40.0/2, 0.0
        						};
       // cout<< "sizeof(ftargetAngles) = " << sizeof(ftargetAngles) << endl;
        for (int i = 0; i < sizeof(ftargetAngles)/sizeof(float); ++i)
        	ftargetAngles[i]*= RAD;

       	std::vector<float> vtargetAngles(ftargetAngles,ftargetAngles + sizeof(ftargetAngles)/ sizeof(float));
        AL::ALValue targetAngles(vtargetAngles);
        float maxSpeedFraction = 0.2;
        motion.angleInterpolationWithSpeed(names, targetAngles, maxSpeedFraction);
	    cout << motion.getSummary() << endl;
		
		
		tts.say("Save state!");
		//boost::shared_ptr<AL::ALMotionProxy> motion = getParentBroker()->getMotionProxy(); 
		//motion->walkInit(); 
	}
	catch(const AL::ALError& e)
	{
		cout<< "Error initWalk: " << e.what() << endl; 
	}
}



void Executer::setPosture(const int& pos)
{
	AL::ALRobotPoseProxy rr(MB_IP, MB_PORT);
	AL::ALValue posStr = "";
	cout<< "In setPosture" << endl;
	//qi::os::sleep(4);
	cout<< "Current Posture: " << rr.getActualPoseAndTime().toString() << endl;
	
	
	cout<< "======set Posture END =======" << endl 
	    << rr.getPoseNames().toString() << endl
	    << "======set Posture END =======" << endl;
	
	//AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
	//tts.say(rr.getPoseNames().toString());
	
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



