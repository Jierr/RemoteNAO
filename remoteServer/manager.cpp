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
	addParam("sp", "");
	addParam("prio", "");
	BIND_METHOD(Manager::addCom)
	
	mem = AL::ALMemoryProxy(broker);
	
	accessExec.exec->initEventList(eventList);
}

Manager::~Manager()
{
	accessExec.exec->exit();
}




void Manager::init()
{	
}

void Manager::addCom(const int& type, const int& ip1, const int& ip2, const string& sp, const int& prio)
{
	event_params_t eventp = {CODE_UNKNOWN, {0,0}, ""};
	
	eventp.type = type;
	eventp.iparams[0] = ip1;
	eventp.iparams[1] = ip2;
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
		else if (fstr.compare("SIT") == 0)
		{
			ep.type = INIT_SIT;
			fstr = "";
			//return <ingnore params>
			return false;
		}
		else if (fstr.compare("AUF") == 0)
		{
			ep.type = INIT_UP;
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
			ep.type = CODE_STOPALL;
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
		else if (fstr.compare("ZST") == 0)
		{
			ep.type = CODE_STATE;
			fstr = "";
			return false;
		}
		else if (fstr.compare("DNC") == 0)
		{
			ep.type = INIT_DANCE;
			fstr = "";
			//return <ingnore params>
			return false;
		}
		else if (fstr.compare("WNK") == 0)
		{
			ep.type = INIT_WAVE;
			fstr = "";
			//return <ingnore params>
			return false;
		}
		else if (fstr.compare("WIP") == 0)
		{
			ep.type = INIT_WIPE;
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
		else if (fstr.compare("ARM") == 0)
			ep.type = CODE_ARM;
		else if (fstr.compare("VID") == 0)
			ep.type = CODE_VID;
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
			//cout<< "getParams:" << toParse[pos] << endl;
			if ((toParse[pos] != '_') && (paramCount==1))
			{
				ep.sparam+=toParse[pos];
				//cout<< "getParams:		" << ep.sparam << endl;
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
		case CODE_ARM:
			if (paramCount == 1)
			{
				switch (toParse[pos])
				{
					case 'L':
						ep.iparams[0] = ARM_LEFT;
						break;
					case 'R':
						ep.iparams[0] = ARM_RIGHT;
						break;
					default:
						ep.type = CODE_INVALID;
						break;
				};
				++pos;
			}
			else if (paramCount == 2)
			{
				switch (toParse[pos])
				{
					case 'F':
						ep.iparams[1] = MOV_FORWARD;
						break;
					case 'B':
						ep.iparams[1] = MOV_BACKWARD;
						break;
					case 'L':
						ep.iparams[1] = MOV_LEFT;
						break;
					case 'R':
						ep.iparams[1] = MOV_RIGHT;
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
			
		case CODE_VID:
			if (paramCount == 1)
			{
				switch (toParse[pos])
				{
					case 'A':
						ep.iparams[0] = VON;
						break;
					case 'D':
					{
						ep.iparams[0] = VOFF;				
						unsigned char com = (char)(ep.iparams[0]);
						char comStr[2] = {0,};
						ep.type = CODE_INVALID;
						try
						{		
							AL::ALProxy pNetNao = AL::ALProxy(string("RMNetNao"), CB_IP, CB_PORT);

							comStr[0] = VOFF;
							pNetNao.callVoid<int, AL::ALValue, int>(
								   "writePipe", pipeWrite, (const char*)comStr, 1);
		
						}
						catch (const AL::ALError& e)
						{
							cout<< "ERROR" << endl << e.what() << endl;
						}	
						++pos;
						return false;
						break;
					}
					default:
						ep.type = CODE_INVALID;
						break;
				};
				++pos;
			}
			else if (paramCount == 2)
			{
				if ((toParse[pos] != '_'))
				{
					ep.sparam+=toParse[pos];
					cout<< "getParams:		" << ep.sparam << endl;
				}
				++pos;		
			}			
			
			if (toParse[pos] == '_')
			{
				++paramCount;
				++pos;
			}	
			
			if(paramCount == 3)
			{		
				cout<< "[PARSER] Übertage Port und Ip zum CAM-Modul" << endl;
				cout<< "[PARSER] IP = " << ip4 << ":" << ep.sparam << endl;
				unsigned char com = (char)(ep.iparams[0]);
				ep.type = CODE_INVALID;
				char comStr[2] = {0,};
				
				if (com == VON)
				{
					cout<< "[PARSER] com == VON" << endl
						<< "[PARSER] Schreiber = " << pipeWrite << endl;
						
					try
					{		
						AL::ALProxy pNetNao = AL::ALProxy(string("RMNetNao"), CB_IP, CB_PORT);

						comStr[0] = VON;
						pNetNao.callVoid<int, AL::ALValue, int>(
							   "writePipe", pipeWrite, (const char*)comStr, 1);
					    comStr[0] = VIP;						    
					    pNetNao.callVoid<int, AL::ALValue, int>(
							   "writePipe", pipeWrite, (const char*)comStr, 1);							   
						pNetNao.callVoid<int, AL::ALValue, int>(
							   "writePipe", pipeWrite, ip4.c_str(), ip4.length()+1);
						comStr[0] = VPORT;
					    pNetNao.callVoid<int, AL::ALValue, int>(
							   "writePipe", pipeWrite, (const char*)comStr, 1);
					    pNetNao.callVoid<int, AL::ALValue, int>(
							   "writePipe", pipeWrite, ep.sparam.c_str(), ep.sparam.length()+1);
		
					}
					catch (const AL::ALError& e)
					{
						cout<< "ERROR" << endl << e.what() << endl;
					}				
				}

				
				return false;	
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
				
				if (ttype == CODE_STOPALL)
					eventList->addFirst(eventp);
				else
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
	//accessExec.exec->initWalk();	
	accessExec.pexec.pCall("behave_stand");
	AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
	qi::os::msleep(12000);
	pbehav.stopBehavior("stand");
	accessExec.exec->initSecure();
	
	/*{
		AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
		cout << "Behaviours" << endl;
		AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
		AL::ALValue behav = pbehav.getDefaultBehaviors();
		cout << behav.toString() << endl;
		//tts.say(behav.toString());
		behav.clear();
		behav = pbehav.getInstalledBehaviors();
		cout << behav.toString() << endl;
		//tts.say(behav.toString());
		behav.clear();
		pbehav.runBehavior("dance");
		//behav = pbehav.getBehaviorNames();
		//cout << behav.toString() << endl;
		//tts.say(behav.toString());
	}	*/
	accessExec.exec->setState(STATE_STANDING);
	
	while(1)
	{
		
		/*AL::ALRobotPoseProxy rr = AL::ALRobotPoseProxy(MB_IP, MB_PORT);
		int pose = (int)((float&)mem.getData("robotPose"));
		cout << "Posture: " << (rr.getPoseNames()[pose]) << endl;*/
		qi::os::msleep(10);
		try
		{
			eventList->removeDone();
			if (eventList->hasPending())
			{
				//cout<< "HAS PENDING" << endl;
				//int iid(pevent->ep.type);
				//void* const iid= (void*)pevent;
				//AL::ALValue iid((const void*)pevent->id, sizeof(pevent->id));
				/*AL::ALValue iid;
				iid.setObject(&pevent->id, sizeof(pevent->id));
				cout<< "pevent->id = " << pevent->id << ", iid = " << iid.toString() 
				<< ", getBinary = "<< (const void*)(*(const void**)(iid.getObject())) << endl;*/
				//eventList->setClassf(pevent->id, EVT_BUSY);
				taskID = accessExec.pexec.pCall(string("process"));
				//eventList->setTask(event.id, taskID);
			}
			else
			{
				//cout<< "HAS NOT PENDING" << endl;
				eventList->setOrder(ORD_STRICT);
			}
		}
		catch(const AL::ALError& e)
		{
			cout<< "Error: [Manager]<runExecuter>:" << endl << e.what() << endl;
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
	

