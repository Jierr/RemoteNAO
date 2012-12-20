#include <iostream>
#include <vector>
#include <alproxies/altexttospeechproxy.h>
#include <alproxies/alrobotposeproxy.h>
#include <alproxies/almotionproxy.h>
#include <alproxies/almemoryproxy.h>
#include <alcommon/alproxy.h>
#include <alvalue/alvalue.h>
#include <qi/os.hpp>
#include <almath/tools/almath.h>


#include "executer.h"
#include "gen.h"

using namespace std;
	
Executer::Executer(boost::shared_ptr<AL::ALBroker> broker, const string& name)
: 	AL::ALModule(broker, name),
	mutex(AL::ALMutex::createALMutex()),
	sync(AL::ALMutex::createALMutex()),
	bat(0)
{
	setModuleDescription("This Module manages the Data comming from the remoteServer module");
	
	functionName("setPosture", getName(), "set Posture of Robot");
	addParam("pos", "posture as int");
	BIND_METHOD(Executer::setPosture);	
	
	functionName("executerRespond", getName(), "Ping the module");
	BIND_METHOD(Executer::executerRespond);	
	
	functionName("initWalk", getName(), "Initializes the posture for walking.");
	BIND_METHOD(Executer::initWalk);
	
	functionName("initSecure", getName(), "Sets NAO in resting mode.");
	BIND_METHOD(Executer::initSecure);
	
	functionName("walk", getName(), "NAO will walk.");
	addParam("x", "Walking mode");
	BIND_METHOD(Executer::walk);
	
	functionName("speak", getName(), "Speak text given.");
	addParam("msg", "Textmessage");
	BIND_METHOD(Executer::speak);
	
	functionName("sendBatteryStatus", getName(), "Will send the Battery status, to the remote host(App),\n\
												  which the Network module RMNetNao is connected to.");
	BIND_METHOD(Executer::sendBatteryStatus);
	
	functionName("process", getName(), "Execute a given event");
	addParam("event", "The event to be processed");
	BIND_METHOD(Executer::process);
	
	functionName("setBat", getName(), "Callback");
	BIND_METHOD(Executer::setBat);
}

Executer::~Executer()
{
}

void Executer::init()
{
	mem = AL::ALMemoryProxy(getParentBroker());
	mem.subscribeToEvent("BatteryLevelChanged", "RMExecuter",
                                  "setBat");
}

void Executer::initEventList(boost::shared_ptr<EventList> eL)
{
	eventList = eL;
}


void Executer::setState(state_t s)
{
	mutex->lock();
	state = s;
	mutex->unlock();
}


state_t Executer::getState()
{
	return state;
}


int Executer::processConflicts(const Event& event)
{
	mutex->lock();
	event_params_t ep;
	int classf = EVT_BUSY;
	
	state_t stateAbs = state & STATE_ABSOLUT;
	state_t statePar = state & STATE_PARALLEL;
	switch (event.ep.type)
	{
		case INIT_WALK:	
			if (stateAbs != STATE_CROUCHING)
			{
				classf = EVT_DONE;
			}		
			break;
		case INIT_REST:
			if (stateAbs != STATE_STANDING)
			{
				classf = EVT_DONE;
			}		
			break;
		case CODE_SPK:
			break;
		case CODE_MOV:
			if (stateAbs == STATE_CROUCHING)
			{
				ep.type = INIT_WALK;
				eventList->addFirst(ep);
				classf = EVT_PENDING;
			}
			else if (stateAbs == STATE_WALKING)
			{
				ep.type = CODE_STOP;
				ep.iparams[0] = MOV_STOP;
				eventList->addFirst(ep);
				classf = EVT_PENDING;
			}
			else if ((stateAbs == STATE_MOVING) 	||
					 (stateAbs == STATE_STOPPING)		)
			{
				classf = EVT_PENDING;
			}
			else if (stateAbs != STATE_STANDING)
			{
				classf = EVT_DONE;
			}
			break;
		case CODE_HEAD:
			if (statePar & STATE_HEADMOVE)
				classf = EVT_PENDING;
			if (stateAbs == STATE_UNKNOWN)
				classf = EVT_DONE;
			break;
		case CODE_STOP:
			if (stateAbs != STATE_WALKING)
				classf = EVT_DONE;
			break;
		case CODE_BAT:
			break;
		case RESET_CONNECTION:
			if (stateAbs == STATE_WALKING)
			{
				cout<< "DIS: first stop Movement" << endl;
				ep.type = CODE_STOP;
				ep.iparams[0] = MOV_STOP;
				eventList->addFirst(ep);
				classf = EVT_PENDING;
			}			
			else if (stateAbs == STATE_STANDING)
			{
				cout<< "DIS: Now Rest" << endl;
				ep.type = INIT_REST;
				eventList->addFirst(ep);
				classf = EVT_PENDING;
			}
			else if ((stateAbs == STATE_STOPPING) || (stateAbs == STATE_MOVING))
				classf = EVT_PENDING;
			break;
		case CODE_INVALID:
		default:
				classf = EVT_DONE;
			//cout<< "Nothing to do" << endl;
			break;		
	};
	mutex->unlock();
	return classf;
}

void Executer::process(const Event& event)
{
	//cout<<"----->Process Event" << endl;
	int classf;	
	
	classf = processConflicts(event);
	eventList->setClassf(event.id, classf);
	if (classf == EVT_BUSY)
	{
		switch (event.ep.type)
		{
			case INIT_WALK:
				setState(STATE_MOVING);
				initWalk();
				setState(STATE_STANDING);
				//cout << "Done initWalk()" << endl;
				break;
			case INIT_REST:
				setState(STATE_MOVING);
				initSecure();
				setState(STATE_CROUCHING);
				break;
			case CODE_SPK:
				speak(event.ep.sparam);
				break;
			case CODE_MOV:
				setState(STATE_WALKING);
				walk(event);
				sync->lock();
				setState(STATE_STANDING);
				sync->unlock();
				break;
			case CODE_HEAD:
				setState(getState() | STATE_HEADMOVE);
				moveHead(event);
				setState(getState() & ~STATE_HEADMOVE);
				break;
			case CODE_STOP:
				//event.ep.iparams[0] = MOV_STOP;
				sync->lock();
				setState(STATE_STOPPING);
				walk(event);
				setState(STATE_STANDING);
				sync->unlock();
				cout<< "STP: NOW STANDING" << endl;
				break;
			case CODE_BAT:
				sendBatteryStatus();
				break;
			case RESET_CONNECTION:
				setState(STATE_CROUCHING);
				break;
			case CODE_INVALID:
				//cout<< "Nothing to do" << endl;
				break;
			default:
			{
				try
				{
					AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
					tts.say("Unknown command!");
				}	
				catch(const AL::ALError& e)
				{
					cout<< "Error [Executer]<process(event)>: " << endl << e.what() << endl;
				}
				break;
			}
		};

		eventList->setClassf(event.id, EVT_DONE);	
	}
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
{	
	try
	{
		AL::ALValue roboConfig;
		AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
		AL::ALMotionProxy motion(MB_IP, MB_PORT);
		//tts.say("init secure!");
		
		
		//motion.rest();
		//motion.walkInit();
		
		
		//boost::shared_ptr<AL::ALMotionProxy> motion = getParentBroker()->getMotionProxy();
		//roboConfig = motion.getRobotConfig();
		//tts.say(roboConfig[1][0]);
		//cout << roboConfig.toString() << endl;
		float stiffnesses  = 1.0f; 
		string snames = "Body";
        motion.stiffnessInterpolation("Body", 1.0, 1.0);
		
		//interpolate to full stiffness in 0.5 seconds
        //motion.setStiffnesses(snames, stiffnesses);
		//motion.walkInit(); 
		//qi::os::sleep(5);
        AL::ALValue names = "Body";
        float ftargetAngles[] = {
        						 	 0.0, 0.0,
		    						 80.0, 20.0, -80.0, -60.0, 0.0, 0.0,
		    						 0.0, 0.0, -150.0/2 + 25.0, 150.0, -150.0/2, -0.0,
		    						 0.0, -0.0, -150.0/2 + 25.0, 150.0, -150.0/2, 0.0,
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
        motion.stiffnessInterpolation("Body", 0.0, 1.2);
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

void Executer::walk(const Event& event)
{
	try
	{
		AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
		AL::ALMotionProxy motion(MB_IP, MB_PORT);
		int mode = event.ep.iparams[0];
		
        motion.stiffnessInterpolation("Body", 1.0, 1.0);
		//motion.walkInit();
		
		switch (mode)
		{
			case MOV_FORWARD:
			{
				vector<string> legs (50, "");
				vector<float> footstep(3, 0.0);
				footstep[0] = 0.2;
				footstep[1] = 0.0;
				footstep[2] = 0.0;
				
				AL::ALValue footsteps;// = AL::ALValue::array(footstep, footstep);
				//footsteps.arraySetSize(50);
				for (int i=0; i<50; ++i)
					footsteps.arrayPush(footstep);
				/*footsteps.arrayReserve(sizeof(vector<int>)*2);
				footsteps.array< vector<int>, vector<int> >(footstep, footstep);
				cout<< footsteps[0][0] << ", " << footsteps[0][1] << ", "<< footsteps[0][2] << ", " << endl
					<< footsteps[1][0] << ", " << footsteps[1][1] << ", "<< footsteps[1][2] << endl;*/
				vector<float> speed(50, 0.6); 				
				cout << "------> Move Forward <------" << endl;
				for (int i = 0; i < 50; i+=2)
					legs[i] = "LLeg";
				for (int i = 1; i < 50; i+=2)
					legs[i] = "RLeg";
				//last argument determines if existing footsteps planned should 
				//be cleared
				motion.setFootStepsWithSpeed(legs, footsteps, speed, true); 
				//motion.walkTo(0,0,0);
				
				break;
			}
			case MOV_BACKWARD:
			{				
				vector<string> legs (50, "");
				vector<float> footstep(3, 0.0);
				footstep[0] = -0.2;
				footstep[1] = 0.0;
				footstep[2] = 0.0;
				
				AL::ALValue footsteps;// = AL::ALValue::array(footstep, footstep);
				//footsteps.arraySetSize(50);
				for (int i=0; i<50; ++i)
					footsteps.arrayPush(footstep);
				vector<float> speed(50, 0.6); 	
				cout << "footsteps: " << footsteps.getSize() << endl;			
				cout << "------> Move Backward <------" << endl;
				for (int i = 0; i < 50; i+=2)
					legs[i] = "LLeg";
				for (int i = 1; i < 50; i+=2)
					legs[i] = "RLeg";
				//last argument determines if existing footsteps planned should 
				//be cleared
				motion.setFootStepsWithSpeed(legs, footsteps, speed, true); 
				
				break;
			}
			case MOV_LEFT:		
			{		
				vector<string> legs (TURN_STEPS, "");
				vector<float> footstep(3, 0.0);
				footstep[0] = 0.0;
				footstep[1] = 0.0;
				footstep[2] = TURN_ANGLE * RAD;
				
				AL::ALValue footsteps;// = AL::ALValue::array(footstep, footstep);
				for (int i=0; i<TURN_STEPS; ++i)
					footsteps.arrayPush(footstep);		
				
				vector<float> speed(TURN_STEPS, 1.0); 				
				cout << "------> Move Left <------" << endl;
				for (int i = 0; i < TURN_STEPS; i+=2)
					legs[i] = "LLeg";
				for (int i = 1; i < TURN_STEPS; i+=2)
					legs[i] = "RLeg";
				//last argument determines if existing footsteps planned should 
				//be cleared
				motion.setFootStepsWithSpeed(legs, footsteps, speed, true); 
				
				break;
			}
			case MOV_RIGHT:
			{		
				vector<string> legs (TURN_STEPS, "");
				vector<float> footstep(3, 0.0);
				footstep[0] = 0.0;
				footstep[1] = 0.0;
				footstep[2] = -TURN_ANGLE * RAD;
				
				AL::ALValue footsteps;// = AL::ALValue::array(footstep, footstep);
				for (int i=0; i<TURN_STEPS; ++i)
					footsteps.arrayPush(footstep);
				
				vector<float> speed(TURN_STEPS, 1.0); 				
				cout << "------> Move Right <------" << endl;
				for (int i = 0; i < TURN_STEPS; i+=2)
					legs[i] = "LLeg";
				for (int i = 1; i < TURN_STEPS; i+=2)
					legs[i] = "RLeg";
				//last argument determines if existing footsteps planned should 
				//be cleared
				motion.setFootStepsWithSpeed(legs, footsteps, speed, true); 
				
				break;
			}
			case MOV_STOP:
			default:
			{
				if(motion.walkIsActive())
					motion.stopWalk();
				vector<string> legs (2, "");
				vector<float> footstep(3, 0.0);
				footstep[0] = 0.0;
				footstep[1] = 0.0;
				footstep[2] = 0.0;	
				
				AL::ALValue footsteps;	
				for (int i=0; i<2; ++i)
					footsteps.arrayPush(footstep);
				vector<float> speed(2, 1.0);
				legs[0] = "LLeg"; 
				legs[1] = "RLeg"; 					
				motion.setFootStepsWithSpeed(legs, footsteps, speed, true);
				
				//motion.wbFootState("Fixed", "Legs");
				
				cout << "------> Move STOP <------" << endl;
				//motion.stopWalk();
				break;
			}
		};
		
		while (motion.walkIsActive())			
			qi::os::msleep(50);
		cout << "walking done!" << endl;
		
	}
	catch(const AL::ALError& e)
	{
		cout << "Error [Executer]<walk>:" << endl << e.what() << endl;
	}
}

void Executer::moveHead(const Event& event)
{
	try
	{
		AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
		AL::ALMotionProxy motion(MB_IP, MB_PORT);
		int mode = event.ep.iparams[0];
		AL::ALValue jointNames;		
		AL::ALValue stiffList;		
		AL::ALValue stiffTime;
        AL::ALValue angleList;
        AL::ALValue angleTime;
		
		switch(mode)
		{
			case MOV_FORWARD: 
				cout<< ">>>MOVE HEAD RIGHT<<<" << endl;       
				jointNames.arrayPush("HeadPitch");
				stiffList.arrayPush(1.0);
				stiffTime.arrayPush(1.0);	
				motion.stiffnessInterpolation(jointNames, stiffList, stiffTime);
				angleList.arrayPush(HEAD_PITCH);
				angleTime.arrayPush(0.5);			
    			motion.angleInterpolation(jointNames, angleList, angleTime, false);
    			stiffList[0] = 0.0;
				motion.stiffnessInterpolation(jointNames, stiffList, stiffTime);
				break;
			case MOV_BACKWARD:
				cout<< ">>>MOVE HEAD RIGHT<<<" << endl;       
				jointNames.arrayPush("HeadPitch");
				stiffList.arrayPush(1.0);
				stiffTime.arrayPush(1.0);	
				motion.stiffnessInterpolation(jointNames, stiffList, stiffTime);
				angleList.arrayPush(-HEAD_PITCH);
				angleTime.arrayPush(0.5);			
    			motion.angleInterpolation(jointNames, angleList, angleTime, false);
    			stiffList[0] = 0.0;
				motion.stiffnessInterpolation(jointNames, stiffList, stiffTime);
				break;
			case MOV_LEFT:
				cout<< ">>>MOVE HEAD LEFT<<<" << endl;       
				jointNames.arrayPush("HeadYaw");
				stiffList.arrayPush(1.0);
				stiffTime.arrayPush(1.0);	
				motion.stiffnessInterpolation(jointNames, stiffList, stiffTime);
				angleList.arrayPush(HEAD_YAW);
				angleTime.arrayPush(0.5);			
    			motion.angleInterpolation(jointNames, angleList, angleTime, false);
    			stiffList[0] = 0.0;
				motion.stiffnessInterpolation(jointNames, stiffList, stiffTime);
				break;
			case MOV_RIGHT:
				cout<< ">>>MOVE HEAD RIGHT<<<" << endl;       
				jointNames.arrayPush("HeadYaw");
				stiffList.arrayPush(1.0);
				stiffTime.arrayPush(1.0);	
				motion.stiffnessInterpolation(jointNames, stiffList, stiffTime);
				angleList.arrayPush(-HEAD_YAW);
				angleTime.arrayPush(0.5);			
    			motion.angleInterpolation(jointNames, angleList, angleTime, false);
    			stiffList[0] = 0.0;
				motion.stiffnessInterpolation(jointNames, stiffList, stiffTime);
				break;
		}
    }
	catch(const AL::ALError& e)
	{
		cout << "Error [Executer]<moveHead>:" << endl << e.what() << endl;
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

void Executer::setBat()
{
	AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
	tts.say("Battery");
	cout<< "Battery Level changed!" << endl;
	bat = (int&)mem.getData("BatteryLevelChanged");
}

void Executer::sendBatteryStatus()
{
	int sclient;
	string buf = "BAT_";
	int value = 0;
	int sent= 0;
	try
	{
		//BatteryLevelChanged
		cout << "Batterylevel: " << (char)bat << endl;
		buf+=(char)(int&)mem.getData("BatteryLevelChanged");
		
		AL::ALProxy pNetNao = AL::ALProxy(string("RMNetNao"), CB_IP, CB_PORT);
		sclient = pNetNao.call<int>("getClient_tcp"); 
		
		cout << "buf.length() = " << (int)buf.length() << endl;
		sent = pNetNao.call<int, int, AL::ALValue, int, int>(
									"sendString", sclient, buf, buf.length(), 0);
		
	}
	catch (const AL::ALError& e)
	{
		cout<< "ERROR [Executer]<sendBatteryStatus>:" << endl << e.what() << endl;
	}
}



