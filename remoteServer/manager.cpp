#include <iostream>
#include <vector>
#include <alvalue/alvalue.h>
#include <althread/almutex.h>
#include <althread/alcriticalsection.h>
#include <alproxies/almemoryproxy.h>
#include <alproxies/altexttospeechproxy.h>
#include <alproxies/albehaviormanagerproxy.h>
#include <alcommon/alproxy.h>
#include <qi/os.hpp>
#include <alproxies/alrobotposeproxy.h>

#include <stdio.h>

#include "manager.h"
#include "executer.h"
#include "gen.h"
#include "eventlist.h"


using namespace std;
	
Manager::Manager(boost::shared_ptr<AL::ALBroker> broker, const string& name)
:	AL::ALModule(broker, name), 
	mutex(AL::ALMutex::createALMutex()),
	accessExec(broker),
	eventList(new EventList),
	blist(new Behavelist)
{
	setModuleDescription("This Module manages the Data comming from the remoteServer module");
	
	functionName("localRespond", getName(), "local Respond");
	BIND_METHOD(Manager::localRespond);
	
	functionName("runExecuter", getName(), "run Executer");
	BIND_METHOD(Manager::runExecuter);	
	
//	functionName("decode", getName(), "decode the argument given");
//	addParam("toParse", "const string&: the symbol to be decoded");
//	setReturn("int", "sucess: return pos parsed, failure: return 0");
//	BIND_METHOD(Manager::decode);
		
	functionName("initPipe", getName(), "initialize Pipe to communicate with cam module");
	addParam("writer", "filedescriptor for the writing end");
	BIND_METHOD(Manager::initPipe)	
		
	functionName("initIp4", getName(), "Init the ip for manager");
	addParam("ip4", "ip4 string");
	BIND_METHOD(Manager::initIp4)	
		
	functionName("addCom", getName(), "add a Event to be executed");
	addParam("type", "");
	addParam("ip1", "");
	addParam("ip2", "");
	addParam("fp", "");
	addParam("sp", "");
	addParam("prio", "");
	BIND_METHOD(Manager::addCom)
	
	functionName("setCB", getName(), "Init the Controle Broker IP and Port");
	addParam("bip", "");
	addParam("bport", "");
	BIND_METHOD(Manager::setCB)
	
	mem = AL::ALMemoryProxy(broker);
	
	accessExec.exec->initEventList(eventList);
	accessExec.exec->initBehavelist(blist);
}

Manager::~Manager()
{
	accessExec.exec->exit();
}




void Manager::init()
{	
	threadcount = 0;
	stateAbs = ABS_UNKNOWN;
	stateDisrupt = false;
	inTransition = 0;
	blockGen = 0;
	cbip = "127.0.0.1";
	cbport = 0;
	for (int c = 0; c < NUM_CODES; ++c)
		block[c] = false;
	for (int c = 0; c < NUM_CODES; ++c)
		parblock[c] = false;
	initblist();
	initAbsTransition();
	initGenAllowed();
	cout<< "[Executer] Liste aller Events" << endl << blist->list() << endl;
}


void Manager::setCB(const string& bip, const int& bport)
{
	cbip = bip;
	cbport = (unsigned short)bport;
	accessExec.exec->cbip = cbip;
	accessExec.exec->cbport = cbport;
}

void Manager::initblist()
{
	int size = 0;
	int b = 0;
	int dots = 0;
	int dfirst = 0;
	int dlast = 0;
	bool found = false;
	string name = "";
	string full = "";
	int fstate = -1;
	int lstate = -1;
	
	string curr = "";
	try
	{
		AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
		AL::ALValue behav;
		behav = pbehav.getInstalledBehaviors();
		size = behav.getSize();
		
		while (b < size)
		{
			curr = (string&)behav[b];
			dots = 0;
			//cout<< "Bearbeite Behaviour: " << curr << endl;
			for (int c = 0; c < curr.length(); ++c)
				if (curr[c] == '.')
				{
					++dots;
					if (dots == 1)
						dfirst = c;
					else if (dots == 2)
						dlast = c;
				}

			if (dots>= 2)
			{
				if(curr.compare(0, dfirst, "none") == 0)
					fstate = -1;
				else if(curr.compare(0, dfirst, "standing") == 0)
					fstate = ABS_STANDING;
				else if(curr.compare(0, dfirst, "sitting") == 0)
					fstate = ABS_SITTING;
				else if(curr.compare(0, dfirst, "walking") == 0)
					fstate = ABS_WALKING;
				else if(curr.compare(0, dfirst, "lieing") == 0)
					fstate = ABS_LIEING;
				else if(curr.compare(0, dfirst, "unknown") == 0)
					fstate = ABS_UNKNOWN;
				else
					fstate = ABS_STANDING;
					
				if(curr.compare(dfirst+1, dlast-1 - dfirst, "none") == 0)
					lstate = -1;
				else if(curr.compare(dfirst+1, dlast-1 - dfirst, "standing") == 0)
					lstate = ABS_STANDING;
				else if(curr.compare(dfirst+1, dlast-1 - dfirst, "sitting") == 0)
					lstate = ABS_SITTING;
				else if(curr.compare(dfirst+1, dlast-1 - dfirst, "walking") == 0)
					lstate = ABS_WALKING;
				else if(curr.compare(dfirst+1, dlast-1 - dfirst, "lieing") == 0)
					lstate = ABS_LIEING;
				else
					lstate = ABS_UNKNOWN;
				if (dlast+1 < curr.length())	
					name = curr.substr(dlast+1, curr.length() - (dlast+1));
				else 
					name = "<name missing>";
		
			}
			else 
			{
				fstate = ABS_STANDING;
				lstate = ABS_UNKNOWN;
				name = curr;				
			}
			full = curr;
//			cout<< "Fuege neu gefundenen Behaviour zur Liste hinzu:" << endl
//				<< ">Name " << name << endl;
			blist->addBehave(fstate, lstate, name, full);
			++b;
		}
//		cout<< "Alle Behaviours bearbeited" << endl;
	}
	catch(const AL::ALError& e)
	{
		cout<< "Error [Executer]<gen>: " << endl << e.what() << endl;
	}
}




bool Manager::isblocked(const int& code)
{
	if ((code < 0) && (code >= NUM_CODES))
		return true;
	
	return parblock[code] || block[code];
}

int Manager::blockfor(const int& code)
{	
	int c = 0;
	switch(code){		
		case C_UP:
			for (c = 0; c < NUM_CODES; ++c)
				block[c] = true;
			block[C_STOPALL] = false;
			block[C_RESET] = false;
			block[C_SPK] = false;
			break;
		case C_REST:
			for (c = 0; c < NUM_CODES; ++c)
				block[c] = true;
			block[C_STOPALL] = false;
			block[C_RESET] = false;
			block[C_SPK] = false;
			break;
		case C_SIT:
			for (c = 0; c < NUM_CODES; ++c)
				block[c] = true;
			block[C_STOPALL] = false;
			block[C_RESET] = false;
			block[C_SPK] = false;
			break;
		case C_WALK:
			for (c = 0; c < NUM_CODES; ++c)
				block[c] = true;
			block[C_STOPALL] = false;
			block[C_RESET] = false;
			block[C_SPK] = false;
			break;
		case C_EXE:
			for (c = 0; c < NUM_CODES; ++c)
				block[c] = true;
			block[C_STOPALL] = false;
			block[C_RESET] = false;
			block[C_SPK] = false;
			break;
		case C_EXE_PAR:
			block[C_STOPALL] = false;
			block[C_RESET] = false;
			break;
		case C_WAVE:
			parblock[C_DANCE] = true;
			parblock[C_UP] = true;
			parblock[C_SIT] = true;
			parblock[C_REST] = true;
			parblock[C_WAVE] = true;
			parblock[C_WIPE] = true;
			parblock[C_ARM] = true;
			parblock[C_HEAD] = true;
			parblock[C_STOPALL] = false;
			break;
		case C_WIPE:
			parblock[C_DANCE] = true;
			parblock[C_UP] = true;
			parblock[C_SIT] = true;
			parblock[C_REST] = true;
			parblock[C_WAVE] = true;
			parblock[C_WIPE] = true;
			parblock[C_ARM] = true;
			parblock[C_HEAD] = true;
			parblock[C_STOPALL] = false;
			break;
		case C_DANCE:
			for (c = 0; c < NUM_CODES; ++c)
				block[c] = true;
			block[C_STOPALL] = false;
			block[C_RESET] = false;
			block[C_SPK] = false;
			break;
		case C_STOP:
			for (c = 0; c < NUM_CODES; ++c)
				block[c] = true;
			block[C_STOPALL] = false;
			block[C_RESET] = false;
			block[C_WIPE] = false;
			block[C_WAVE] = false;
			block[C_ARM] = false;
			block[C_HEAD] = false;
			block[C_SPK] = false;
			break;
		case C_STOPALL:
			for (c = 0; c < NUM_CODES; ++c)
				block[c] = true;
			break;
		case C_MOV:
			for (c = 0; c < NUM_CODES; ++c)
				block[c] = true;
			block[C_MOV] = false;
			block[C_STOP] = false;
			block[C_WIPE] = false;
			block[C_WAVE] = false;
			block[C_ARM] = false;
			block[C_HEAD] = false;
			block[C_SPK] = false;
			block[C_STOPALL] = false;
			block[C_RESET] = false;
			break;
		case C_VID:
			break;
		case C_SPK:
			parblock[C_SPK] = true;
			break;
		case C_HEAD:
			parblock[C_HEAD] = true;
			break;
		case C_ARM:
			parblock[C_ARM] = true;
			parblock[C_WAVE] = true;
			parblock[C_WIPE] = true;
			break;
		case C_BAT:
			break;
		case C_STATE:
			break;
		case C_RESET:
			for (c = 0; c < NUM_CODES; ++c)
				block[c] = true;
			block[C_STOPALL] = false;
			break;
		default:
			for (c = 0; c < NUM_CODES; ++c)
				block[c] = true;
			break;
	};
	
	block[C_RESET] = false;
	parblock[C_RESET] = false;
	
}


bool Manager::isblocked(Event* event)
{

	switch(event->ep.type)
	{
		case INIT_UP:
			return isblocked(C_UP);
			break;
		case INIT_REST:
			return isblocked(C_REST);
			break;
		case INIT_SIT:
			return isblocked(C_SIT);
			break;
		case INIT_WALK:
			return isblocked(C_WALK);
			break;
		case INIT_WAVE:
			return isblocked(C_WAVE);
			break;
		case INIT_WIPE:
			return isblocked(C_WIPE);
			break;
		case INIT_DANCE:
			return isblocked(C_DANCE);
			break;
		case CODE_STOP:
			return isblocked(C_STOP);
			break;
		case CODE_STOPALL:
			return isblocked(C_STOPALL);			
			break;
		case CODE_MOV:
			return isblocked(C_MOV);
			break; 
		case CODE_HEAD:
			return isblocked(C_HEAD);
			break; 
		case CODE_ARM:
			return isblocked(C_ARM);
			break; 
		case CODE_SPK:
			return isblocked(C_SPK);
			break; 
		case RESET_CONNECTION:
			return isblocked(C_RESET);
			break; 
		case CODE_STATE:
			return isblocked(C_STATE);
			break;
		case CODE_EXE:
			return isblocked(C_EXE);
			break;
		default:
			return true;
			break;
	}
}


void Manager::initAbsTransition()
{
	int size = 0;
	behave_t* curr;
	event_params_t invalid = EP_DEFAULT(CODE_INVALID);
	event_params_t valid = EP_DEFAULT(CODE_VALID);
	for(int from = 0; from<NUM_ABS_STATES; ++from)
		for(int to = 0; to<NUM_ABS_STATES; ++to)
			absTransition[from][to] = invalid;
			
	absTransition[ABS_STANDING][ABS_STANDING] = invalid;
	valid.type = INIT_SIT;
	absTransition[ABS_STANDING][ABS_SITTING] = valid;
	valid.type = INIT_REST;
	absTransition[ABS_STANDING][ABS_CROUCHING] = valid;
	valid.type = CODE_INVALID;
	absTransition[ABS_STANDING][ABS_LIEING] = invalid;
	valid.type = CODE_VALID;
	absTransition[ABS_STANDING][ABS_WALKING] = valid;
	//generic behaviours have no predefined state so its end state is ABS_UNKNOWN
	//which needs to be resolved
	valid.type = CODE_VALID;
	absTransition[ABS_STANDING][ABS_UNKNOWN] = valid;

	
	
	valid.type = INIT_UP;
	absTransition[ABS_SITTING][ABS_STANDING] = valid;
	absTransition[ABS_SITTING][ABS_SITTING] = invalid;
	absTransition[ABS_SITTING][ABS_CROUCHING] = invalid;
	valid.type = CODE_INVALID;
	absTransition[ABS_SITTING][ABS_LIEING] = invalid;
	absTransition[ABS_SITTING][ABS_WALKING] = invalid;
	absTransition[ABS_SITTING][ABS_UNKNOWN] = invalid;	
	
	
	valid.type = INIT_UP;
	absTransition[ABS_CROUCHING][ABS_STANDING] = valid;
	valid.type = INIT_SIT;
	absTransition[ABS_CROUCHING][ABS_SITTING] = valid;
	absTransition[ABS_CROUCHING][ABS_CROUCHING] = invalid;
	valid.type = CODE_INVALID;
	absTransition[ABS_CROUCHING][ABS_LIEING] = invalid;
	valid.type = CODE_VALID;
	absTransition[ABS_CROUCHING][ABS_WALKING] = valid;
	absTransition[ABS_CROUCHING][ABS_UNKNOWN] = invalid;	
	
	
	valid.type = INIT_UP;
	absTransition[ABS_LIEING][ABS_STANDING] = valid;
	valid.type = INIT_SIT;
	absTransition[ABS_LIEING][ABS_SITTING] = valid;
	absTransition[ABS_LIEING][ABS_CROUCHING] = invalid;
	absTransition[ABS_LIEING][ABS_LIEING] = invalid;
	absTransition[ABS_LIEING][ABS_WALKING] = invalid;
	absTransition[ABS_LIEING][ABS_UNKNOWN] = invalid;
	
	
	valid.type = CODE_STOP;
	valid.iparams[0] = MOV_STOP;
	absTransition[ABS_WALKING][ABS_STANDING] = valid;
	absTransition[ABS_WALKING][ABS_SITTING] = invalid;
	absTransition[ABS_WALKING][ABS_CROUCHING] = invalid;
	absTransition[ABS_WALKING][ABS_LIEING] = invalid;
	absTransition[ABS_WALKING][ABS_WALKING] = invalid;
	absTransition[ABS_WALKING][ABS_UNKNOWN] = invalid;	
	valid.iparams[0] = 0;
		
	valid.type = INIT_UP;
	absTransition[ABS_UNKNOWN][ABS_STANDING] = valid;
	valid.type = INIT_SIT;
	absTransition[ABS_UNKNOWN][ABS_SITTING] = valid;
	absTransition[ABS_UNKNOWN][ABS_CROUCHING] = invalid;
	valid.type = CODE_INVALID;
	absTransition[ABS_UNKNOWN][ABS_LIEING] = invalid;
	absTransition[ABS_UNKNOWN][ABS_WALKING] = invalid;
	absTransition[ABS_UNKNOWN][ABS_UNKNOWN] = invalid;	
	
	//adapt this state machine now with all generic transitions
	
	size = blist->getSize();
	for (int nr = 0; nr < size; ++nr)
	{
		curr = blist->getwNr(nr);
		if ((curr->fstate>=0) && (curr->lstate>=0) && 
			(curr->fstate<NUM_ABS_STATES) && (curr->lstate<NUM_ABS_STATES))
		{
			if (absTransition[curr->fstate][curr->lstate].type == CODE_INVALID)
			{
				absTransition[curr->fstate][curr->lstate].type = CODE_EXE;
				absTransition[curr->fstate][curr->lstate].sparam = curr->name;
			}
		}
	}
}

void Manager::initGenAllowed()
{
	for (int g = 0; g < NUM_GEN_STATES; ++g)
		for(int a = 0; a < NUM_ABS_STATES; ++a)
			genAllowed[g][a] = 0;
			
	genAllowed[GEN_SPEAK][ABS_STANDING] = 1;
	genAllowed[GEN_SPEAK][ABS_SITTING] = 1;
	genAllowed[GEN_SPEAK][ABS_CROUCHING] = 1;
	genAllowed[GEN_SPEAK][ABS_LIEING] = 1;
	genAllowed[GEN_SPEAK][ABS_WALKING] = 1;
	genAllowed[GEN_SPEAK][ABS_UNKNOWN] = 1;
			
	genAllowed[GEN_ARM][ABS_STANDING] = 1;
	genAllowed[GEN_ARM][ABS_SITTING] = 1;
	genAllowed[GEN_ARM][ABS_CROUCHING] = 1;
			
	genAllowed[GEN_HEAD][ABS_STANDING] = 1;
	genAllowed[GEN_HEAD][ABS_SITTING] = 1;
	genAllowed[GEN_HEAD][ABS_CROUCHING] = 1;
	genAllowed[GEN_HEAD][ABS_WALKING] = 1;
			
	genAllowed[GEN_DANCE][ABS_STANDING] = 1;
			
	genAllowed[GEN_WIPE][ABS_STANDING] = 1;
	genAllowed[GEN_WIPE][ABS_SITTING] = 1;
	genAllowed[GEN_WIPE][ABS_CROUCHING] = 1;
	genAllowed[GEN_WIPE][ABS_WALKING] = 1;
			
	genAllowed[GEN_HELLO][ABS_STANDING] = 1;
	genAllowed[GEN_HELLO][ABS_SITTING] = 1;
	genAllowed[GEN_HELLO][ABS_CROUCHING] = 1;
	genAllowed[GEN_HELLO][ABS_WALKING] = 1;
}

int Manager::retrieveTrans(const int& from, const int& to, event_params_t* ep)
{
	if((from < 0 || from >= NUM_ABS_STATES) || (to < 0 || to >= NUM_ABS_STATES))
		return -1;
		
	if (ep)
		*ep = absTransition[from][to];
	
	if(absTransition[from][to].type == CODE_INVALID)
		return -2;
	if(absTransition[from][to].type == CODE_VALID)
		return 1;		
	return 0;
}

int Manager::isGenAllowed(const int& gen, const int& abs)
{
	if ((gen < 0) || (gen >= NUM_GEN_STATES) || (abs < 0) || (abs >= NUM_ABS_STATES))
		return -1;
		
	return genAllowed[gen][abs];
}

int Manager::processConflicts(Event* event)
{
	switch (event->ep.type)
	{
		case INIT_WALK:
			if (isblocked(C_WALK))
				return -1;
			if (stateAbs == STATE_CROUCHING)
			{
				blockfor(C_WALK);
				inTransition = 1;
				blockGen = 1;
				return 1;
			}
			else 
				return -3;
			
		case INIT_REST:	
			if (isblocked(C_REST))
				return -1;
			if (retrieveTrans(stateAbs, ABS_CROUCHING, 0) >= 0)
			{
				blockfor(C_REST);
				inTransition = 1;
				blockGen = 1;
				return 1;
			}
			else 
				return 0;
			break;
		case INIT_SIT:	
			if (isblocked(C_SIT))
				return -1;
			if (retrieveTrans(stateAbs, ABS_SITTING, 0) >= 0)
			{
				blockfor(C_SIT);
				inTransition = 1;
				blockGen = 1;
				return 1;
			}
			else 
				return 0;
			break;
		case INIT_UP:
			if (isblocked(C_UP))
				return -1;
			if (retrieveTrans(stateAbs, ABS_STANDING, 0) >= 0)
			{
				blockfor(C_UP);
				inTransition = 1;
				blockGen = 1;
				return 1;
			}
			else 
				return 0;
			break;
		case CODE_EXE:
		{
			string& name = event->ep.sparam;
			int fstate = blist->getfState(name);
			int lstate = blist->getlState(name);
			
			if ((fstate >= 0) && (lstate >= 0))
			{
				if (isblocked(C_EXE))
					return -1;
				if (retrieveTrans(stateAbs, lstate, 0) >= 0)
				{
					blockfor(C_EXE);
					inTransition = 1;
					blockGen = 1;
					return 1;
				}
				else 
					return 0;
				break;
			}
			else if ((fstate < 0) && (lstate < 0))
			{
				blockfor(C_EXE_PAR);
				return 1;
			}
			else 
			{
				return -3;
			}
			break;
		}
		case INIT_WIPE:
			if(isblocked(C_WIPE))
				return -2;
			if (isGenAllowed(GEN_WIPE, stateAbs) == 1)
			{
				blockfor(C_WIPE);
				blockGen = 1;
				return 1;
			}
			else 
				return -3;
		case INIT_WAVE:	
			if(isblocked(C_WAVE))
				return -2;
			if (isGenAllowed(GEN_HELLO, stateAbs) == 1)
			{
				blockfor(C_WAVE);
				blockGen = 1;
				return 1;
			}
			else 
				return -3;
			break;
		case INIT_DANCE:
			if (isblocked(C_DANCE))
				return -1;
			if (isGenAllowed(GEN_DANCE, stateAbs) == 1)
			{
				blockfor(C_DANCE);
				inTransition = 1;
				blockGen = 1;
				return 1;
			}
			else 
				return 0;
			break;
		case CODE_SPK:
			return 1;
			break;
		case CODE_MOV:
			if (isblocked(C_MOV))
				return -1;
			if (retrieveTrans(stateAbs, ABS_WALKING, 0) >= 0)
			{
				blockfor(C_MOV);
				inTransition = 1;
				blockGen = 0;
				return 1;
			}
			else 
				return 0;
			break;
		case CODE_HEAD:
			if(isblocked(C_HEAD))
				return -2;
			if (isGenAllowed(GEN_HEAD, stateAbs) == 1)
			{
				blockfor(C_HEAD);
				blockGen = 1;
				return 1;
			}
			else 
				return -3;
			break;
		case CODE_ARM:
			if(isblocked(C_ARM))
				return -2;
			if (isGenAllowed(GEN_ARM, stateAbs) == 1)
			{
				blockfor(C_ARM);
				blockGen = 1;
				return 1;
			}
			else 
				return -3;
			break;
		case CODE_STOP:
			if(isblocked(C_STOP))
			{
				cout<< "[Manager]<processConflicts>CODE_STOP blocked!" << endl;
				return -3;
			}
			if (retrieveTrans(stateAbs, ABS_STANDING, 0) >= 0)
			{
				cout<< "[Manager]<processConflicts>CODE_STOP NOT blocked!" << endl;
				//TODO CODE_STOP wird immer wieder gescheduled solange der Robo noch läuft und ein Laufevent daher nicht bedient werden kann.
				//Wenn CODE_STOP gescheduled wurde müssen alle anderen Lauf und Stopevents geblockt werden.
				//Lösung implementierung einer Art block Mutex, der nur bestimmte Events für einen bestimmten Befehl zulässt.
				//inTransition und blockGen wird damit hinfällig
				//TODO Wenn ganz viele Laufbewegungen gescheduled werden, wird für jede Laufbewegung ein CODE_STOP ausgeführt
				//Wenn eine Laufbewegung noch nicht begonnen hat, in dem Moment wo CodeStop ausgeführt wird, bleibt CODE_STOP ohne Wirkung if
				blockfor(C_STOP);			
				blockGen = 0;
				return 1;
			}
			else
				return -3;
			break;
		case CODE_STOPALL:
			if(isblocked(C_STOPALL))
				return -2;		
			blockfor(C_STOPALL);
			inTransition = 1;
			blockGen = 1;
			return 1;
			break;
		case RESET_CONNECTION:
			if(isblocked(C_RESET))
				return -2;		
			blockfor(C_RESET);
			inTransition = 1;
			blockGen = 1;
			return 1;
			break;
		case CODE_INVALID:
		default:
			return -3;
			break;		
	};
}

int Manager::resolveConflict(Event* event, const int& from, const int& to)
{
	event_params_t ep;
	int tto = 0;
	int ffrom = from;
	
	int states[NUM_ABS_STATES] = {-1,};
	int col[NUM_ABS_STATES] = {0,};
	int q[NUM_ABS_STATES] = {-1,};
	unsigned int step = 0;
	q[0] = from;
	int head = 0;
	int tail = 1;
	while (q[head] != -1)
	{
		++step;
		ffrom = q[head];
		q[head] = -1;
		++head;
		//Do not color the ABS_WALKING node if it is also the final state
		if(!((step == 1) && (to == from)))
			col[ffrom] = 1;
		if(head == NUM_ABS_STATES)
			head = 0;
		for (tto = 0; tto < NUM_ABS_STATES; ++tto)
		{
			if((col[tto] == 0) && (retrieveTrans(ffrom, tto, 0) == 0))
			{
				//only take paths which lead only in their final execution in ABS_UNKNOWN
				if ((to != ABS_UNKNOWN) && (tto == ABS_UNKNOWN))
				{
					col[tto] = 2;
				}
				else
				{
					states[tto] = ffrom;
					col[tto] = 1;
					q[tail] = tto;
					tail++;
					if (tail == NUM_ABS_STATES)
						tail = 0;
				}
			}
		}
		if(!((step == 1) && (to == from)))
			col[ffrom] = 2;
	}
	
	if(states[to] == -1)
		return -1;
	
	tto = states[to];
	do
	{
		ffrom = states[tto];
		retrieveTrans(ffrom, tto, &ep);
		eventList->addFirst(ep);
		tto = ffrom;	
	}
	while(ffrom != from);
	
	return 0;	
}

int Manager::adaptEventList(const int& confresult, Event* event)
{
	int abs;
	string& name = event->ep.sparam;
	int lstate = blist->getlState(name);
	if (confresult == -3)
	{
		if (event)
			eventList->setClassf(event->id, EVT_DONE);
	}
	else if(confresult == 0)
	{
		//Konflikt auflösen
		
		switch (event->ep.type)
		{
			case INIT_REST:
				abs = ABS_CROUCHING;
				break;
			case INIT_SIT:
				abs = ABS_SITTING;
				break;
			case INIT_UP:
				abs = ABS_STANDING;
				break;
			case CODE_MOV:
				abs = ABS_WALKING;
				break;
			case INIT_DANCE:
				abs = ABS_UNKNOWN;
				break;
			case CODE_EXE:
			{
				
				if (lstate >= 0)
					abs = lstate;
				else 
					abs = ABS_UNKNOWN;
				break;	
			}			
			default:
				return confresult;
				break;
		};
		if (resolveConflict(event, stateAbs, abs) != 0)
		{
			
			if (event)
				eventList->setClassf(event->id, EVT_DONE);
			cerr<< "Error: [Manager]<adaptEventList> No way found to resolve the conflict" << endl;
		}
	}
	else if((confresult == -1) || (confresult == -2))
	{
		//event zurückhalten, nächstes Event holen
		//automaticly done, when in Transition
	}
	return confresult;
}

void Manager::addCom(const int& type, const int& ip1, const int& ip2, const float& fp, const string& sp, const int& prio)
{
	event_params_t eventp = EP_DEFAULT(CODE_UNKNOWN);
	
	eventp.type = type;
	eventp.iparams[0] = ip1;
	eventp.iparams[1] = ip2;
	eventp.fparam = fp;
	eventp.sparam = sp;
	
	if (prio == 0)
		eventList->addFirst(eventp);
	else
		eventList->addEvent(eventp);
}

void Manager::localRespond()
{
	cout<< "RMManager was pinged at!" << endl;
	cout<< "Value of lastOp: ";
	//lastOp = mem.getData("lastOp");
	cout<< (int&)lastOp << endl;
	//dec->decoderRespond();
}

void Manager::initPipe(const int& writer)
{
	pipeWrite = writer;
}


void Manager::initIp4(const string& ip)
{
	ip4 = ip;
}


void Manager::runExecuter()
{
	struct exec_arg* targ[MAX_THREADS] = {0,};
	event_params_t ep = EP_DEFAULT(CODE_VALID);
	int taskID = 0;
	Event* event = 0;
	int tnum = 0;
	int conf = 0;
	unsigned int ticks = 0;
	
	stateAbs = ABS_UNKNOWN;
	eventList->setOrder(ORD_STRICT);
	//accessExec.exec->initWalk();	
//	accessExec.pexec.pCall("behave_stand");
//	AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
//	qi::os::msleep(12000);
//	pbehav.stopBehavior("stand");
//	accessExec.exec->initSecure();



	//accessExec.exec->setState(STATE_STANDING);
	//stateAbs = ABS_SITTING;
	accessExec.exec->setStateMan(&stateAbs, &inTransition, &blockGen, block, parblock);
	
	//ep.type = INIT_UP;
	//eventList->addEvent(ep);
//	ep.type = INIT_REST;
//	eventList->addEvent(ep);	
	while(1)
	{
		mutex->lock();
		if(accessExec.exec->cbcall)
		{
			if(accessExec.exec->cbstate == STATE_UNKNOWN)
			{
				accessExec.exec->cbcall = false;
				if (ticks == 0)
					++ticks;
			}
			else 
			{
				accessExec.exec->cbcall = false;
				ticks = 0;
			}
		}
		if(ticks >= 1)
		{
			++ticks;
			//if 2 secs in STATE UNKNOWN
			if (ticks >= 2000/10)
			{
				ticks = 0;
				stateAbs = ABS_UNKNOWN;
				accessExec.exec->cbinc = true;
				accessExec.exec->sendState();		
				try 
				{
					AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
					tts.say("Oh something went wrong!");
				}		
				catch(const AL::ALError& e)
				{
					cerr<< "EXCEPTION: " << e.what() << endl;
				}
			}
		}
		mutex->unlock();
		
		/*AL::ALRobotPoseProxy rr = AL::ALRobotPoseProxy(MB_IP, MB_PORT);
		int pose = (int)((float&)mem.getData("robotPose"));
		cout << "Posture: " << (rr.getPoseNames()[pose]) << endl;*/
		qi::os::msleep(10);
		try
		{
			free_done_threads(targ);
			eventList->removeDone();
			if (eventList->hasPending())
			{
				mutex->lock();
				if (inTransition)
					event = eventList->getPending(false);
				else
					event = eventList->getPending(true);
				
				
				conf = processConflicts(event);
				adaptEventList(conf, event);
				
				cout << "=======================================" << endl;
				cout << "[Current Event] " << event->ep.type << endl;
				eventList->list();
				cout << "=======================================" << endl;
				
				if (conf <= 0)
				{
					mutex->unlock();
					//cout<< "[Manager]<runExecuter> Robot in Motion right now, event waits" << endl;
				}
				else
				{
					tnum = new_thread(targ, event, eventList);
					if (tnum<0)
					{
						mutex->unlock();
						cerr<< "Error: [Manager]<runExecuter> Maximale Anzahl an Threads erreicht!" << endl;
					}
					else
					{
						eventList->setClassf(event->id, EVT_BUSY);							
						mutex->unlock();
						pthread_create(&targ[tnum]->id, 0, &Executer::execute, targ[tnum]);	
						pthread_detach(targ[tnum]->id);
						cout<< "[Manager]<runExecuter>: Threadcount = " << threadcount << endl;
						//ressourcen freigeben und Anzahl der aktiven THreads aktualisieren
					}
				}	
				//taskID = accessExec.pexec.pCall(string("process"));
			}
			else
			{
				eventList->setOrder(ORD_STRICT);
			}
		}
		catch(const AL::ALError& e)
		{
			cout<< "Error: [Manager]<runExecuter>:" << endl << e.what() << endl;
		}
	}
}

//void Manager::runExecuter()
//{
//	//AL::ALValue posVal = mem.getData("robotPose");
//	//cout<< "ROBOT POSE: " << posVal.toString() << endl;
//	//mem.insertData("robotPoseChanged", string("Sit"));
//	//mem.insertData("robotPose", 2);
//	//posVal = mem.getData("robotPose");
//	//cout<< "ROBOT POSE: " << posVal.toString() << endl;
//	//mem.insertData("robotPose", 5.0f);
//	//mem.insertData("robotPose", 3.0f);
//	//accessExec.exec->executerRespond();
//	//AL::ALValue post;
//	int taskID = 0;
//	//xec->setPosture((int&)post);
//	
//	
//	
//	eventList->setOrder(ORD_STRICT);
//	//accessExec.exec->initWalk();	
//	accessExec.pexec.pCall("behave_stand");
//	AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
//	qi::os::msleep(12000);
//	pbehav.stopBehavior("stand");
//	accessExec.exec->initSecure();
//	
//	/*{
//		AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
//		cout << "Behaviours" << endl;
//		AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
//		AL::ALValue behav = pbehav.getDefaultBehaviors();
//		cout << behav.toString() << endl;
//		//tts.say(behav.toString());
//		behav.clear();
//		behav = pbehav.getInstalledBehaviors();
//		cout << behav.toString() << endl;
//		//tts.say(behav.toString());
//		behav.clear();
//		pbehav.runBehavior("dance");
//		//behav = pbehav.getBehaviorNames();
//		//cout << behav.toString() << endl;
//		//tts.say(behav.toString());
//	}	*/
//	accessExec.exec->setState(STATE_STANDING);
//	
//	while(1)
//	{
//		
//		/*AL::ALRobotPoseProxy rr = AL::ALRobotPoseProxy(MB_IP, MB_PORT);
//		int pose = (int)((float&)mem.getData("robotPose"));
//		cout << "Posture: " << (rr.getPoseNames()[pose]) << endl;*/
//		qi::os::msleep(10);
//		try
//		{
//			eventList->removeDone();
//			if (eventList->hasPending())
//			{
//				//cout<< "HAS PENDING" << endl;
//				//int iid(pevent->ep.type);
//				//void* const iid= (void*)pevent;
//				//AL::ALValue iid((const void*)pevent->id, sizeof(pevent->id));
//				/*AL::ALValue iid;
//				iid.setObject(&pevent->id, sizeof(pevent->id));
//				cout<< "pevent->id = " << pevent->id << ", iid = " << iid.toString() 
//				<< ", getBinary = "<< (const void*)(*(const void**)(iid.getObject())) << endl;*/
//				//eventList->setClassf(pevent->id, EVT_BUSY);
//				taskID = accessExec.pexec.pCall(string("process"));
//				//eventList->setTask(event.id, taskID);
//			}
//			else
//			{
//				//cout<< "HAS NOT PENDING" << endl;
//				eventList->setOrder(ORD_STRICT);
//			}
//		}
//		catch(const AL::ALError& e)
//		{
//			cout<< "Error: [Manager]<runExecuter>:" << endl << e.what() << endl;
//		}
//	}
//}

int Manager::new_thread(struct exec_arg* targ[MAX_THREADS], Event* event, boost::shared_ptr<EventList> eL)
{
	int t = 0;
	while (targ[t] && (t<MAX_THREADS))
		++t;
	if (t==MAX_THREADS)
		return -1;
		
	++threadcount;
	targ[t] = new (struct exec_arg);//(struct exec_arg*)malloc(sizeof(struct exec_arg));
	targ[t]->tnum = t;
	targ[t]->event = event;
	targ[t]->eventList = eL;
	targ[t]->mutex = mutex;
	
	return t;
}

int Manager::delete_thread(struct exec_arg* targ[MAX_THREADS], int t)
{
	targ[t]->eventList.reset();
	targ[t]->mutex.reset();
	delete targ[t];
	targ[t] = 0;
	--threadcount;
	return t;
}

int Manager::free_done_threads(struct exec_arg* targ[MAX_THREADS])
{
	for (int i = 0; i<MAX_THREADS; ++i)
	{
		if(targ[i] && (targ[i]->event == 0))
			delete_thread(targ, i);
	}
	return 0;
}

int Manager::catch_dangling_threads(struct exec_arg* targ[MAX_THREADS])
{
	int t = 0;
	while(threadcount>0)
	{
		for(t = 0; t<MAX_THREADS; ++t)
		{
			if(targ[t])
			{
				pthread_join(targ[t]->id, 0);
				delete_thread(targ, t);
			}
		}
	}
}


AcessExec::AcessExec(boost::shared_ptr<AL::ALBroker> broker)
:	exec(AL::ALModule::createModule<Executer>(broker, "RMExecuter")), 
 	pexec(AL::ALProxy(string("RMExecuter"), AL::ALProxy::FORCED_LOCAL, 0))
{
}

AcessExec::~AcessExec()
{
}
	

