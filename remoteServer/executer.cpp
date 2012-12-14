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
		//AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
		//AL::ALMotionProxy motion(MB_IP, MB_PORT);
		//tts.say("init standing!");
		//motion.wakeUp();
		//motion.walkInit();
		float stiffnesses  = 1.0f;
		string snames = "Body";
		float x(1.0), y(0.0), t(0.0); 
		
		boost::shared_ptr<AL::ALMotionProxy> motion = getParentBroker()->getMotionProxy(); 
        motion->stiffnessInterpolation(snames, stiffnesses, 1.0);
        //motion->setStiffnesses(snames, stiffnesses);
		motion->walkInit(); 
		//motion->walkTo(x,y,t);
		stiffnesses  = 0.0f;
        //motion->setStiffnesses(snames, stiffnesses);
		//cout << motion.getSummary() << endl;
		//tts.say("Standing!");
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
		
		
		//motion.rest();
		//motion.walkInit();
		
		
		//boost::shared_ptr<AL::ALMotionProxy> motion = getParentBroker()->getMotionProxy();
		roboConfig = motion.getRobotConfig();
		tts.say(roboConfig[1][0]);
		float stiffnesses  = 1.0f; 
		string snames = "Body";
        motion.stiffnessInterpolation("Body", 1.0, 1.0);
		
		//interpolate to full stiffness in 0.5 seconds
       // motion.setStiffnesses(snames, stiffnesses);
		motion.walkInit(); 
		//qi::os::sleep(5);
        AL::ALValue names = "Body";
        float ftargetAngles[] = {
        						 	 0.0, 0.0,
		    						 80.0, 20.0, -80.0, -60.0, 0.0, 0.0,
		    						 0.0, 0.0, -40.0/2 - 0.0, 40.0, -40.0/2, -0.0,
		    						 0.0, -0.0, -40.0/2 - 0.0, 40.0, -40.0/2, 0.0,
		    						 80.0, -20.0, +80.0, +60.0, 0.0, 0.0
        						};
       // cout<< "sizeof(ftargetAngles) = " << sizeof(ftargetAngles) << endl;
        for (int i = 0; i < sizeof(ftargetAngles)/sizeof(float); ++i)
        	ftargetAngles[i]*= RAD;

       	std::vector<float> vtargetAngles(ftargetAngles,ftargetAngles + sizeof(ftargetAngles)/ sizeof(float));
        AL::ALValue targetAngles(vtargetAngles);
        float maxSpeedFraction = 0.2;
        qi::os::msleep(100);
        
		float x(1.0), y(0.0), t(0.0); 
		//motion.walkTo(x,y,0);
        motion.angleInterpolationWithSpeed(names, targetAngles, maxSpeedFraction);
		
		stiffnesses  = 0.0f; 
        motion.stiffnessInterpolation("Body", 0.2, 1.0);
       // motion->setStiffnesses(snames, stiffnesses);
	    //cout << motion.getSummary() << endl;
		
		
		//tts.say("Save state!");
		//boost::shared_ptr<AL::ALMotionProxy> motion = getParentBroker()->getMotionProxy(); 
		//motion->walkInit(); 
	}
	catch(const AL::ALError& e)
	{
		cout<< "Error initWalk: " << e.what() << endl; 
	}
}

void Executer::walk(const int& x)
{
	try
	{
		AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
		AL::ALMotionProxy motion(MB_IP, MB_PORT);
		
        motion.stiffnessInterpolation("Body", 1.0, 1.0);
		motion.walkInit();
		switch (x)
		{
			case MOV_FORWARD:
			{
				vector<string> legs (2, "");
				vector<float> footstep(3, 0.0);
				footstep[0] = 0.2;
				footstep[1] = 0.0;
				footstep[2] = 0.0;
				
				AL::ALValue footsteps = AL::ALValue::array(footstep, footstep);
				/*footsteps.arrayReserve(sizeof(vector<int>)*2);
				footsteps.array< vector<int>, vector<int> >(footstep, footstep);
				cout<< footsteps[0][0] << ", " << footsteps[0][1] << ", "<< footsteps[0][2] << ", " << endl
					<< footsteps[1][0] << ", " << footsteps[1][1] << ", "<< footsteps[1][2] << endl;*/
				vector<float> speed(2, 1.0); 				
				cout << "------> Move Forward <------" << endl;
				legs[0] = "LLeg";
				legs[1] = "RLeg";
				//last argument determines if existing footsteps planned should 
				//be cleared
				motion.setFootStepsWithSpeed(legs, footsteps, speed, false); 
				//motion.walkTo(0,0,0);
				
				break;
			}
			case MOV_BACKWARD:
			{				
				vector<string> legs (2, "");
				vector<float> footstep(3, 0.0);
				footstep[0] = -0.2;
				footstep[1] = 0.0;
				footstep[2] = 0.0;
				
				AL::ALValue footsteps = AL::ALValue::array(footstep, footstep);
				vector<float> speed(2, 1.0); 				
				cout << "------> Move Backward <------" << endl;
				legs[0] = "LLeg";
				legs[1] = "RLeg";
				//last argument determines if existing footsteps planned should 
				//be cleared
				motion.setFootStepsWithSpeed(legs, footsteps, speed, false); 
				
				break;
			}
			case MOV_LEFT:		
			{		
				vector<string> legs (2, "");
				vector<float> footstep(3, 0.0);
				footstep[0] = 0.2;
				footstep[1] = 0.0;
				footstep[2] = 0.3;
				
				AL::ALValue footsteps = AL::ALValue::array(footstep, footstep);
				vector<float> speed(2, 1.0); 				
				cout << "------> Move Left <------" << endl;
				legs[0] = "LLeg";
				legs[1] = "RLeg";
				//last argument determines if existing footsteps planned should 
				//be cleared
				motion.setFootStepsWithSpeed(legs, footsteps, speed, false); 
				
				break;
			}
			case MOV_RIGHT:
			{		
				vector<string> legs (2, "");
				vector<float> footstep(3, 0.0);
				footstep[0] = 0.2;
				footstep[1] = 0.0;
				footstep[2] = -0.3;
				
				AL::ALValue footsteps = AL::ALValue::array(footstep, footstep);
				vector<float> speed(2, 1.0); 				
				cout << "------> Move Right <------" << endl;
				legs[0] = "LLeg";
				legs[1] = "RLeg";
				//last argument determines if existing footsteps planned should 
				//be cleared
				motion.setFootStepsWithSpeed(legs, footsteps, speed, false); 
				
				break;
			}
			default:
				break;
		};
		
	}
	catch(const AL::ALError& e)
	{
		cout << "Error walk: " << e.what() << endl;
	}
}

void Executer::speak(const string& msg)
{
	cout << "In SPEAK:" << msg << endl;
	try
	{
		AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
		tts.say(msg);
	}
	catch(const AL::ALError& e)
	{
		cout << "Error speak: " << e.what() << endl;
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



