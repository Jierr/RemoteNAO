#include <iostream>
#include <alvalue/alvalue.h>
#include <alproxies/almemoryproxy.h>
#include <alproxies/altexttospeechproxy.h>

#include "manager.h"
#include "decoder.h"
#include "executer.h"
#include "gen.h"


using namespace std;
	
Manager::Manager(boost::shared_ptr<AL::ALBroker> broker, const string& name)
: AL::ALModule(broker, name)
{
	setModuleDescription("This Module manages the Data comming from the remoteServer module");
	
	functionName("localRespond", getName(), "local Respond");
	BIND_METHOD(Manager::localRespond);
	
	functionName("runExecuter", getName(), "run Executer");
	BIND_METHOD(Manager::runExecuter);	
	
	functionName("decode", getName(), "decode the argument given");
	addParam("toParse", "const string&: the symbol to be decoded");
	BIND_METHOD(Manager::decode);
	
	mem = AL::ALMemoryProxy(broker);
	//mem.setDescription(string("lastOp"), string("Last Operation"));	
	lastOp = 42;	
	mem.insertData("lastOp", lastOp);
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
	dec->decoderRespond();
}

string Manager::fetch(const string& untouched)
{
	int pos = 0;
	string result = "";
	while (pos < untouched.length())
	{
		if (untouched[pos] != '_')
			result+=untouched[pos];
		else
			return result;
		++pos;
	}
	return result;
}


bool Manager::getParams(const string& code, const string& touched, void**& params)
{

	if (code.compare("INIT") == 0)
	{
		params = new void*[2];
		params[0] = new int;
		params[1] = 0;
		*(int*)params[0] = INIT_WALK;
		return true;
	}
	else if (code.compare("MOV") == 0)
	{
		params = new void*[2];
		params[0] = new int;
		params[1] = 0;
		*(int*)params[0] = CODE_MOV;
		return true;
	}
	return false;
}

void Manager::decode(const string& toParse)
{
	AL::ALValue result;
	int pos = 0;
	string symbol = "";
	bool valid = false;
	void** params = 0;
	cout<< "In DECODE ====================" << endl
		<< "code-argument = " << toParse << endl
		<< "End DECODE ===================" << endl;
	
	symbol = fetch(toParse);
	cout<< "Symbol = " << symbol << endl;
	valid = getParams(symbol, toParse, params);
	
	
	if (valid)
	{
	    result = *(int*)params[0];
		cout<< "Decoded Function: " << (int&)result << endl;
		mem.insertData("lastOp", result);
		
		for (int i = 0; params[i] != 0; ++i)
		{
			delete params[i];
		}
		delete[] params;
	}
	else 
		mem.insertData("lastOp", CODE_INVALID);
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
	exec->executerRespond();
	AL::ALValue post;
	exec->setPosture((int&)post);
	
	lastOp = mem.getData("lastOp");
	
	switch ((int&)lastOp)
	{
		case INIT_WALK:
			exec->initSecure();
			cout << "Done initSecure()" << endl;
			break;
		case CODE_MOV:
			break;
		case CODE_INVALID:
			cout<< "Nothing to do" << endl;
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
	//while(1)
	//{
		/*post = mem.getData("lastOp");
		exec->setPosture((int&)post);
		post = 0;
		mem.insertData("lastOp", post);*/
	//}
}
/*
boost::weak_ptr<Manager> Manager::getManager()
{
	static boost::shared_ptr<Manager> pManager(this, null_deleter());
	managerSingleton = pManager;
	return managerSingleton;
}
*/
	

