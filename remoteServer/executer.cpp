#include <iostream>
#include <vector>
#include <alproxies/altexttospeechproxy.h>
#include <alproxies/alrobotposeproxy.h>
#include <alproxies/almotionproxy.h>
#include <alproxies/almemoryproxy.h>
#include <alproxies/albatteryproxy.h>
#include <alproxies/albehaviormanagerproxy.h>
#include <alproxies/dcmproxy.h>
#include <alcommon/alproxy.h>
#include <alvalue/alvalue.h>
#include <qi/os.hpp>
#include <almath/tools/almath.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>


#include "executer.h"
#include "gen.h"

using namespace std;
	
Executer::Executer(boost::shared_ptr<AL::ALBroker> broker, const string& name)
: 	AL::ALModule(broker, name),
	mutex(AL::ALMutex::createALMutex()),
	sync(AL::ALMutex::createALMutex())
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
	
	functionName("behave_stand", getName(), "stand");
	BIND_METHOD(Executer::behave_stand);
	
	
	functionName("behave_wipe", getName(), "stand");
	BIND_METHOD(Executer::behave_wipe);
	
	functionName("behave_sit", getName(), "sit");
	BIND_METHOD(Executer::behave_sit);
	
	functionName("behave_hello", getName(), "say hello");
	BIND_METHOD(Executer::behave_hello);
	
	functionName("behave_dance", getName(), "dance");
	BIND_METHOD(Executer::behave_dance);
	
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
	//addParam("event", "The event to be processed");
	BIND_METHOD(Executer::process);
	
	functionName("callback", getName(), "Callback");
	/*addParam("eventName", "The event to be processed");
	addParam("percentage", "The event to be processed");
	addParam("subscriberIdentifier", "The event to be processed");*/
	BIND_METHOD(Executer::callback);
	
	functionName("cbPoseChanged", getName(), "callback");
	addParam("eventName", "The event to be processed");
	addParam("postureName", "The name of posture");
	addParam("subscriberIdentifier", "ID");
	BIND_METHOD(Executer::cbPoseChanged);
}

Executer::~Executer()
{
}

void Executer::init()
{
	mem = AL::ALMemoryProxy(getParentBroker());
	mem.subscribeToEvent("BatteryLevelChanged", "RMExecuter", "callback");
	//mem.subscribeToEvent("BodyStiffnessChanged", "RMExecuter", "callback");
	mpose = (string&)mem.getData("robotPoseChanged");
	mem.subscribeToEvent("robotPoseChanged", "RMExecuter", "cbPoseChanged");
	
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
	sendState();
}


state_t Executer::getState(int type)
{
	int result;
	mutex->lock();
	result = state & type;
	mutex->unlock();
	return result;
}


int Executer::processConflicts(const Event& event)
{
	mutex->lock();
	//eventList->list();
	event_params_t ep;
	int classf = EVT_BUSY;
	
	eventList->setOrder(ORD_STRICT);
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
		case INIT_SIT:
			if (stateAbs == STATE_WALKING)
			{
				ep.type = CODE_STOP;
				ep.iparams[0] = MOV_STOP;
				eventList->addFirst(ep);
				classf = EVT_PENDINGABS;
			}	
			else if (stateAbs == STATE_CROUCHING)
			{
				ep.type = INIT_WALK;
				eventList->addFirst(ep);
				classf = EVT_PENDINGABS;
			}		
			else if ((stateAbs == STATE_MOVING) 	||
					 (stateAbs == STATE_STOPPING)		)
			{
				classf = EVT_PENDINGABS;
				eventList->setOrder(ORD_PAR);
			}
			else if (stateAbs != STATE_STANDING)
			{
				classf = EVT_DONE;
			}		
			break;
		case INIT_UP:
			if ((stateAbs == STATE_MOVING) || (stateAbs == STATE_STOPPING))
			{
				classf = EVT_DONE;
			}	
			else if((stateAbs == STATE_WALKING))
			{
				classf = EVT_PENDINGABS;
			}		
			break;
		case INIT_WIPE:
		case INIT_WAVE:
			if (stateAbs == STATE_MOVING)
			{
				classf = EVT_DONE;
			}	
			else if((stateAbs == STATE_WALKING) || (stateAbs == STATE_STOPPING))
			{
				classf = EVT_PENDINGABS;
			}		
			break;
		case INIT_DANCE:
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
				classf = EVT_PENDINGABS;
			}
			else if (stateAbs == STATE_SITTING)
			{
				ep.type = INIT_UP;
				eventList->addFirst(ep);
				classf = EVT_PENDINGABS;
			}
			else if (stateAbs == STATE_WALKING)
			{
				ep.type = CODE_STOP;
				ep.iparams[0] = MOV_STOP;
				eventList->addFirst(ep);
				classf = EVT_PENDINGABS;
			}
			else if ((stateAbs == STATE_MOVING) 	||
					 (stateAbs == STATE_STOPPING)		)
			{
				classf = EVT_PENDINGABS;
				eventList->setOrder(ORD_PAR);
			}
			else if (stateAbs != STATE_STANDING)
			{
				classf = EVT_DONE;
			}
			break;
		case CODE_HEAD:
			if (statePar == STATE_HEADMOVE)
			{
				classf = EVT_PENDINGPAR;
			}
			if (stateAbs == STATE_UNKNOWN)
				classf = EVT_DONE;
			break;
		case CODE_ARM:
			if (statePar == STATE_ARMMOVE)
			{
				classf = EVT_PENDINGPAR;
			}
			if (stateAbs == STATE_UNKNOWN)
				classf = EVT_DONE;
			break;
		case CODE_STOP:
			if (stateAbs != STATE_WALKING)
				classf = EVT_DONE;
			break;
		case CODE_BAT:
			break;
		case CODE_STATE:
			break;
		case RESET_CONNECTION:
			if (stateAbs == STATE_WALKING)
			{
				cout<< "DIS: first stop Movement" << endl;
				ep.type = CODE_STOP;
				ep.iparams[0] = MOV_STOP;
				eventList->addFirst(ep);
				classf = EVT_PENDINGABS;
			}			
			else if (stateAbs == STATE_STANDING)
			{
				cout<< "DIS: Now Rest" << endl;
				ep.type = INIT_REST;
				eventList->addFirst(ep);
				classf = EVT_PENDINGABS;
			}
			else if ((stateAbs == STATE_STOPPING) || (stateAbs == STATE_MOVING))
				classf = EVT_PENDINGABS;
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


void Executer::process()
{
	//cout<<"----->Process Event" << endl;
	int classf;	
	//cout << "PROCESS pevent = " << pevent.toString() << endl;
	//Event* event = & (eventList->withID((const void*)(*(const void**)(pevent.getObject()))));
	//const void* const id = pevent;
	//Event event(eventList->withID(id));
	
	mutex->lock();
	Event* event = eventList->getPending();
	if (!event)
	{
		mutex->unlock();
		return;
	}
	eventList->setClassf(event->id, EVT_BUSY);
	mutex->unlock();
	cout<< "[PROCESS] ID: " << event->id << ", TYPE: " << event->ep.type << endl;	
	classf = processConflicts(*event);
	eventList->setClassf(event->id, classf);
	if (classf == EVT_BUSY)
	{
		switch (event->ep.type)
		{
			case INIT_WALK:
				setState(getState(STATE_PARALLEL) | STATE_MOVING);
				initWalk();
				setState(STATE_STANDING);
				//cout << "Done initWalk()" << endl;
				break;
			case INIT_REST:
				setState(getState(STATE_PARALLEL) | STATE_MOVING);
				initSecure();
				setState(STATE_CROUCHING);
				break;
			case INIT_SIT:
				{
				
					setState(getState(STATE_PARALLEL) | STATE_MOVING);
					AL::ALProxy pexec(AL::ALProxy(string("RMExecuter"), AL::ALProxy::FORCED_LOCAL, 0));
					pexec.pCall("behave_sit");
					qi::os::msleep(10000);
					AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
					pbehav.stopBehavior("sit");
					//standToSit();
					//behave_sit();
					/*accessExec.pexec.pCall("behave_sit");
					AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
					qi::os::msleep(5000);
					pbehav.stopBehavior("behave_sit");*/
					setState(STATE_SITTING);
					break;
				}
			case INIT_UP:
				{
					
					setState(getState(STATE_PARALLEL) | STATE_MOVING);
					AL::ALProxy pexec(AL::ALProxy(string("RMExecuter"), AL::ALProxy::FORCED_LOCAL, 0));
					pexec.pCall("behave_stand");
					qi::os::msleep(12000);
				
					AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
					//while (pbehav.isBehaviorRunning("stand"));
						//qi::os::msleep(50);
					pbehav.stopBehavior("stand");
					//sitToStand();
					//behave_stand();
					/*accessExec.pexec.pCall("behave_stand");
					AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
					qi::os::msleep(5000);
					pbehav.stopBehavior("behave_stand");*/
					setState(STATE_STANDING);
					break;
				}
			case INIT_WAVE:
				{
					int state = getState(STATE_ABSOLUT);
					setState(getState(STATE_PARALLEL) | STATE_MOVING);
					AL::ALProxy pexec(AL::ALProxy(string("RMExecuter"), AL::ALProxy::FORCED_LOCAL, 0));
					pexec.pCall("behave_hello");
					qi::os::msleep(5000);
					AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
					pbehav.stopBehavior("hello");
					//sitToStand();
					//behave_stand();
					/*accessExec.pexec.pCall("behave_stand");
					AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
					qi::os::msleep(5000);
					pbehav.stopBehavior("behave_stand");*/
					setState(getState(STATE_PARALLEL) | state);
					break;
				}
			case INIT_WIPE:
				{
					
					int state = getState(STATE_ABSOLUT);  
					setState(getState(STATE_PARALLEL) | STATE_MOVING);
					AL::ALProxy pexec(AL::ALProxy(string("RMExecuter"), AL::ALProxy::FORCED_LOCAL, 0));
					pexec.pCall("behave_wipe");
					qi::os::msleep(5000);
					AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
					pbehav.stopBehavior("wipe");
					//sitToStand();
					//behave_stand();
					/*accessExec.pexec.pCall("behave_stand");
					AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
					qi::os::msleep(5000);
					pbehav.stopBehavior("behave_stand");*/
					
					setState(getState(STATE_PARALLEL) | state);
					break;
				}
			case INIT_DANCE:
				{
					
					setState(getState(STATE_PARALLEL) | STATE_MOVING);
					AL::ALProxy pexec(AL::ALProxy(string("RMExecuter"), AL::ALProxy::FORCED_LOCAL, 0));
					pexec.pCall("behave_dance");
					qi::os::sleep(50);
					AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
					pbehav.stopBehavior("dance");
					//sitToStand();
					//behave_stand();
					/*accessExec.pexec.pCall("behave_stand");
					AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
					qi::os::msleep(5000);
					pbehav.stopBehavior("behave_stand");*/
					setState(STATE_STANDING);
					break;
				}
			case CODE_SPK:
				speak(event->ep.sparam);
				break;
			case CODE_MOV:
				setState(getState(STATE_PARALLEL) | STATE_WALKING);
				walk(*event);
				sync->lock();
				setState(STATE_STANDING);
				sync->unlock();
				break;
			case CODE_HEAD:
				setState(getState() | STATE_HEADMOVE);
				moveHead(*event);
				setState(getState() & ~STATE_HEADMOVE);
				break;
			case CODE_ARM:
				setState(getState() | STATE_ARMMOVE);
				moveArm(*event);
				setState(getState() & ~STATE_ARMMOVE);
				break;
			//TODO: Stopping can trigger while walking before acutally walking
			//		Stop would be done before the walking started --> 
			//  	Stop would have no actual result
			case CODE_STOP:
				//event.ep.iparams[0] = MOV_STOP;
				sync->lock();
				setState(getState(STATE_PARALLEL) | STATE_STOPPING);
				walk(*event);
				setState(STATE_STANDING);
				sync->unlock();
				cout<< "STP: NOW STANDING" << endl;
				break;
			case CODE_BAT:			
				sendBatteryStatus();
				break;
			case CODE_STATE:			
				sendState();
				break;
			case RESET_CONNECTION:
				setState(getState(STATE_PARALLEL) | STATE_CROUCHING);
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
				
		eventList->setClassf(event->id, EVT_DONE);	
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
		
		
		AL::ALMotionProxy motion(MB_IP, MB_PORT);
		//boost::shared_ptr<AL::ALMotionProxy> motion = getParentBroker()->getMotionProxy(); 
        motion.stiffnessInterpolation(snames, stiffnesses, 1.0);
        //motion->setStiffnesses(snames, stiffnesses);
		motion.walkInit(); 
		//motion->walkTo(x,y,t);
	}
	catch(const AL::ALError& e)
	{
		cout<< "Error initWalk: " << e.what() << endl; 
	}
}


void Executer::joints()
{
		AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
		AL::ALMotionProxy motion(MB_IP, MB_PORT);
		
		float stiffnesses  = 1.0f; 
		string snames = "Body";
        motion.stiffnessInterpolation("Body", 1.0, 1.0);
        
        AL::ALValue names = "Body";
        float ftargetAngles[] = {
        						 	 0.5, 0.2,
		    						 0.0, 90.0, -0.0, -90.0, 0.0, 0.0, //80.0
		    						 -0.0, 0.0, -10.0/2 + 0.0, 10.0, -10.0/2, -0.0,
		    						 -0.0, -0.0, -10.0/2 + 0.0, 10.0, -10.0/2, 0.0,
		    						 0.0, -90.0, +90.0, +0.0, 0.0, 0.0 //80.0
        						};
        						
        //shoulderPitch == 0.0
       // cout<< "sizeof(ftargetAngles) = " << sizeof(ftargetAngles) << endl;
        for (int i = 0; i < sizeof(ftargetAngles)/sizeof(float); ++i)
        	ftargetAngles[i]*= RAD;

       	std::vector<float> vtargetAngles(ftargetAngles,ftargetAngles + sizeof(ftargetAngles)/ sizeof(float));
        AL::ALValue targetAngles(vtargetAngles);
        float maxSpeedFraction = 0.2;
        motion.angleInterpolationWithSpeed(names, targetAngles, maxSpeedFraction);
}

void Executer::standToSit()
{
		AL::ALMotionProxy motion(MB_IP, MB_PORT);
		
		float stiffnesses  = 1.0f; 
		string snames = "Body";
        motion.stiffnessInterpolation("Body", 1.0, 1.0);		
        
        AL::ALValue names = "Body";
        /*
       		HeadYawAngle, HeadPitchAngle
        	ShoulderPitchAngle, +ShoulderRollAngle, +ElbowYawAngle, +ElbowRollAngle, WristYawAngle, HandAngle
        	hipYawPitch,  spreadAngle, -kneeAngle/2-torsoAngle, kneeAngle, -kneeAngle/2, -spreadAngle
        	-hipYawPitch, -spreadAngle, -kneeAngle/2-torsoAngle, kneeAngle, -kneeAngle/2,  spreadAngle
        	ShoulderPitchAngle, -ShoulderRollAngle, -ElbowYawAngle, -ElbowRollAngle, WristYawAngle, HandAngle
        */
        {
        	float ftargetAngles[] = {
		    						 	 0.0, 0.0,
										 119.0, -18.0, +0.0, -0.0, 0.0, 0.0,
										 -35.0, 15.0, -150.0/2 - 0.0, 150.0, -25.0, -15.0,
										 -35.0, -15.0, -150.0/2 - 0.0, 150.0, -25.0, 15.0,
										 119.0, 18.0, -0.0, +0.0, 0.0, 0.0
		    						};
		    						
		    for (int i = 0; i < sizeof(ftargetAngles)/sizeof(float); ++i)
		    	ftargetAngles[i]*= RAD;
		   	std::vector<float> vtargetAngles(ftargetAngles,ftargetAngles + sizeof(ftargetAngles)/ sizeof(float));
		    AL::ALValue targetAngles(vtargetAngles);
		    float maxSpeedFraction = 0.2;
		    
		    motion.angleInterpolationWithSpeed(names, targetAngles, maxSpeedFraction);
        }
        
        names.clear();
        names = AL::ALValue::array("LLeg", "RLeg");
        {
        	float ftargetAngles[] = {
										 -45.0, 15.0, -200.0/2 - 50.0, 200.0, -0.0, -15.0,
										 -45.0, -15.0, -200.0/2 - 50.0, 200.0, -0.0, 15.0,
		    						};
		    						
		    for (int i = 0; i < sizeof(ftargetAngles)/sizeof(float); ++i)
		    	ftargetAngles[i]*= RAD;
		   	std::vector<float> vtargetAngles(ftargetAngles,ftargetAngles + sizeof(ftargetAngles)/ sizeof(float));
		    AL::ALValue targetAngles(vtargetAngles);
		    float maxSpeedFraction = 0.2;
		    
		    motion.angleInterpolationWithSpeed(names, targetAngles, maxSpeedFraction);
        }
        
        names.clear();
        names = AL::ALValue::array("LLeg", "RLeg");
        {
        	float ftargetAngles[] = {
										 -45.0, 15.0, -200.0/2 - 50.0, 200.0, 20.0, -15.0,
										 -45.0, -15.0, -200.0/2 - 50.0, 200.0, 20.0, 15.0,
		    						};
		    						
		    for (int i = 0; i < sizeof(ftargetAngles)/sizeof(float); ++i)
		    	ftargetAngles[i]*= RAD;
		   	std::vector<float> vtargetAngles(ftargetAngles,ftargetAngles + sizeof(ftargetAngles)/ sizeof(float));
		    AL::ALValue targetAngles(vtargetAngles);
		    float maxSpeedFraction = 0.2;
		    
		    motion.angleInterpolationWithSpeed(names, targetAngles, maxSpeedFraction);
        }
        
        
        qi::os::msleep(500);
        names.clear();
        names = AL::ALValue::array("LLeg", "RLeg");
        {
        	float ftargetAngles[] = {
										 -45.0, 15.0, -90, 60.0, 60.0, -15.0,
										 -45.0, -15.0, -90, 60.0, 60.0, 15.0,
		    						};
		    						
		    for (int i = 0; i < sizeof(ftargetAngles)/sizeof(float); ++i)
		    	ftargetAngles[i]*= RAD;
		   	std::vector<float> vtargetAngles(ftargetAngles,ftargetAngles + sizeof(ftargetAngles)/ sizeof(float));
		    AL::ALValue targetAngles(vtargetAngles);
		    float maxSpeedFraction = 0.2;
		    
		    motion.angleInterpolationWithSpeed(names, targetAngles, maxSpeedFraction);
        }     
}

void Executer::behave_stand()
{
		AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
		AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
		AL::ALValue behav = pbehav.getDefaultBehaviors();
		pbehav.preloadBehavior("stand");
		pbehav.runBehavior("stand");
		//tts.say("Behaviour done");
}
		
		
void Executer::behave_sit()
{
		AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
		AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
		AL::ALValue behav = pbehav.getDefaultBehaviors();
		pbehav.preloadBehavior("sit");
		pbehav.runBehavior("sit");
		//tts.say("Behaviour done");

}		
		
void Executer::behave_wipe()
{
		AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
		AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
		AL::ALValue behav = pbehav.getDefaultBehaviors();
		pbehav.preloadBehavior("wipe");
		pbehav.runBehavior("wipe");
		//tts.say("Behaviour done");

}		
		
void Executer::behave_hello()
{
		AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
		AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
		AL::ALValue behav = pbehav.getDefaultBehaviors();
		pbehav.preloadBehavior("hello");
		pbehav.runBehavior("hello");
		//tts.say("Behaviour done");

}		
		
void Executer::behave_dance()
{
		AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
		AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
		AL::ALValue behav = pbehav.getDefaultBehaviors();
		pbehav.preloadBehavior("dance");
		pbehav.runBehavior("dance");
		//tts.say("Behaviour done");

}

void Executer::sitToStand()
{        
		AL::ALMotionProxy motion(MB_IP, MB_PORT);
		AL::ALValue names;
		 
        motion.stiffnessInterpolation("Body", 1.0, 1.0);	

		names.clear();
        names = AL::ALValue::array("LLeg", "RLeg", "LArm", "RArm");
        {
        	float ftargetAngles[] = {
										 -45.0, 15.0, -100, 120.0, 20.0, -15.0,
										 -45.0, -15.0, -90, 60.0, 60.0, 15.0,
										 119.0, -18.0, +0.0, -0.0, 0.0, 0.0,
										 100.0, -25.0, -0.0, +0.0, 0.0, 0.0
		    						};
		    						
		    for (int i = 0; i < sizeof(ftargetAngles)/sizeof(float); ++i)
		    	ftargetAngles[i]*= RAD;
		   	std::vector<float> vtargetAngles(ftargetAngles,ftargetAngles + sizeof(ftargetAngles)/ sizeof(float));
		    AL::ALValue targetAngles(vtargetAngles);
		    float maxSpeedFraction = 0.2;
		    
		    motion.angleInterpolationWithSpeed(names, targetAngles, maxSpeedFraction);
        }     
            	
        names.clear();
        names = AL::ALValue::array("LLeg", "RLeg", "LArm", "RArm");
        {
        	float ftargetAngles[] = {
										 -45.0, 15.0, -100, 120.0, 20.0, -15.0,
										 -45.0, -15.0, -90, 60.0, 60.0, 15.0,
										 119.0, -18.0, +0.0, -0.0, 0.0, 0.0,
										 100.0, -25.0, -0.0, +0.0, 0.0, 0.0
		    						};
		    						
		    for (int i = 0; i < sizeof(ftargetAngles)/sizeof(float); ++i)
		    	ftargetAngles[i]*= RAD;
		   	std::vector<float> vtargetAngles(ftargetAngles,ftargetAngles + sizeof(ftargetAngles)/ sizeof(float));
		    AL::ALValue targetAngles(vtargetAngles);
		    float maxSpeedFraction = 0.2;
		    
		    motion.angleInterpolationWithSpeed(names, targetAngles, maxSpeedFraction);
        }      
        
        //CHANGE LARM    	
        names.clear();
        names = AL::ALValue::array("LLeg", "RLeg", "LArm", "RArm");
        {
        	float ftargetAngles[] = {
										 -45.0, 10.0, -50, 120.0, -20.0, -15.0,
										 -45.0, -15.0, -90, 60.0, 60.0, -15.0,
										 0.0, 30.0, +0.0, -20.0, 0.0, 0.0,
										 100.0, -25.0, -0.0, +0.0, 0.0, 0.0
		    						};
		    						
		    for (int i = 0; i < sizeof(ftargetAngles)/sizeof(float); ++i)
		    	ftargetAngles[i]*= RAD;
		   	std::vector<float> vtargetAngles(ftargetAngles,ftargetAngles + sizeof(ftargetAngles)/ sizeof(float));
		    AL::ALValue targetAngles(vtargetAngles);
		    float maxSpeedFraction = 0.2;
		    
		    motion.angleInterpolationWithSpeed(names, targetAngles, maxSpeedFraction);
        }   
                  	
        names.clear();
        names = AL::ALValue::array("LLeg", "RLeg", "LArm", "RArm");
        {
        	float ftargetAngles[] = {
										 -45.0, 10.0, -0.0, 120.0, -50.0, -15.0,
										 -45.0, -40.0, -100.0, 60.0, 60.0, -15.0,
										 60.0, 0.0, +0.0, -20.0, 0.0, 0.0,
										 100.0, -25.0, -0.0, +0.0, 0.0, 0.0
		    						};
		    						
		    for (int i = 0; i < sizeof(ftargetAngles)/sizeof(float); ++i)
		    	ftargetAngles[i]*= RAD;
		   	std::vector<float> vtargetAngles(ftargetAngles,ftargetAngles + sizeof(ftargetAngles)/ sizeof(float));
		    AL::ALValue targetAngles(vtargetAngles);
		    float maxSpeedFraction = 0.2;
		    
		    motion.angleInterpolationWithSpeed(names, targetAngles, maxSpeedFraction);
        } 
                          	
        names.clear();
        names = AL::ALValue::array("LLeg", "RLeg", "LArm", "RArm");
        {
        	float ftargetAngles[] = {
										 -45.0, 10.0, 0.0, 120.0, -50.0, -20.0,
										 -45.0, -40.0, -100.0, 60.0, 60.0, 0.0,
										 60.0, -18.0, +0.0, -20.0, 0.0, 0.0,
										 119.0, -20.0, -0.0, +0.0, 0.0, 0.0
		    						};
		    						
		    for (int i = 0; i < sizeof(ftargetAngles)/sizeof(float); ++i)
		    	ftargetAngles[i]*= RAD;
		   	std::vector<float> vtargetAngles(ftargetAngles,ftargetAngles + sizeof(ftargetAngles)/ sizeof(float));
		    AL::ALValue targetAngles(vtargetAngles);
		    float maxSpeedFraction = 0.2;
		    
		    motion.angleInterpolationWithSpeed(names, targetAngles, maxSpeedFraction);
        }         
        
        names.clear();
        names = AL::ALValue::array("LLeg", "RLeg", "LArm", "RArm");
        {
        	float ftargetAngles[] = {
										 -45.0, 0.0, 0.0, 120.0, -50.0, -20.0,
										 -45.0, -40.0, -100.0, 100.0, 20.0, 0.0,
										 60.0, -18.0, +0.0, -20.0, 0.0, 0.0,
										 119.0, -20.0, -0.0, +0.0, 0.0, 0.0
		    						};
		    						
		    for (int i = 0; i < sizeof(ftargetAngles)/sizeof(float); ++i)
		    	ftargetAngles[i]*= RAD;
		   	std::vector<float> vtargetAngles(ftargetAngles,ftargetAngles + sizeof(ftargetAngles)/ sizeof(float));
		    AL::ALValue targetAngles(vtargetAngles);
		    float maxSpeedFraction = 0.2;
		    
		    motion.angleInterpolationWithSpeed(names, targetAngles, maxSpeedFraction);
        } 
          
         //changed knee 100 to 80, left 120
         //ankle right 10 to 20 to 10
         //ankle left -30 to 0
         //rshoulderroll -20 
         //rshoulderpitch 119 to 100
         //rhippitch -40 to -60
         //lhippitch -0 to -20
        names.clear();
        names = AL::ALValue::array("LLeg", "RLeg", "LArm", "RArm");
        {
        	float ftargetAngles[] = {
										 -45.0, -20.0, -20.0, 120.0, -40.0, -20.0,
										 -45.0, -60.0, -100.0, 80.0, 10.0, 0.0,
										 20.0, -18.0, +0.0, -20.0, 0.0, 0.0,
										 100.0, -20.0, -0.0, +0.0, 0.0, 0.0
		    						};
		    						
		    for (int i = 0; i < sizeof(ftargetAngles)/sizeof(float); ++i)
		    	ftargetAngles[i]*= RAD;
		   	std::vector<float> vtargetAngles(ftargetAngles,ftargetAngles + sizeof(ftargetAngles)/ sizeof(float));
		    AL::ALValue targetAngles(vtargetAngles);
		    float maxSpeedFraction = 0.2;
		    
		    motion.angleInterpolationWithSpeed(names, targetAngles, maxSpeedFraction);
        }  
              
        //rshoulderpitch 119 to 80  
        //ankle right -10 to -40 to 20
        //ankle left -10 to -40 to 20
        names.clear();
        names = AL::ALValue::array("LLeg", "RLeg", "LArm", "RArm");
        {
        	float ftargetAngles[] = {
										 -45.0, 0.0, -100.0, 120.0, 20.0, -0.0,
										 -45.0, -0.0, -100.0, 120.0, 20.0, 0.0,
										 20.0, -18.0, +0.0, -20.0, 0.0, 0.0,
										 80.0, -20.0, -20.0, +0.0, 0.0, 0.0
		    						};
		    						
		    for (int i = 0; i < sizeof(ftargetAngles)/sizeof(float); ++i)
		    	ftargetAngles[i]*= RAD;
		   	std::vector<float> vtargetAngles(ftargetAngles,ftargetAngles + sizeof(ftargetAngles)/ sizeof(float));
		    AL::ALValue targetAngles(vtargetAngles);
		    float maxSpeedFraction = 0.2;
		    
		    motion.angleInterpolationWithSpeed(names, targetAngles, maxSpeedFraction);
        } 
        
		motion.walkInit();
}



void initSit2()
{
		AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
		AL::ALMotionProxy motion(MB_IP, MB_PORT);
		
		float stiffnesses  = 1.0f; 
		string snames = "Body";
        motion.stiffnessInterpolation("Body", 1.0, 1.0);		
        
        /*motion.wbEnable(true); 
		motion.wbFootState("Plane", "LLeg");
		motion.wbFootState("Plane", "RLeg");
		motion.wbEnableEffectorOptimization("LArm", true);
		motion.wbEnableEffectorOptimization("RArm", true);*/
        
        AL::ALValue names = "Body";
        /*
       		HeadYawAngle, HeadPitchAngle
        	ShoulderPitchAngle, +ShoulderRollAngle, +ElbowYawAngle, +ElbowRollAngle, WristYawAngle, HandAngle
        	hipYawPitch,  spreadAngle, -kneeAngle/2-torsoAngle, kneeAngle, -kneeAngle/2, -spreadAngle
        	-hipYawPitch, -spreadAngle, -kneeAngle/2-torsoAngle, kneeAngle, -kneeAngle/2,  spreadAngle
        	ShoulderPitchAngle, -ShoulderRollAngle, -ElbowYawAngle, -ElbowRollAngle, WristYawAngle, HandAngle
        */
        {
        
       
		   float ftargetAngles[] = {
		    						 	 0.0, 0.0,
										 60.0, 0.0, +0.0, -60.0, 0.0, 0.0,
										 -35.0, 15.0, -100.0/2 + 25.0, 100.0, -100.0/2, -15.0,
										 -35.0, -15.0, -100.0/2 + 25.0, 100.0, -100.0/2, 15.0,
										 100.0, -0.0, -0.0, +0.0, 0.0, 0.0
		    						};
		    						
		    for (int i = 0; i < sizeof(ftargetAngles)/sizeof(float); ++i)
		    	ftargetAngles[i]*= RAD;
		   	std::vector<float> vtargetAngles(ftargetAngles,ftargetAngles + sizeof(ftargetAngles)/ sizeof(float));
		    AL::ALValue targetAngles(vtargetAngles);
		    float maxSpeedFraction = 0.2;
		    
		    motion.angleInterpolationWithSpeed(names, targetAngles, maxSpeedFraction);
        }



		motion.wbEnable(true); 
		string effector 	 = "Torso";
		AL::ALValue path     = AL::ALValue(motion.getPosition(effector, SPACE_NAO, true));
		//path[0] = (float&)path[0] - 0.3f;
		path[1] = (float&)path[1] + 0.5f;
		//path[2] = 0.0f;//(float&)path[2] - 0.1f;
		//AL::ALValue path     = AL::ALValue::array(-0.0f, -0.00f, -0.0f, 0.0f, 0.0f, 0.0f);
		AL::ALValue timeList = 2.0f; // seconds
		int axisMask = 7; // control all axis of the effector
		motion.positionInterpolation(effector, SPACE_NAO, path,
                                     axisMask, timeList, true);
                                     

		/*motion.wbFootState("Plane", "LLeg");
		motion.wbFootState("Free", "RLeg");	
		motion.wbEnableBalanceConstraint(true, "LLeg");
		motion.wbGoToBalance("LLeg", 1.0);*/
		
		effector 	 = "RLeg";
		path     = AL::ALValue::array(0.25f, -0.07f, -0.0f, 0.0f, 0.0f, 0.0f);
	    axisMask = 7; // control all axis of the effector
		timeList = 2.0f; // seconds
		motion.positionInterpolation(effector, SPACE_NAO, path,
                                     axisMask, timeList, false);
                                 
  		//motion.wbFootState("Plane", "Legs");        
		//motion.wbGoToBalance("Legs", 1.0);      
		                             				
        /*effector 	 = "LLeg";
		path     = AL::ALValue::array(0.1f, 0.1f, -0.0f, 0.0f, 0.0f, 0.0f);
	    axisMask = 7; // control all axis of the effector
		timeList = 2.0f; // seconds
		motion.positionInterpolation(effector, SPACE_NAO, path,
                                     axisMask, timeList, false);*/
        
        //motion.wbEnable(false);                              		
	                                     
                                     
		//motion.wbFootState("Plane", "Legs");        
		//motion.wbGoToBalance("Legs", 1.0);
		
		
					
		                   
		motion.wbGoToBalance("Legs", 1.0);         
                                     
        effector 	 = "Torso";
		path     = AL::ALValue(motion.getPosition(effector, SPACE_NAO, true));
		//path[0] = (float&)path[0] - 0.3f;
		//path[1] = (float&)path[1] - 0.1f;
		//path[2] = 0.0f;//(float&)path[2] - 0.1f;
		path[4] = (float&)path[4] - 20*RAD;
		//path[4] = (float&)path[3] - 90*RAD;
	    axisMask = 63; // control all axis of the effector
		timeList = 2.0f; // seconds
		motion.positionInterpolation(effector, SPACE_NAO, path,
                                     axisMask, timeList, true);  
                                     
                                     
       /* effector 	 = "Torso";
		path     = AL::ALValue(motion.getPosition(effector, SPACE_NAO, true));
		//path[0] = (float&)path[0] - 0.3f;
		//path[1] = (float&)path[1] - 0.1f;
		//path[2] = 0.0f;//(float&)path[2] - 0.1f;
		path[5] = (float&)path[5] + 20*RAD;
		//path[4] = (float&)path[3] - 90*RAD;
	    axisMask = 63; // control all axis of the effector
		timeList = 2.0f; // seconds
		motion.positionInterpolation(effector, SPACE_NAO, path,
                                     axisMask, timeList, true);  */                                
                                     
                                                                         
       	effector 	 = "Torso";
		path     = AL::ALValue(motion.getPosition(effector, SPACE_NAO, true));
		//path[0] = (float&)path[0] - 0.3f;
		//path[1] = (float&)path[1] - 0.1f;
		//path[2] = 0.0f;//(float&)path[2] - 0.1f;
		path[3] = (float&)path[3] + 60*RAD;
		//path[4] = (float&)path[3] - 90*RAD;
	    axisMask = 63; // control all axis of the effector
		timeList = 2.0f; // seconds
		motion.positionInterpolation(effector, SPACE_NAO, path,
                                     axisMask, timeList, true); 
                                     
                                     
        effector 	 = "Torso";
		path     = AL::ALValue(motion.getPosition(effector, SPACE_NAO, true));
		//path[0] = (float&)path[0] - 0.3f;
		//path[1] = (float&)path[1] - 0.1f;
		path[2] = 0.f;//(float&)path[2] - 0.1f;
		//path[3] = (float&)path[3] + 90*RAD;
	    axisMask = 7; // control all axis of the effector
		timeList = 2.0f; // seconds
		motion.positionInterpolation(effector, SPACE_NAO, path,
                                     axisMask, timeList, true);   
                                     
        //motion.wbGoToBalance("RLeg", 1.0);                           
		
		motion.wbEnable(false); 
		


		
		
		
		//motion.wbEnableEffectorOptimization("RArm", true);
		//motion.wbEnableEffectorOptimization("LArm", true);
		//motion.wbSetEffectorControl("RArm", AL::ALValue::array()); 
       // motion.wbGoToBalance("RLeg", 1.0);
		/*motion.wbFootState("Free", "LLeg");
		motion.wbFootState("Plane", "RLeg");	
		motion.wbEnableBalanceConstraint(false, "Legs");
		motion.wbEnableBalanceConstraint(true, "RLeg");    */
        //motion.wbGoToBalance("RLeg", 1.0);    
		
		
		
		names.clear();
		names = AL::ALValue::array("LArm");
        {
        
       
		   float ftargetAngles[] = {
										 100.0, 60.0, +0.0, -0.0, 0.0, 0.0,
								   };
		    						
		    for (int i = 0; i < sizeof(ftargetAngles)/sizeof(float); ++i)
		    	ftargetAngles[i]*= RAD;
		   	std::vector<float> vtargetAngles(ftargetAngles,ftargetAngles + sizeof(ftargetAngles)/ sizeof(float));
		    AL::ALValue targetAngles(vtargetAngles);
		    float maxSpeedFraction = 0.2;
		    
		    motion.angleInterpolationWithSpeed(names, targetAngles, maxSpeedFraction);
        } 
        
        names.clear();
		names = AL::ALValue::array("LArm");
        {
        
       
		   float ftargetAngles[] = {
										 -35.0, 15.0, -100.0/2 + 25.0, 100.0, -100.0/2, -15.0,
								   };
		    						
		    for (int i = 0; i < sizeof(ftargetAngles)/sizeof(float); ++i)
		    	ftargetAngles[i]*= RAD;
		   	std::vector<float> vtargetAngles(ftargetAngles,ftargetAngles + sizeof(ftargetAngles)/ sizeof(float));
		    AL::ALValue targetAngles(vtargetAngles);
		    float maxSpeedFraction = 0.2;
		    
		    motion.angleInterpolationWithSpeed(names, targetAngles, maxSpeedFraction);
        }  	
        
        
        names.clear();
		names = AL::ALValue::array("LLeg");
        {
        
       
		   float ftargetAngles[] = {
										 119.0, -18.0, +0.0, -0.0, 0.0, 0.0,
								   };
		    						
		    for (int i = 0; i < sizeof(ftargetAngles)/sizeof(float); ++i)
		    	ftargetAngles[i]*= RAD;
		   	std::vector<float> vtargetAngles(ftargetAngles,ftargetAngles + sizeof(ftargetAngles)/ sizeof(float));
		    AL::ALValue targetAngles(vtargetAngles);
		    float maxSpeedFraction = 0.2;
		    
		    motion.angleInterpolationWithSpeed(names, targetAngles, maxSpeedFraction);
        } 
        
       
                                     
        //motion.wbGoToBalance("RLeg", 1.0);                           
		
		motion.wbEnable(false); 
			
		/*effector 	 = "LLeg";
		path     = AL::ALValue::array(0.2f, 0.1f, -0.0f, 0.0f, 0.0f, 0.0f);
	    axisMask = 63; // control all axis of the effector
		timeList = 2.0f; // seconds
		motion.positionInterpolation(effector, SPACE_NAO, path,
                                     axisMask, timeList, false);*/
		
   
                                    
 		
		/*motion.wbEnableEffectorOptimization("RArm", false);
		motion.wbEnableEffectorOptimization("LArm", false);
		//motion.wbSetEffectorControl("RArm", AL::ALValue::array());
		motion.wbFootState("Free", "LLeg");
		motion.wbFootState("Free", "RLeg");	
		motion.wbEnableBalanceConstraint(false, "Legs");
		motion.wbEnableBalanceConstraint(false, "RLeg");   
		
        effector 	 = "Torso";
		path     = AL::ALValue(motion.getPosition(effector, SPACE_NAO, true));
		//path[0] = (float&)path[0] - 0.3f;
		//path[1] = (float&)path[1] - 0.1f;
		path[2] = 0.0f;//(float&)path[2] - 0.1f;
		//path[3] = (float&)path[3] + 90*RAD;
	    axisMask = 7; // control all axis of the effector
		timeList = 2.0f; // seconds
		motion.positionInterpolation(effector, SPACE_NAO, path,
                                     axisMask, timeList, true); 
		
                                     
        
        */

		
		//motion.wbEnable(false);
		
		//stiffnesses  = 0.0f; 
        //motion.stiffnessInterpolation("Body", 0.0, 1.2);
        
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
			qi::os::msleep(20);
		cout << "walking done!" << endl;
		
	}
	catch(const AL::ALError& e)
	{
		cout << "Error [Executer]<walk>:" << endl << e.what() << endl;
	}
}

void Executer::moveHead(const Event& event)
{
	int mode = event.ep.iparams[0];
	AL::ALValue jointNames;		
	AL::ALValue stiffList;		
	AL::ALValue stiffTime;
    AL::ALValue angleList;
    AL::ALValue angleTime;
        	
	try
	{
		AL::ALMotionProxy motion(MB_IP, MB_PORT);
			
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

void Executer::moveArm(const Event& event)
{	
	AL::ALValue jointNames;		
	AL::ALValue stiffList;		
	AL::ALValue stiffTime;
    AL::ALValue angleList;
    AL::ALValue angleTime;
	int arm = event.ep.iparams[0];  
	int mode = event.ep.iparams[1]; 
	jointNames.arrayPush("Body");
	stiffList.arrayPush(0.6);
	stiffTime.arrayPush(1.0);	
	cout<< "MOVE_ARM" << endl;
	
	if (arm == ARM_LEFT)
	{
		try
		{
			AL::ALMotionProxy motion(MB_IP, MB_PORT);
			motion.stiffnessInterpolation(jointNames, stiffList, stiffTime);
			switch(mode)
			{
				case MOV_FORWARD: 
					cout<< ">>>ARM FORWARD<<<" << endl;
					jointNames.clear(); 
					jointNames.arrayPush("LShoulderPitch");      
					angleList.arrayPush(-ARM_SHOULDER_PITCH*RAD);
					angleTime.arrayPush(0.5);			
					motion.angleInterpolation(jointNames, angleList, angleTime, false);
					break;
				case MOV_BACKWARD:
					cout<< ">>>ARM BACKWARD<<<" << endl;
					jointNames.clear(); 
					jointNames.arrayPush("LShoulderPitch");      
					angleList.arrayPush(ARM_SHOULDER_PITCH*RAD);
					angleTime.arrayPush(0.5);			
					motion.angleInterpolation(jointNames, angleList, angleTime, false);
					break;
				case MOV_LEFT:
					cout<< ">>>ARM LEFT<<<" << endl;
					jointNames.clear(); 
					jointNames.arrayPush("LShoulderRoll");      
					angleList.arrayPush(ARM_SHOULDER_ROLL*RAD);
					angleTime.arrayPush(0.5);			
					motion.angleInterpolation(jointNames, angleList, angleTime, false);
					break;
				case MOV_RIGHT:
					cout<< ">>>ARM RIGHT<<<" << endl;
					jointNames.clear(); 
					jointNames.arrayPush("LShoulderRoll");      
					angleList.arrayPush(-ARM_SHOULDER_ROLL*RAD);
					angleTime.arrayPush(0.5);			
					motion.angleInterpolation(jointNames, angleList, angleTime, false);
					break;
			}
		
		}
		catch(const AL::ALError& e)
		{
			cout << "Error [Executer]<moveHead>:" << endl << e.what() << endl;
		}
	}
	else
	{
		try
		{
			AL::ALMotionProxy motion(MB_IP, MB_PORT);
			motion.stiffnessInterpolation(jointNames, stiffList, stiffTime);
			switch(mode)
			{
				case MOV_FORWARD: 
					cout<< ">>>ARM FORWARD<<<" << endl;
					jointNames.clear(); 
					jointNames.arrayPush("RShoulderPitch");      
					angleList.arrayPush(-ARM_SHOULDER_PITCH*RAD);
					angleTime.arrayPush(0.5);			
					motion.angleInterpolation(jointNames, angleList, angleTime, false);
					break;
				case MOV_BACKWARD:
					cout<< ">>>ARM BACKWARD<<<" << endl;
					jointNames.clear(); 
					jointNames.arrayPush("RShoulderPitch");      
					angleList.arrayPush(ARM_SHOULDER_PITCH*RAD);
					angleTime.arrayPush(0.5);			
					motion.angleInterpolation(jointNames, angleList, angleTime, false);
					break;
				case MOV_LEFT:
					cout<< ">>>ARM LEFT<<<" << endl;
					jointNames.clear(); 
					jointNames.arrayPush("RShoulderRoll");      
					angleList.arrayPush(ARM_SHOULDER_ROLL*RAD);
					angleTime.arrayPush(0.5);			
					motion.angleInterpolation(jointNames, angleList, angleTime, false);
					break;
				case MOV_RIGHT:
					cout<< ">>>ARM RIGHT<<<" << endl;
					jointNames.clear(); 
					jointNames.arrayPush("RShoulderRoll");      
					angleList.arrayPush(-ARM_SHOULDER_ROLL*RAD);
					angleTime.arrayPush(0.5);			
					motion.angleInterpolation(jointNames, angleList, angleTime, false);
					break;
			}
		
		}
		catch(const AL::ALError& e)
		{
			cout << "Error [Executer]<moveHead>:" << endl << e.what() << endl;
		}
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



void Executer::sendBatteryStatus()
{
	int sclient;
	int value = 0;
	int sent= 0;
	string buf = "BAT_";
	char str[15];
	try
	{
		AL::ALBatteryProxy pbat = AL::ALBatteryProxy(MB_IP, MB_PORT);
		AL::DCMProxy dcm = AL::DCMProxy(MB_IP, MB_PORT);
		
		/*vector<string> events;
		vector<string> subs;
		events = mem.getDataList("Battery");
		cout << "Available Events: " << events.size() << endl;
		for (int i = 0; i < events.size(); ++i)
			cout << events[i] <<  endl;
			
		subs = mem.getSubscribers("BatteryLevelChanged");
		cout << "Available Subscribers: " << subs.size() << endl;
		for (int i = 0; i < subs.size(); ++i)
			cout << subs[i] << " with type: " << mem.getType("BatteryLevelChanged") << endl;
		*/	
		//mem.raiseEvent("BatteryLevelChanged", 5);
		//mem.insertData("BatteryLevelChanged", 90);
		
		AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
		
		value=(int)((float&)mem.getData("Device/SubDeviceList/Battery/Charge/Sensor/Value") * 100);
		sprintf(str, "%d", value);
		tts.say(str);
		cout << "value: " << value << ":" << mem.getData("BatteryLevelChanged").toString() << endl;
		
		
		AL::ALProxy pNetNao = AL::ALProxy(string("RMNetNao"), CB_IP, CB_PORT);
		sclient = pNetNao.call<int>("getClient_tcp"); 
		
		buf = buf + (char)(value);
		cout << "buf.length() = " << (int)buf.length() << endl;
		sent = pNetNao.call<int, int, AL::ALValue, int, int>(
									"sendString", sclient, buf, buf.length(), 0);
		
	}
	catch (const AL::ALError& e)
	{
		cout<< "ERROR [Executer]<sendBatteryStatus>:" << endl << e.what() << endl;
	}
}


void Executer::sendState()
{
	int sclient;
	string value;
	int sent= 0;
	string buf;
	try
	{		
		AL::ALProxy pNetNao = AL::ALProxy(string("RMNetNao"), CB_IP, CB_PORT);
		sclient = pNetNao.call<int>("getClient_tcp"); 
		
		switch (getState(STATE_ABSOLUT))
		{
			case STATE_CROUCHING: 
				buf = "ZST_CROUCHING";
				break;
			case STATE_STANDING: 
				buf = "ZST_STANDING";
				break;
			case STATE_SITTING: 
				buf = "ZST_SITTING";
				break;
			case STATE_WALKING: 
				buf = "ZST_WALKING";
				break;
			case STATE_STOPPING: 
				buf = "ZST_STOPPING";
				break;
			case STATE_MOVING: 
				buf = "ZST_MOVING";
				break;
			case STATE_UNKNOWN: 
			default:
				buf = "ZST_UNKNOWN";
				break;
		};
		sent = pNetNao.call<int, int, AL::ALValue, int, int>(
									"sendString", sclient, buf, buf.length(), 0);
		cout << "buf.length() = " << (int)buf.length() << ", gesendet: " << sent << endl;
		
	}
	catch (const AL::ALError& e)
	{
		cout<< "ERROR [Executer]<sendBatteryStatus>:" << endl << e.what() << endl;
	}	
}

void Executer::callback()
{
}

void Executer::cbPoseChanged(const string& eventName, const string& postureName, const string& subscriberIdentifier)
{
	mpose = (string&)mem.getData("robotPoseChanged");
	//cout<< ">>>POSTURE CHANGED<<<>" << "[" << postureName << "]:" << mpose << endl;
	try
	{
		//AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
		//	tts.say((mpose));
	}
	catch (const AL::ALError& e)
	{
		cout<< "Error Callback:" << e.what() << endl;
	}
}



