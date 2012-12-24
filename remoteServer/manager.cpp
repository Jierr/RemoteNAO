#include <iostream>
#include <vector>
#include <alvalue/alvalue.h>
#include <althread/almutex.h>
#include <althread/alcriticalsection.h>
#include <alproxies/almemoryproxy.h>
#include <alproxies/altexttospeechproxy.h>
#include <alcommon/alproxy.h>
#include <qi/os.hpp>
#include <alproxies/alrobotposeproxy.h>

#include "manager.h"
#include "decoder.h"
#include "executer.h"
#include "gen.h"
#include "eventlist.h"


using namespace std;
	
Manager::Manager(boost::shared_ptr<AL::ALBroker> broker, const string& name)
:	AL::ALModule(broker, name), 
	mutex(AL::ALMutex::createALMutex()),
	accessExec(broker),
	eventList(new EventList)
{
	setModuleDescription("This Module manages the Data comming from the remoteServer module");
	
	functionName("localRespond", getName(), "local Respond");
	BIND_METHOD(Manager::localRespond);
	
	functionName("runExecuter", getName(), "run Executer");
	BIND_METHOD(Manager::runExecuter);	
	
	functionName("decode", getName(), "decode the argument given");
	addParam("toParse", "const string&: the symbol to be decoded");
	setReturn("int", "sucess: return pos parsed, failure: return 0");
	BIND_METHOD(Manager::decode);
	
	mem = AL::ALMemoryProxy(broker);
	//mem.setDescription(string("lastOp"), string("Last Operation"));	
	
	//mem.insertData("lastOp", CODE_INVALID);
	//mem.insertData("msg", string("")); 
	
	vector<int> vtemp (2, 0);
	vector<int> vtemp2 (2, 1);
	//mem.insertData("iparams", (vector<int>)vtemp);
	//vtemp2 = mem.getData("iparams");
	cout << "vtemp2[0] = " << vtemp2[0] << endl;
	cout << "vtemp2[1] = " << vtemp2[1] << endl;
	
	//lastOp = mem.getData("lastOp");
	cout<< "lastOp Constructor: " << (int&)lastOp << endl;
	
	cout<< "Constructor!" << endl;
	//dec = AL::ALModule::createModule<Decoder>(broker, "RMDecoder");
	//exec = AL::ALModule::createModule<Executer>(broker, "RMExecuter");
	
	//accessExec.exec = exec;
	//accessExec.pexec = AL::ALProxy(string("RMExecuter"), AL::ALProxy::FORCED_LOCAL, 0);
	accessExec.exec->initEventList(eventList);
	//accessExec.exec->executerRespond();
}

Manager::~Manager()
{
	accessExec.exec->exit();
}

void Manager::init()
{
	/*event_params_t ep;
	Event* tmp;
	
	cout<< "Initialisierung:" << endl;
	for (int n = 0; n < 10; ++n)
	{
		ep.type = n;
		eventList->addEvent(ep);
	}
	if (!eventList->isEmpty())
		eventList->list();
	
	
	cout<< "Lösche First:" << endl;
	tmp = new Event(eventList->getFirst());
	eventList->removeEvent(tmp->id);
	delete tmp;
	
	if (!eventList->isEmpty())
		eventList->list();	

	
	cout<< "Lösche Last" << endl;	
	tmp = new Event(eventList->getLast());
	eventList->removeEvent(tmp->id);
	delete tmp;
	
	if (!eventList->isEmpty())
		eventList->list();
		
	for (int i = 0; i< 8; ++i)
	{
		cout<< "Lösche PENDING------------------" << endl;	
		tmp = new Event(eventList->getPending());
		eventList->removeEvent(tmp->id);
		delete tmp;
	
		if (!eventList->isEmpty())
			eventList->list();
	}
	*/
	
	
}

void Manager::localRespond()
{
	cout<< "RMManager was pinged at!" << endl;
	cout<< "Value of lastOp: ";
	//lastOp = mem.getData("lastOp");
	cout<< (int&)lastOp << endl;
	//dec->decoderRespond();
}

bool Manager::fetch(const string& toParse, int& pos, event_params_t& ep)
{
	static string fstr = "";
	if (pos <= 2)
	{
		fstr+=toParse[pos];
		++pos;
	}
	
	if (pos == 3)
	{
		if (fstr.compare("INI") == 0)
		{
			ep.type = INIT_WALK;
			fstr = "";
			//return <ingnore params>
			return false;
		}
		else if (fstr.compare("RST") == 0)
		{
			ep.type = INIT_REST;
			fstr = "";
			//return <ingnore params>
			return false;
		}
		else if (fstr.compare("DIS") == 0)
		{
			ep.type = RESET_CONNECTION;
			fstr = "";
			return false;
		}		
		else if (fstr.compare("STP") == 0)
		{
			ep.type = CODE_STOP;
			ep.iparams[0] = MOV_STOP;
			fstr = "";
			return false;
		}
		else if (fstr.compare("BAT") == 0)
		{
			ep.type = CODE_BAT;
			fstr = "";
			//return <ingnore params>
			return false;
		}
		else if (fstr.compare("MOV") == 0)
			ep.type = CODE_MOV;
		else if (fstr.compare("SPK") == 0)
			ep.type = CODE_SPK;
		else if (fstr.compare("HAD") == 0)
			ep.type = CODE_HEAD;
		else 
			ep.type = CODE_INVALID;	
		fstr = "";	
	}
	//return <parse for process params>
	return true;
}



bool Manager::getParams(const string& toParse, int& pos, event_params_t& ep, int& paramCount)
{
	switch (ep.type)
	{
		case CODE_SPK:
		{
			cout<< "getParams:" << toParse[pos] << endl;
			if ((toParse[pos] != '_') && (paramCount==1))
			{
				ep.sparam+=toParse[pos];
				cout<< "getParams:		" << ep.sparam << endl;
			}
			else if (toParse[pos] == '_')
			{
				++paramCount;
				if (paramCount >= 2)
				{
					++pos;
					return false;
				}
			}
			else 
			{
				ep.type = CODE_INVALID;
				return false;
			}			
			++pos;
			break;
		}
		case CODE_MOV:	
			if (paramCount == 1)
			{
				switch (toParse[pos])
				{
					case 'F':
						ep.iparams[0] = MOV_FORWARD;
						break;
					case 'B':
						ep.iparams[0] = MOV_BACKWARD;
						break;
					case 'L':
						ep.iparams[0] = MOV_LEFT;
						break;
					case 'R':
						ep.iparams[0] = MOV_RIGHT;
						break;
					default:
						ep.type = CODE_INVALID;
						break;
				};
				++pos;
				return false;
			}
			
			if (toParse[pos] == '_')
			{
				++paramCount;
				++pos;
			}			
			break;
		case CODE_HEAD:
			if (paramCount == 1)
			{
				switch (toParse[pos])
				{
					case 'F':
						ep.iparams[0] = MOV_FORWARD;
						break;
					case 'B':
						ep.iparams[0] = MOV_BACKWARD;
						break;
					case 'L':
						ep.iparams[0] = MOV_LEFT;
						break;
					case 'R':
						ep.iparams[0] = MOV_RIGHT;
						break;
					default:
						ep.type = CODE_INVALID;
						break;
				};
				++pos;
				return false;
			}
			
			if (toParse[pos] == '_')
			{
				++paramCount;
				++pos;
			}			
			break;
		default:
			ep.type = CODE_INVALID;
			return false;
			break;
	};
	return true;

}

int Manager::decode(const string& toParse)
{
	AL::ALValue result;
	static bool cont = false;
	static int stage = STG_FETCH;
	static int paramCount = 0;
	static int pos = 0;
	static int trace = 0;
	static event_params_t eventp = {CODE_UNKNOWN, {0,0}, ""};
	cout<< "In DECODE ====================" << endl
		<< "code-argument[" << toParse.length() << "] = " << toParse << endl;
	cout<< "Symbol = " << eventp.type << ", Pos = " << pos << endl;
	
	//needs to end when UNKNOWN Command not fully given
	//needs to resume when not fully staged yet
	while ((pos < toParse.length()) && 
		   (stage != STG_VALID)     && 
		   (stage != STG_ERROR)        )
	{
		switch (stage)
		{
			case STG_FETCH:	
				cont = fetch(toParse, pos, eventp);
				//As long the code is unknown fetch more
				if ((eventp.type != CODE_INVALID) && (eventp.type != CODE_UNKNOWN))
				{	
					//if !cont skipp the scann for parameters
					if (!cont)			
						stage = STG_VALID;
					else 
						stage = STG_PARAM;
				}
				else if (eventp.type == CODE_INVALID)
				{
					stage = STG_ERROR;
				}
				break;
			case STG_PARAM:
				if (eventp.type != CODE_INVALID)
					cont = getParams(toParse, pos, eventp, paramCount);
				else 
				{
					stage = STG_ERROR;
				}
			
				if (!cont && (eventp.type != CODE_INVALID))
				{
					stage = STG_VALID;
				}
				break;
				default:
					stage = STG_ERROR;
					break;
		};
		


	}	
		//cout<< "POS: " << pos << ", Trace: " << trace << endl;	
		cout<< "STAGE: " << stage << ", type: " << eventp.type << ", sparam: " << eventp.sparam << endl;
		switch (stage)
		{			
			case STG_VALID:
			{
				cout<< "-------------->Decoded Function: " << (int&)eventp.type << ", " << eventp.sparam << endl;
				int ttype = eventp.type;
				/*
				vector<int> vtemp (eventp.iparams, eventp.iparams + sizeof(eventp.iparams)/sizeof(int));
				mutex->lock();
					mem.insertData("lastOp", eventp.type);
					mem.insertData("msg", eventp.sparam);
					mem.insertData("iparams", (vector<int>)vtemp);
				mutex->unlock();	*/
				eventList->addEvent(eventp);
			
				pos = 0;
				trace = pos;
				stage = STG_FETCH;
				paramCount = 0;
				cont = false;
				eventp.type = CODE_UNKNOWN;
				for (int p = 0; p < IPARAM_LEN; ++p)
					eventp.iparams[p] = 0;
				eventp.sparam = "";
				if (ttype == RESET_CONNECTION)
					return -1;
			
				break;
			}
			case STG_ERROR:
			{
				cout << "-------------->Error Invalid CODE!" << endl;
				pos = 0;
				++trace;
				stage = STG_FETCH;
				paramCount = 0;
				cont = false;
				eventp.type = CODE_UNKNOWN;
				for (int p = 0; p < IPARAM_LEN; ++p)
					eventp.iparams[p] = 0;
				eventp.sparam = "";
				break;
			}
			default:
				break;	
		};

	cout<< "End DECODE ===================" << endl;

	return pos;

	//lastOp = code;
	
}

void Manager::runExecuter()
{
	//AL::ALValue posVal = mem.getData("robotPose");
	//cout<< "ROBOT POSE: " << posVal.toString() << endl;
	//mem.insertData("robotPoseChanged", string("Sit"));
	//mem.insertData("robotPose", 2);
	//posVal = mem.getData("robotPose");
	//cout<< "ROBOT POSE: " << posVal.toString() << endl;
	//mem.insertData("robotPose", 5.0f);
	//mem.insertData("robotPose", 3.0f);
	//accessExec.exec->executerRespond();
	//AL::ALValue post;
	int taskID = 0;
	//xec->setPosture((int&)post);
	


	eventList->setOrder(ORD_STRICT);
	accessExec.exec->initWalk();	
	accessExec.exec->setState(STATE_STANDING);
	while(1)
	{
		
		/*AL::ALRobotPoseProxy rr = AL::ALRobotPoseProxy(MB_IP, MB_PORT);
		int pose = (int)((float&)mem.getData("robotPose"));
		cout << "Posture: " << (rr.getPoseNames()[pose]) << endl;*/
		qi::os::msleep(50);
		try
		{
			eventList->removeDone();
			//cout<< "removeDone" << endl;
			Event event(eventList->getPending());
			eventList->setClassf(event.id, EVT_BUSY);
			taskID = accessExec.pexec.pCall<Event>("process", event);
			//eventList->setTask(event.id, taskID);
		}
		catch(const AL::ALError& e)
		{
			cout<< "Error: [Manager]<runExecuter>:" << endl << e.what() << endl;
		}
		/*
		string msg = "";
		vector<int> vtemp (2, 0);
		mutex->lock();
			lastOp = mem.getData("lastOp");
			msg = (string&)mem.getData("msg");
			vtemp =  mem.getData("iparams");
			mem.insertData("lastOp", CODE_INVALID);
		mutex->unlock();
		
		switch (event.ep.type)
		{
			case INIT_WALK:
				accessExec.exec->initWalk();
				//cout << "Done initWalk()" << endl;
				break;
			case INIT_REST:
				accessExec.exec->initSecure();
				break;
			case CODE_SPK:
				accessExec.exec->speak(msg);
				break;
			case CODE_MOV:
				accessExec.exec->walk(event);
				break;
			case CODE_STOP:
				//event.ep.iparams[0] = MOV_STOP;
				accessExec.exec->walk(event);
				break;
			case CODE_BAT:
				accessExec.exec->sendBatteryStatus();
				break;
			case RESET_CONNECTION:
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
					cout<< "Error runExecuter case default: " << endl << e.what() << endl;
				}
				break;
			}
		
		};*/
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
	

