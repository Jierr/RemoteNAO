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
#include <time.h>


#include "executer.h"
#include "gen.h"

using namespace std;
	
	
Executer* Executer::self = 0;
Executer::Executer(boost::shared_ptr<AL::ALBroker> broker, const string& name)
: 	AL::ALModule(broker, name),
	mutex(AL::ALMutex::createALMutex()),
	sync(AL::ALMutex::createALMutex())
{
	setModuleDescription("This Module manages the Data comming from the Network remotemodule");

	
	functionName("executerRespond", getName(), "Ping the module");
	BIND_METHOD(Executer::executerRespond);	
	
	functionName("initWalk", getName(), "Initializes the posture for walking.");
	BIND_METHOD(Executer::initWalk);
	
	functionName("initSecure", getName(), "Sets NAO in resting mode.");
	BIND_METHOD(Executer::initSecure);
	
	functionName("behave_stand", getName(), "Behavior stand");
	BIND_METHOD(Executer::behave_stand);
	
	
	functionName("behave_wipe", getName(), "Behavior wipe forehead");
	BIND_METHOD(Executer::behave_wipe);
	
	functionName("behave_sit", getName(), "Behavior sit");
	BIND_METHOD(Executer::behave_sit);
	
	functionName("behave_hello", getName(), "Behavior say hello and wave");
	BIND_METHOD(Executer::behave_hello);
	
	functionName("behave_dance", getName(), "Behavior dance");
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
//	
//	functionName("process", getName(), "Execute a given event");
//	//addParam("event", "The event to be processed");
//	BIND_METHOD(Executer::process);
//	
	functionName("cbPoseChanged", getName(), "callback");
	addParam("eventName", "The event to be processed");
	addParam("postureName", "The name of posture");
	addParam("subscriberIdentifier", "ID");
	BIND_METHOD(Executer::cbPoseChanged);
}

Executer::~Executer()
{
	Executer::self = 0;
}

void Executer::init()
{
	cbstate = STATE_UNKNOWN;
	self = this;
	cbinc = false;
	cbcall = false;
	mem = AL::ALMemoryProxy(getParentBroker());
	mpose = (string&)mem.getData("robotPoseChanged");
	mem.subscribeToEvent("robotPoseChanged", "RMExecuter", "cbPoseChanged");
	
}

void Executer::initEventList(boost::shared_ptr<EventList> eL)
{
	eventList = eL;
}

void Executer::setStateMan(int* abs, int* itrans, int* gblock, bool* b, bool* pb)
{
	stateAbs = abs;
	inTransition = itrans;
	blockGen = gblock;
	block = b;
	parblock = pb;
}


int Executer::unblockfor(const int& code)
{	
	int c = 0;
	switch(code){		
		case C_UP:
			for (c = 0; c < NUM_CODES; ++c)
				block[c] = false;
			break;
		case C_REST:
			for (c = 0; c < NUM_CODES; ++c)
				block[c] = false;
			break;
		case C_SIT:
			for (c = 0; c < NUM_CODES; ++c)
				block[c] = false;
			break;
		case C_WALK:
			for (c = 0; c < NUM_CODES; ++c)
				block[c] = false;
			break;
		case C_EXE:
			for (c = 0; c < NUM_CODES; ++c)
				block[c] = false;
			break;
		case C_WAVE:
			parblock[C_DANCE] = false;
			parblock[C_UP] = false;
			parblock[C_SIT] = false;
			parblock[C_REST] = false;
			parblock[C_WAVE] = false;
			parblock[C_WIPE] = false;
			parblock[C_ARM] = false;
			parblock[C_HEAD] = false;
			parblock[C_STOPALL] = false;
			break;
		case C_WIPE:
			parblock[C_DANCE] = false;
			parblock[C_UP] = false;
			parblock[C_SIT] = false;
			parblock[C_REST] = false;
			parblock[C_WAVE] = false;
			parblock[C_WIPE] = false;
			parblock[C_ARM] = false;
			parblock[C_HEAD] = false;
			parblock[C_STOPALL] = false;
			break;
		case C_DANCE:
			for (c = 0; c < NUM_CODES; ++c)
				block[c] = false;
			break;
		case C_STOP:
			for (c = 0; c < NUM_CODES; ++c)
				block[c] = false;
			block[C_MOV] = false;
			block[C_STOP] = false;
			block[C_STOPALL] = false;
			break;
		case C_STOPALL:
			for (c = 0; c < NUM_CODES; ++c)
				block[c] = false;
			break;
		case C_MOV:
			for (c = 0; c < NUM_CODES; ++c)
				block[c] = false;
			break;
		case C_VID:
			break;
		case C_SPK:
			parblock[C_SPK] = false;
			break;
		case C_HEAD:
			parblock[C_HEAD] = false;
			break;
		case C_ARM:
			parblock[C_ARM] = false;
			parblock[C_WAVE] = false;
			parblock[C_WIPE] = false;
			break;
		case C_BAT:
			break;
		case C_STATE:
			break;
		case C_RESET:
			break;		
		default:
			for (c = 0; c < NUM_CODES; ++c)
				block[c] = false;
			break;
	};
	
	block[C_STATE] = false; 
	parblock[C_STATE] = false; 
}


void Executer::setState(state_t s)
{
	mutex->lock();
	if (cbstate == STATE_KNOWN)
	{
		//if the motion hasn't been frozen by stop
		if (((state & STATE_ABSOLUT) == STATE_STOPPED) &&
			((s & STATE_ABSOLUT) != STATE_STANDING)	  &&
			((s & STATE_ABSOLUT) != STATE_SITTING))
			state = s;
		else if ((state & STATE_ABSOLUT) != STATE_STOPPED)
			state = s;
	}
	else
		state = STATE_UNKNOWN;
	mutex->unlock();
	sendState();
}


state_t Executer::getState(int type)
{
	int result;
	mutex->lock();
	if (cbstate == STATE_KNOWN)
		result = state & type;
	else
		result = STATE_UNKNOWN;
	mutex->unlock();
	return result;
}


//int Executer::processConflicts(const Event& event)
//{
//	mutex->lock();
//	//eventList->list();
//	event_params_t ep;
//	int classf = EVT_BUSY;
//	static time_t obat = time(0);
//	time_t nbat = time(0);
//	
//	
//	eventList->setOrder(ORD_STRICT);
//	state_t stateAbs = state & STATE_ABSOLUT;
//	state_t statePar = state & STATE_PARALLEL;
//	//If currently everything is to be stopped do not queue new events
//	if (stateAbs == STATE_STOPPINGALL)
//	{
//		classf = EVT_DONE;
//		mutex->unlock();
//		return classf;
//	}
//	switch (event.ep.type)
//	{
//		case INIT_WALK:	
//			if (stateAbs != STATE_CROUCHING)
//			{
//				classf = EVT_DONE;
//			}		
//			break;
//		case INIT_REST:
//			if (stateAbs != STATE_STANDING)
//			{
//				classf = EVT_DONE;
//			}		
//			break;
//		case INIT_SIT:
//			if (stateAbs == STATE_WALKING)
//			{
//				ep.type = CODE_STOP;
//				ep.iparams[0] = MOV_STOP;
//				eventList->addFirst(ep);
//				classf = EVT_PENDINGABS;
//			}		
//			else if (stateAbs == STATE_STOPPING)
//			{
//				classf = EVT_PENDINGABS;
//				eventList->setOrder(ORD_PAR);
//			}
//			else if ((stateAbs != STATE_STANDING) &&
//					 (stateAbs != STATE_CROUCHING)&&
//					 (stateAbs != STATE_STOPPED))
//			{
//				classf = EVT_DONE;
//			}		
//			break;
//		case INIT_UP:
//			if ((stateAbs == STATE_MOVING) 	|| 
//				(stateAbs == STATE_STOPPING))
//			{
//				classf = EVT_DONE;
//			}	
//			else if((stateAbs == STATE_WALKING))
//			{
//				classf = EVT_PENDINGABS;
//			}		
//			break;
//		case INIT_WIPE:
//		case INIT_WAVE:
//			if ((stateAbs == STATE_MOVING) ||
//			    (stateAbs == STATE_STOPPED) )
//			{
//				classf = EVT_DONE;
//			}	
//			else if((stateAbs == STATE_WALKING)	|| 
//			        (stateAbs == STATE_STOPPING))
//			{
//				classf = EVT_PENDINGABS;
//			}		
//			break;
//		case INIT_DANCE:
//			if (stateAbs != STATE_STANDING)
//			{
//				classf = EVT_DONE;
//			}	
//			break;
//		case CODE_SPK:
//			break;
//		case CODE_MOV:
//			if (stateAbs == STATE_CROUCHING)
//			{
//				ep.type = INIT_WALK;
//				eventList->addFirst(ep);
//				classf = EVT_PENDINGABS;
//			}
//			else if (stateAbs == STATE_SITTING)
//			{
//				ep.type = INIT_UP;
//				eventList->addFirst(ep);
//				classf = EVT_PENDINGABS;
//			}
//			else if (stateAbs == STATE_WALKING)
//			{
//				ep.type = CODE_STOP;
//				ep.iparams[0] = MOV_STOP;
//				eventList->addFirst(ep);
//				classf = EVT_PENDINGABS;
//			}
//			else if ((stateAbs == STATE_MOVING) 	||
//					 (stateAbs == STATE_STOPPING))
//			{
//				classf = EVT_PENDINGABS;
//				eventList->setOrder(ORD_PAR);
//			}
//			else if (stateAbs != STATE_STANDING)
//			{
//				classf = EVT_DONE;
//			}
//			break;
//		case CODE_HEAD:
//			if (statePar == STATE_HEADMOVE)
//			{
//				classf = EVT_PENDINGPAR;
//			}
//			if ((stateAbs == STATE_UNKNOWN) ||
//			    (stateAbs == STATE_STOPPED))
//				classf = EVT_DONE;
//			break;
//		case CODE_ARM:
//			if (statePar == STATE_ARMMOVE)
//			{
//				classf = EVT_PENDINGPAR;
//			}
//			if ((stateAbs == STATE_UNKNOWN) ||
//			    (stateAbs == STATE_STOPPED))
//				classf = EVT_DONE;
//			break;
//		case CODE_STOP:
//			if (stateAbs != STATE_WALKING)
//				classf = EVT_DONE;
//			break;
//		case CODE_STOPALL:
//			break;
//		case CODE_BAT:
//			{
//				nbat = time(0);
//				if ((nbat - obat) > 15)
//				{	
//					ep.type = RESET_CONNECTION;
//					//eventList->addFirst(ep);
//				}
//				break;
//			}
//		case CODE_STATE:
//			break;
//		case RESET_CONNECTION:
//			if (stateAbs == STATE_WALKING)
//			{
//				cout<< "DIS: first stop Movement" << endl;
//				ep.type = CODE_STOP;
//				ep.iparams[0] = MOV_STOP;
//				eventList->addFirst(ep);
//				classf = EVT_PENDINGABS;
//			}			
//			else if (stateAbs == STATE_STANDING)
//			{
//				cout<< "DIS: Now Rest" << endl;
//				ep.type = INIT_REST;
//				eventList->addFirst(ep);
//				classf = EVT_PENDINGABS;
//			}
//			else if ((stateAbs == STATE_STOPPING) || (stateAbs == STATE_MOVING))
//				classf = EVT_PENDINGABS;
//			else if(stateAbs != STATE_CROUCHING)
//				classf = EVT_DONE;
//			break;
//		case CODE_INVALID:
//		default:
//				classf = EVT_DONE;
//			//cout<< "Nothing to do" << endl;
//			break;		
//	};
//	mutex->unlock();
//	return classf;
//}




void Executer::mark_thread_done(struct exec_arg* aargs)
{
	aargs->event = 0;
}


void* Executer::execute(void* args)
{
	struct exec_arg* aargs = (struct exec_arg*)args;
	int oldstate;
	int oldtype;
	
	if(pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype) != 0)	
		cerr<< "[Executer]<execute> pthread_setcanceltype failed" << endl;
	if(pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldstate) != 0)
		cerr<< "[Executer]<execute> pthread_setcancelstate failed" << endl;
	
	if (!self)	
		return 0;
	cout<< "[Executer]<execute> " << endl 
		<< "ID: " << aargs->id << endl
		<< "Nummer: " << aargs->tnum << endl
		<< "Event.type: " << aargs->event->ep.type << endl
		<< "Event.ip1: " << aargs->event->ep.iparams[0] << endl
		<< "Event.ip2: " << aargs->event->ep.iparams[1] << endl
		<< "Event.string: " << aargs->event->ep.sparam << endl;
	
	switch(aargs->event->ep.type)
	{
		case INIT_WALK:
			self->initWalk();
			aargs->mutex->lock();
			if (!self->cbinc)
				*self->stateAbs = ABS_STANDING;
			self->unblockfor(C_WALK);
			*self->inTransition = 0;
			aargs->mutex->unlock();
			break;
		case INIT_REST:
			self->initSecure();
			aargs->mutex->lock();
			if (!self->cbinc)
				*self->stateAbs = ABS_CROUCHING;
			self->unblockfor(C_REST);
			*self->inTransition = 0;
			aargs->mutex->unlock();
			break;
		case INIT_SIT:
			self->behave_sit();
			aargs->mutex->lock();
			*self->stateAbs = ABS_SITTING;
			self->cbinc = false;
			self->unblockfor(C_SIT);
			*self->inTransition = 0;
			aargs->mutex->unlock();
			break;
		case INIT_UP:
			self->behave_stand();
			aargs->mutex->lock();
			*self->stateAbs = ABS_STANDING;
			self->cbinc = false;
			self->unblockfor(C_UP);
			*self->inTransition = 0;
			aargs->mutex->unlock();
			break;
		case INIT_WAVE:
			self->behave_hello();
			aargs->mutex->lock();
			self->unblockfor(C_WAVE);
			*self->inTransition = 0;
			aargs->mutex->unlock();
			break;
		case INIT_DANCE:
			self->behave_dance();
			aargs->mutex->lock();
			if (!self->cbinc)
				*self->stateAbs = ABS_STANDING;
			self->unblockfor(C_DANCE);
			*self->inTransition = 0;
			aargs->mutex->unlock();
			break;
		case INIT_WIPE:
			self->behave_wipe();
			aargs->mutex->lock();
			self->unblockfor(C_WIPE);
			aargs->mutex->unlock();
			break;
		case CODE_MOV:		
			cout<< "[Executer]<execute>: MOV" << endl;
			aargs->mutex->lock();
			if (!aargs->eventList->reduceLastWalking())
			{
				if (!self->cbinc)
				{
					*self->stateAbs = ABS_WALKING;
					aargs->mutex->unlock();
					self->walk(*aargs->event);
					aargs->mutex->lock();
					if ((!self->cbinc) && (*self->stateAbs == ABS_WALKING))
						*self->stateAbs = ABS_STANDING;
					self->unblockfor(C_MOV);
					*self->inTransition = 0;
					aargs->mutex->unlock();
				}
				else
				{
					self->unblockfor(C_MOV);
					*self->inTransition = 0;
					aargs->mutex->unlock();					
				}
			}
			else
			{
				self->unblockfor(C_MOV);
				*self->inTransition = 0;
				aargs->mutex->unlock();					
			}
					
			break;
		case CODE_STOP:
			cout<< "[Executer]<execute>: STOP" << endl;
			aargs->mutex->lock();
			aargs->eventList->reduceLastWalking();
			self->walk(*aargs->event);
//			if (!self->cbinc)
//				*self->stateAbs = ABS_STANDING;
			self->unblockfor(C_STOP);
			aargs->mutex->unlock();
			break;
		case CODE_STOPALL:
			aargs->mutex->lock();
			aargs->eventList->removePending();
			if (*self->stateAbs == ABS_WALKING)
			{
				self->walk(*aargs->event);
				*self->stateAbs = ABS_STANDING; 
			}
			self->killBehaviors();
			self->unblockfor(C_STOPALL);
			*self->inTransition = 0;
			aargs->mutex->unlock();
			break;
		case CODE_SPK:
			self->speak(aargs->event->ep.sparam);
			aargs->mutex->lock();
			self->unblockfor(C_SPK);
			aargs->mutex->unlock();
			break;
		case CODE_ARM:
			self->moveArm(*aargs->event);
			aargs->mutex->lock();
			self->unblockfor(C_ARM);
			aargs->mutex->unlock();
			break;
		case CODE_HEAD:
			self->moveHead(*aargs->event);
			aargs->mutex->lock();
			self->unblockfor(C_HEAD);
			aargs->mutex->unlock();
			break;
		case CODE_EXE:
			self->behave_gen(aargs->event->ep.sparam);
			aargs->mutex->lock();
			*self->stateAbs = ABS_UNKNOWN;
			self->unblockfor(C_EXE);
			*self->inTransition = 0;
			aargs->mutex->unlock();
			break;
			
		default:
			cout<< "[Executer]<execute>Not specified Comand executed" << endl;
			break;
	};
	
	self->sendState();
	
	aargs->eventList->setClassf(aargs->event->id, EVT_DONE);	
	Executer::mark_thread_done(aargs);	
	return 0;
}

//void Executer::process()
//{
//	//cout<<"----->Process Event" << endl;
//	int classf;	
//	//cout << "PROCESS pevent = " << pevent.toString() << endl;
//	//Event* event = & (eventList->withID((const void*)(*(const void**)(pevent.getObject()))));
//	//const void* const id = pevent;
//	//Event event(eventList->withID(id));
//	
//	mutex->lock();
//	Event* event = eventList->getPending();
//	if (!event)
//	{
//		mutex->unlock();
//		return;
//	}
//	eventList->setClassf(event->id, EVT_BUSY);
//	mutex->unlock();
//	//cout<< "[PROCESS] ID: " << event->id << ", TYPE: " << event->ep.type << endl;	
//	classf = processConflicts(*event);
//	eventList->setClassf(event->id, classf);
//	if (classf == EVT_BUSY)
//	{
//		switch (event->ep.type)
//		{
//			case INIT_WALK:
//				setState(getState(STATE_PARALLEL) | STATE_MOVING);
//				initWalk();
//				setState(getState(STATE_PARALLEL) | STATE_STANDING);
//				//cout << "Done initWalk()" << endl;
//				break;
//			case INIT_REST:
//				setState(getState(STATE_PARALLEL) | STATE_MOVING);
//				initSecure();
//				setState(getState(STATE_PARALLEL) | STATE_CROUCHING);
//				break;
//			case INIT_SIT:
//				{
//					setState(getState(STATE_PARALLEL) | STATE_MOVING);
//					AL::ALProxy pexec(AL::ALProxy(string("RMExecuter"), AL::ALProxy::FORCED_LOCAL, 0));
//					pexec.pCall("behave_sit");
//					qi::os::msleep(10000);
//					AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
//					pbehav.stopBehavior("sit");
//					setState(getState(STATE_PARALLEL) | STATE_SITTING);
//					break;
//				}
//			case INIT_UP:
//				{
//					int stateAbs = getState(STATE_ABSOLUT); 
//					setState(getState(STATE_PARALLEL) | STATE_MOVING);
//					AL::ALProxy pexec(AL::ALProxy(string("RMExecuter"), AL::ALProxy::FORCED_LOCAL, 0));
//					pexec.pCall("behave_stand");
//					if ((stateAbs == STATE_CROUCHING) || (stateAbs == STATE_STANDING))
//						qi::os::msleep(3000);
//					else if (stateAbs != STATE_STANDING)
//						qi::os::msleep(12000);
//					

//						AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
//						//while (pbehav.isBehaviorRunning("stand"));
//							//qi::os::msleep(50);
//						pbehav.stopBehavior("stand");
//					setState(getState(STATE_PARALLEL) | STATE_STANDING);
//					break;
//				}
//			case INIT_WAVE:
//				{
//					int stateAbs = getState(STATE_ABSOLUT);
//					setState(getState(STATE_PARALLEL) | STATE_MOVING);
//					AL::ALProxy pexec(AL::ALProxy(string("RMExecuter"), AL::ALProxy::FORCED_LOCAL, 0));
//					pexec.pCall("behave_hello");
//					qi::os::msleep(5000);
//					AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
//					pbehav.stopBehavior("hello");
//					if (getState(STATE_ABSOLUT) == STATE_STOPPED)
//					{
//						mutex->lock();
//						state = getState(STATE_PARALLEL) | stateAbs;
//						mutex->unlock();
//						setState(getState());
//					}
//					else
//						setState(getState(STATE_PARALLEL) | stateAbs);
//					break;
//				}
//			case INIT_WIPE:
//				{
//					int stateAbs = getState(STATE_ABSOLUT);  
//					setState(getState(STATE_PARALLEL) | STATE_MOVING);
//					AL::ALProxy pexec(AL::ALProxy(string("RMExecuter"), AL::ALProxy::FORCED_LOCAL, 0));
//					pexec.pCall("behave_wipe");
//					qi::os::msleep(5000);
//					AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
//					pbehav.stopBehavior("wipe");
//					if (getState(STATE_ABSOLUT) == STATE_STOPPED)
//					{
//						mutex->lock();
//						state = stateAbs;
//						mutex->unlock();
//						setState(getState());
//					}
//					else
//						setState(getState(STATE_PARALLEL) | stateAbs);
//					break;
//				}
//			case INIT_DANCE:
//				{
//					
//					setState(getState(STATE_PARALLEL) | STATE_MOVING);
//					AL::ALProxy pexec(AL::ALProxy(string("RMExecuter"), AL::ALProxy::FORCED_LOCAL, 0));
//					pexec.pCall("behave_dance");
//					qi::os::sleep(50);
//					AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
//					pbehav.stopBehavior("dance");
//					setState(getState(STATE_PARALLEL) | STATE_STANDING);
//					break;
//				}
//			case CODE_SPK:
//				speak(event->ep.sparam);
//				break;
//			case CODE_MOV:
//				setState(getState(STATE_PARALLEL) | STATE_WALKING);
//				walk(*event);
//				sync->lock();
//				setState(getState(STATE_PARALLEL) | STATE_STANDING);
//				sync->unlock();
//				break;
//			case CODE_HEAD:
//				setState(getState(STATE_ALL) | STATE_HEADMOVE);
//				moveHead(*event);
//				setState(getState(STATE_ALL) & ~STATE_HEADMOVE);
//				break;
//			case CODE_ARM:
//				setState(getState(STATE_ALL) | STATE_ARMMOVE);
//				moveArm(*event);
//				setState(getState(STATE_ALL) & ~STATE_ARMMOVE);
//				break;
//			//TODO: Stopping can trigger while walking before acutally walking
//			//		Stop would be done before the walking started --> 
//			//  	Stop would have no actual result
//			case CODE_STOP:
//			{
//				//event.ep.iparams[0] = MOV_STOP;
//				sync->lock();
//				setState(getState(STATE_PARALLEL) | STATE_STOPPING);
//				walk(*event);
//				setState(getState(STATE_PARALLEL) | STATE_STANDING);
//				sync->unlock();
//				cout<< "STP: NOW STANDING" << endl;
//				break;
//			}
//			case CODE_STOPALL:
//			{
//				//save old state the Robo is in.
//				int stateAbs = getState(STATE_ABSOLUT);
//				setState(STATE_STOPPINGALL);
//				AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
//				//tts.say("Stop1");
//				mutex->lock();
//					//tts.say("Stop2");
//					eventList->removePending();
//					killBehaviors();
//					//stop walking --> stop controlled
//					if((stateAbs != STATE_STOPPING) && 
//					   (stateAbs != STATE_MOVING)   &&
//					   (stateAbs != STATE_STOPPED))
//					{
//						event_params_t ep;
//						ep.type = CODE_STOP;
//						ep.iparams[0] = MOV_STOP;
//						Event ev;
//						ev.ep = ep;
//						walk(ev);
//						stateAbs = STATE_STANDING;
//					}
//					else if (stateAbs == STATE_MOVING)
//						stateAbs = STATE_STOPPED;
//					qi::os::msleep(10);
//					killRemainingTasks();
//				mutex->unlock();	
//				setState(stateAbs);
//				//tts.say("DONE");
//							
//				break;
//			}
//			case CODE_BAT:			
//				sendBatteryStatus();
//				break;
//			case CODE_STATE:			
//				sendState();
//				break;
//			case RESET_CONNECTION:
//				//setState(getState(STATE_PARALLEL) | STATE_CROUCHING);
//				break;
//			case CODE_INVALID:
//				//cout<< "Nothing to do" << endl;
//				break;
//			default:
//			{
//				try
//				{
//					AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
//					tts.say("Unknown command!");
//				}	
//				catch(const AL::ALError& e)
//				{
//					cout<< "Error [Executer]<process(event)>: " << endl << e.what() << endl;
//				}
//				break;
//			}
//		};
//				
//		eventList->setClassf(event->id, EVT_DONE);	
//	}
//}

void Executer::executerRespond()
{
	AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
			tts.say(string("Executer"));
}

void Executer::initWalk()
{
	try
	{
		float stiffnesses  = 1.0f;
		string snames = "Body";
		
		
		AL::ALMotionProxy motion(MB_IP, MB_PORT);
		//boost::shared_ptr<AL::ALMotionProxy> motion = getParentBroker()->getMotionProxy(); 
        motion.stiffnessInterpolation(snames, stiffnesses, 1.0);
		motion.walkInit(); 
	}
	catch(const AL::ALError& e)
	{
		cout<< "Error initWalk: " << e.what() << endl; 
	}
}



void Executer::killBehaviors()
{
	try
	{
		AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
		pbehav.stopAllBehaviors();
	}
	catch(const AL::ALError& e)
	{
		cout<< "Error [Executer]<killBehaviors>: " << endl << e.what() << endl;
	}
}



void Executer::killRemainingTasks()
{
}

void Executer::behave_gen(const string& com)
{
	try
	{
		AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
		pbehav.preloadBehavior(com);
		pbehav.runBehavior(com);
		//tts.say("Behaviour done");
	}
	catch(const AL::ALError& e)
	{
		cout<< "Error [Executer]<gen>: " << endl << e.what() << endl;
	}
}

void Executer::behave_stand()
{
	try
	{
		AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
		pbehav.preloadBehavior("stand");
		pbehav.runBehavior("stand");
		//tts.say("Behaviour done");
	}
	catch(const AL::ALError& e)
	{
		cout<< "Error [Executer]<stand>: " << endl << e.what() << endl;
	}
}
		
		
void Executer::behave_sit()
{
	try
	{
		AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
		AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
		pbehav.preloadBehavior("sit");
		pbehav.runBehavior("sit");
		//tts.say("Behaviour done");
	}
	catch(const AL::ALError& e)
	{
		cout<< "Error [Executer]<sit>: " << endl << e.what() << endl;
	}

}		
		
void Executer::behave_wipe()
{
	try
	{
		AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
		AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
		pbehav.preloadBehavior("wipe");
		pbehav.runBehavior("wipe");
		//tts.say("Behaviour done");
	}
	catch(const AL::ALError& e)
	{
		cout<< "Error [Executer]<wipe>: " << endl << e.what() << endl;
	}

}		
		
void Executer::behave_hello()
{
	try
	{
		AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
		
		AL::ALProxy tts(string("ALTextToSpeech"), AL::ALProxy::FORCED_LOCAL, 0);
		tts.pCall<string>("say", "Hi there, my name is NAO");
		//tts.say("Stop1");
		pbehav.preloadBehavior("hello");
		pbehav.runBehavior("hello");
		//tts.say("Behaviour done");
	}
	catch(const AL::ALError& e)
	{
		cout<< "Error [Executer]<hello>: " << endl << e.what() << endl;
	}

}		
		
void Executer::behave_dance()
{
	try
	{
		AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
		AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
		AL::ALValue behav = pbehav.getDefaultBehaviors();
		pbehav.preloadBehavior("dance");
		pbehav.runBehavior("dance");
		//tts.say("Behaviour done");
	}
	catch(const AL::ALError& e)
	{
		cout<< "Error [Executer]<dance>: " << endl << e.what() << endl;
	}

}

void Executer::initSecure()
{	
	try
	{
		AL::ALValue roboConfig;
		AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
		AL::ALMotionProxy motion(MB_IP, MB_PORT);
		float stiffnesses  = 1.0f; 
		string snames = "Body";
        motion.stiffnessInterpolation("Body", 1.0, 1.0);
        AL::ALValue names = "Body";
        float ftargetAngles[] = {
        						 	 0.0, 0.0,
		    						 80.0, 20.0, -80.0, -60.0, 0.0, 0.0,
		    						 0.0, 0.0, -150.0/2 + 25.0, 150.0, -150.0/2, -0.0,
		    						 0.0, -0.0, -150.0/2 + 25.0, 150.0, -150.0/2, 0.0,
		    						 80.0, -20.0, +80.0, +60.0, 0.0, 0.0
        						};
        for (int i = 0; i < sizeof(ftargetAngles)/sizeof(float); ++i)
        	ftargetAngles[i]*= RAD;

       	std::vector<float> vtargetAngles(ftargetAngles,ftargetAngles + sizeof(ftargetAngles)/ sizeof(float));
        AL::ALValue targetAngles(vtargetAngles);
        float maxSpeedFraction = 0.2;
        qi::os::msleep(100);
        
		float x(1.0), y(0.0), t(0.0); 
        motion.angleInterpolationWithSpeed(names, targetAngles, maxSpeedFraction);
		
		stiffnesses  = 0.0f; 
        motion.stiffnessInterpolation("Body", 0.0, 1.2);
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
		std::vector<float> stiffnesses = motion.getStiffnesses("Body");
		if ((stiffnesses.capacity() > 0) && (stiffnesses[0] < 0.9))
       		motion.stiffnessInterpolation("Body", 1.0, 1.0);
		//motion.walkInit();
		
		switch (mode)
		{
			case MOV_FORWARD:
			{
				
				while (motion.walkIsActive())			
					qi::os::msleep(20);
				/*
				vector<string> legs (50, "");
				vector<float> footstep(3, 0.0);
				footstep[0] = 0.2;
				footstep[1] = 0.0;
				footstep[2] = 0.0;
				
				AL::ALValue footsteps;// = AL::ALValue::array(footstep, footstep);
				//footsteps.arraySetSize(50);
				for (int i=0; i<50; ++i)
					footsteps.arrayPush(footstep);
				//footsteps.arrayReserve(sizeof(vector<int>)*2);
				//footsteps.array< vector<int>, vector<int> >(footstep, footstep);
				//cout<< footsteps[0][0] << ", " << footsteps[0][1] << ", "<< footsteps[0][2] << ", " << endl
				//	<< footsteps[1][0] << ", " << footsteps[1][1] << ", "<< footsteps[1][2] << endl;
				vector<float> speed(50, 0.6); 				
				cout << "------> Move Forward <------" << endl;
				for (int i = 0; i < 50; i+=2)
					legs[i] = "LLeg";
				for (int i = 1; i < 50; i+=2)
					legs[i] = "RLeg";
				//last argument determines if existing footsteps planned should 
				//be cleared
				motion.setFootStepsWithSpeed(legs, footsteps, speed, true); */
				motion.walkTo(10,0,0);
				
				break;
			}
			case MOV_BACKWARD:
			{	
				while (motion.walkIsActive())			
					qi::os::msleep(20);
				/*			
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
				motion.setFootStepsWithSpeed(legs, footsteps, speed, true); */
				motion.walkTo(-10,0,0);
				break;
			}
			case MOV_LEFT:		
			{		
				while (motion.walkIsActive())			
					qi::os::msleep(20);
				/*vector<string> legs (TURN_STEPS, "");
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
				motion.setFootStepsWithSpeed(legs, footsteps, speed, true); */
				motion.walkTo(0,0,TURN_ANGLE * RAD);
				
				break;
			}
			case MOV_RIGHT:
			{		
				while (motion.walkIsActive())			
					qi::os::msleep(20);
				/*vector<string> legs (TURN_STEPS, "");
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
				motion.setFootStepsWithSpeed(legs, footsteps, speed, true); */
				
				motion.walkTo(0,0,-TURN_ANGLE * RAD);
				break;
			}
			case MOV_STOP:
			default:
			{
				if(motion.walkIsActive())
				{
					motion.stopWalk();
				}
				else //if not walking don't do anything
					return;
					
//				vector<string> legs (2, "");
//				vector<float> footstep(3, 0.0);
//				footstep[0] = 0.0;
//				footstep[1] = 0.0;
//				footstep[2] = 0.0;	
//				
//				AL::ALValue footsteps;	
//				for (int i=0; i<2; ++i)
//					footsteps.arrayPush(footstep);
//				vector<float> speed(2, 1.0);
//				legs[0] = "LLeg"; 
//				legs[1] = "RLeg"; 		
//				//set the foots parallel			
//				motion.setFootStepsWithSpeed(legs, footsteps, speed, true);
				
				cout << "------> Move STOP <------" << endl;
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
				cout<< ">>>MOVE HEAD FORWARD<<<" << endl;       
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
				cout<< ">>>MOVE HEAD BACKWARD<<<" << endl;       
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
	
		value=(int)((float&)mem.getData("Device/SubDeviceList/Battery/Charge/Sensor/Value") * 100);
			
		AL::ALProxy pNetNao = AL::ALProxy(string("RMNetNao"), CB_IP, CB_PORT);
		sclient = pNetNao.call<int>("getClient_tcp"); 
		
		buf = buf + (char)(value);
		cout << "[Executer:sendBatteryStatus]buf.length() = " << (int)buf.length() << endl;

		while (sent < buf.length())
		{
			sent += pNetNao.call<int, int, AL::ALValue, int, int>(
										"sendString", sclient, buf, buf.length(), 0);
		}
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
		
		switch (*stateAbs)
		{
			case ABS_CROUCHING: 
				buf = "ZST_CROUCHING";
				break;
			case ABS_STANDING: 
				buf = "ZST_STANDING";
				break;
			case ABS_SITTING: 
				buf = "ZST_SITTING";
				break;
			case ABS_WALKING: 
				buf = "ZST_WALKING";
				break;
			case ABS_UNKNOWN: 
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
		cout<< "ERROR [Executer]<sendState>:" << endl << e.what() << endl;
	}	
}

void Executer::cbPoseChanged(const string& eventName, const string& postureName, const string& subscriberIdentifier)
{
	mpose = (string&)mem.getData("robotPoseChanged");
	//cout<< ">>>POSTURE CHANGED<<<>" << "[" << postureName << "]:" << mpose << endl;
	mutex->lock();
	cbcall = true;
	try
	{
		//AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
		//	tts.say((mpose));
		if ((mpose.compare("Unknown") == 0) ||
			(mpose.compare("Back") == 0) ||
			(mpose.compare("Belly") == 0) ||
			(mpose.compare("HeadBack") == 0) ||
			(mpose.compare("Left") == 0) ||
			(mpose.compare("Right") == 0))
		{
			cbstate=STATE_UNKNOWN;
		}
		else
			cbstate=STATE_KNOWN;
	}
	catch (const AL::ALError& e)
	{
		cout<< "Error Callback:" << e.what() << endl;
	}
	mutex->unlock();
}



