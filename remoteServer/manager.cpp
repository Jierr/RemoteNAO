#include <iostream>
#include <vector>
#include <alvalue/alvalue.h>
#include <althread/almutex.h>
#include <althread/alcriticalsection.h>
#include <alproxies/almemoryproxy.h>
#include <alproxies/altexttospeechproxy.h>
#include <qi/os.hpp>

#include "manager.h"
#include "decoder.h"
#include "executer.h"
#include "gen.h"


using namespace std;
	
Manager::Manager(boost::shared_ptr<AL::ALBroker> broker, const string& name)
: AL::ALModule(broker, name), mutex(AL::ALMutex::createALMutex())
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
	
	mem.insertData("lastOp", CODE_INVALID);
	mem.insertData("msg", string("")); 
	
	vector<int> vtemp (2, 0);
	vector<int> vtemp2 (2, 1);
	mem.insertData("iparams", (vector<int>)vtemp);
	vtemp2 = mem.getData("iparams");
	cout << "vtemp2[0] = " << vtemp2[0] << endl;
	cout << "vtemp2[1] = " << vtemp2[1] << endl;
	
	lastOp = mem.getData("lastOp");
	cout<< "lastOp Constructor: " << (int&)lastOp << endl;
	
	cout<< "Constructor!" << endl;
	dec = AL::ALModule::createModule<Decoder>(broker, "RMDecoder");
	exec = AL::ALModule::createModule<Executer>(broker, "RMExecuter");
}

Manager::~Manager()
{
	exec->exit();
	dec->exit();
}

void Manager::init()
{
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
		if (fstr == "INI")
		{
			ep.type = INIT_WALK;
			fstr = "";
			//return <ingnore params>
			return false;
		}
		else if (fstr == "RST")
		{
			ep.type = INIT_REST;
			fstr = "";
			//return <ingnore params>
			return false;
		}
		else if (fstr == "MOV")
			ep.type = CODE_MOV;
		else if (fstr == "SPK")
			ep.type = CODE_SPK;
		else 
			ep.type = CODE_INVALID;	
		fstr = "";	
	}
	//return <process params>
	return true;
}



bool Manager::getParams(const string& toParse, int& pos, event_params_t& ep, int& paramCount)
{
	switch (ep.type)
	{
		case INIT_WALK:
			//do not continue looking for more params
			return false;
			break;
		case INIT_REST:
			//do not continue looking for more params
			return false;
			break;
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
					return false;
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
	static event_params_t eventp = {CODE_UNKNOWN, {0,0}, ""};
	cout<< "In DECODE ====================" << endl
		<< "code-argument[" << toParse.length() << "] = " << toParse << endl;
	cout<< "Symbol = " << eventp.type << ", Pos = " << pos << endl;
	
	//needs to end when UNKNOWN Command not fully given
	//needs to resume when not fully staged yet
	while ((pos < toParse.length()) && 
		   (stage != STG_VALID)     && 
		   (stage != STG_ERROR)         )
	{
		switch (stage)
		{
			case STG_FETCH:	
				cont = fetch(toParse, pos, eventp);
				//As long the code is unknown fetch more
				if ((eventp.type != CODE_INVALID) && (eventp.type != CODE_UNKNOWN))
				{	
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
	
	cout<< "STAGE: " << stage << ", type: " << eventp.type << ", sparam: " << eventp.sparam << endl;
	switch (stage)
	{			
		case STG_VALID:
		{
			cout<< "Decoded Function: " << (int&)eventp.type << ", " << eventp.sparam << endl;
			vector<int> vtemp (eventp.iparams, eventp.iparams + sizeof(eventp.iparams)/sizeof(int));
			mutex->lock();
				mem.insertData("lastOp", eventp.type);
				mem.insertData("msg", eventp.sparam);
				mem.insertData("iparams", (vector<int>)vtemp);
			mutex->unlock();
						
			pos = 0;
			stage = STG_FETCH;
			paramCount = 0;
			cont = false;
			eventp.type = CODE_UNKNOWN;
			for (int p = 0; p < IPARAM_LEN; ++p)
				eventp.iparams[p] = 0;
			eventp.sparam = "";
			break;
		}
		case STG_ERROR:
		{
			cout << "Error Invalid CODE!" << endl;
			pos = 0;
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
	AL::ALValue posVal = mem.getData("robotPose");
	cout<< "ROBOT POSE: " << posVal.toString() << endl;
	//mem.insertData("robotPoseChanged", string("Sit"));
	//mem.insertData("robotPose", 2);
	posVal = mem.getData("robotPose");
	cout<< "ROBOT POSE: " << posVal.toString() << endl;
	//mem.insertData("robotPose", 5.0f);
	//mem.insertData("robotPose", 3.0f);
	//exec->executerRespond();
	AL::ALValue post;
	//xec->setPosture((int&)post);
	

	
	while(1)
	{
		string msg = "";
		vector<int> vtemp (2, 0);
		qi::os::msleep(50);
		mutex->lock();
			lastOp = mem.getData("lastOp");
			msg = (string&)mem.getData("msg");
			vtemp =  mem.getData("iparams");
			mem.insertData("lastOp", CODE_INVALID);
		mutex->unlock();
	
		switch ((int&)lastOp)
		{
			case INIT_WALK:
				exec->initWalk();
				//cout << "Done initWalk()" << endl;
				break;
			case INIT_REST:
				exec->initSecure();
				break;
			case CODE_SPK:
				exec->speak(msg);
				break;
			case CODE_MOV:
				exec->walk(vtemp[0]);
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
					cout<< "Error runExecuter case default: " << endl << e.what() << endl;
				}
				break;
			}
		
		};
	}
}
/*
boost::weak_ptr<Manager> Manager::getManager()
{
	static boost::shared_ptr<Manager> pManager(this, null_deleter());
	managerSingleton = pManager;
	return managerSingleton;
}
*/
	

