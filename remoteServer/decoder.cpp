#include "decoder.h"
#include "eventlist.h"
#include "gen.h"

#include <alproxies/almemoryproxy.h>
#include <alproxies/albatteryproxy.h>
#include <alproxies/dcmproxy.h>
#include <alproxies/albehaviormanagerproxy.h>
#include <qi/os.hpp>
#include <string>
#include <sstream>      // std::stringstream
#include <iostream>
#include <ctype.h>


#include <time.h>

using namespace std;


void* Decoder::timer(void* args)
{
	int oldstate;
	int oldtype;
	
	time_t told = time(0);
	time_t tnew = told;
	bool newConn = true;
		 
	struct timer_arg* aargs = (struct timer_arg*)args;
	boost::shared_ptr<int>& bat_count = aargs->bat_count;
	boost::shared_ptr<NetNao>& net = aargs->net;
	
	
	
	if(pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype) != 0)	
		cerr<< "[Executer]<execute> pthread_setcanceltype failed" << endl;
	if(pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldstate) != 0)
		cerr<< "[Executer]<execute> pthread_setcancelstate failed" << endl;
	
	while(1)
	{
		qi::os::sleep(1);
		//cout<< "[Decoder]<timer>:" << tnew - told << ", bat_count = " << *bat_count << endl;
		tnew = time(0);
		if(net->getMode() == CONN_ACTIVE)
		{
			if(newConn)
			{
				cout<< "[Decoder]<timer> Neue Verbindung" << endl;
				told = time(0);
				tnew = told;
				newConn = false;
			}
			if((tnew - told > 20))
			{
				if (*bat_count < 1)
				{
					net->ckill(net->getClient_tcp());
				}
				else 
				{
					told = time(0);
					*bat_count = 0;
				}
			}
		}
		else
		{
			if(!newConn)
				cout<< "[Decoder]<timer> Verbindung getrennt" << endl;
			newConn = true;
			*bat_count = 0;
		}
	}
	
	
}


void Decoder::setIp4(const string& ip)
{
	ip4 = ip;
}

void Decoder::setPort(const string& iport)
{
	port = iport;
}

void Decoder::setPipe(const int& pw)
{
	pipeWrite = pw;
}


void Decoder::setManager(AL::ALProxy* ppm)
{
	pproxyManager = ppm;
}

	
void Decoder::writePipe(const int& writer, const char* buf, const int& len)
{
	if (writer >= 0)
		write(writer, buf, len);
	else if (pipeWrite >= 0)
		write(pipeWrite, buf, len);
}

bool Decoder::fetch(const char& toParse, int& pos, event_params_t& ep)
{
	static string fstr = "";
	if (pos <= 2)
	{
		fstr+=toParse;
		++pos;
	}
	
	cout << "BEFEHL[" << fstr << "]" << endl;
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
			ep.iparams[0] = MOV_STOP;
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
		else if (fstr.compare("GEN") == 0)
		{
			ep.type = CODE_GEN;
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
		else if (fstr.compare("EXE") == 0)
			ep.type = CODE_EXE;
		else 
			ep.type = CODE_INVALID;
			
		if (ep.type == CODE_INVALID)
		{
			fstr = fstr.substr(1);
			pos = 2;
		}
		else	
		{
			fstr = "";	
		}
	}
	//return <parse for process params>
	return true;
}



bool Decoder::getParams(const char& toParse, int& pos, event_params_t& ep, int& paramCount)
{
	switch (ep.type)
	{
		case CODE_SPK:
		{
			//cout<< "getParams:" << toParse[pos] << endl;
			if ((toParse != '_') && (paramCount==1))
			{
				ep.sparam+=toParse;
				//cout<< "getParams:		" << ep.sparam << endl;
			}
			else if (toParse == '_')
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
				pos = 0;
				ep.type = CODE_INVALID;
				return false;
			}			
			++pos;
			break;
		}
		case CODE_EXE:
		{
			//cout<< "getParams:" << toParse[pos] << endl;
			if ((toParse != '_') && (paramCount==1))
			{
				ep.sparam+=toParse;
				//cout<< "getParams:		" << ep.sparam << endl;
			}
			else if (toParse == '_')
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
				pos = 0;
				ep.type = CODE_INVALID;
				return false;
			}			
			++pos;
			break;
		}
		case CODE_MOV:	
			if (paramCount == 1)
			{
				switch (toParse)
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
					case '_':
						break;
					default:
						pos = 0;
						ep.type = CODE_INVALID;
						return false;
						break;
				};
				++pos;
			}
			else if (paramCount == 2)
			{
				if (toParse != '_')
				{
					ep.sparam+=toParse;
					cout<< "getParams:		" << ep.sparam << endl;
				}
				++pos;
			}
			
			if (toParse == '_')
			{
				++paramCount;
				++pos;
				if (paramCount >= 3)
				{
					int l = 0;
					float val = 0.0f;
					stringstream parsval;
					parsval.clear();
					parsval<< ep.sparam;
					parsval>> val;
					if (!parsval.fail())
					{
						ep.fparam = val;
						cout<< "[Decoder]Die empfangene Zahl ist " << val << endl;
					}
					else
					{
						pos = 0;
						ep.type = CODE_INVALID;
						cout<< "[Decoder]Der empfangene Parameter is invalid (expected float)" << endl;
					}
					return false;
				}
			}			
			break;
		case CODE_HEAD:
			if (paramCount == 1)
			{
				switch (toParse)
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
					case '_':
						break;
					default:
						pos = 0;
						ep.type = CODE_INVALID;
						return false;
						break;
				};
				++pos;
			}
			else if (paramCount == 2)
			{
				if (toParse != '_')
				{
					ep.sparam+=toParse;
					cout<< "getParams:		" << ep.sparam << endl;
				}
				++pos;
			}
			
			if (toParse == '_')
			{
				++paramCount;
				++pos;
				if (paramCount >= 3)
				{
					int l = 0;
					float val = 0.0f;
					stringstream parsval;
					parsval.clear();
					parsval<< ep.sparam;
					parsval>> val;
					if (!parsval.fail())
					{
						ep.fparam = val;
						cout<< "[Decoder]Die empfangene Zahl ist " << val << endl;
					}
					else
					{
						pos = 0;
						ep.type = CODE_INVALID;
						cout<< "[Decoder]Der empfangene Parameter is invalid (expected float)" << endl;
					}
					return false;
				}
			}			
			break;
		case CODE_ARM:
		
			if (toParse == '_')
			{
				++paramCount;
				++pos;
			}	
			else if (paramCount == 1)
			{
				switch (toParse)
				{
					case 'L':
						ep.iparams[0] = ARM_LEFT;
						break;
					case 'R':
						ep.iparams[0] = ARM_RIGHT;
						break;
					default:
						pos = 0;
						ep.type = CODE_INVALID;
						return false;
						break;
				};
				++pos;
			}
			else if (paramCount == 2)
			{
				switch (toParse)
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
						pos = 0;
						ep.type = CODE_INVALID;
						return false;
						break;
				};
				++pos;
			}
			else if (paramCount == 3)
			{
				if (toParse != '_')
				{
					ep.sparam+=toParse;
					cout<< "getParams:		" << ep.sparam << endl;
				}
				++pos;
			
			}
			else if (paramCount >= 4)
			{
				int l = 0;
				float val = 0.0f;
				stringstream parsval;
				parsval.clear();
				parsval<< ep.sparam;
				parsval>> val;
				if (!parsval.fail())
				{
					ep.fparam = val;
					cout<< "[Decoder]Die empfangene Zahl ist " << val << endl;
				}
				else
				{
					pos = 0;
					ep.type = CODE_INVALID;
					cout<< "[Decoder]Der empfangene Parameter is invalid (expected float)" << endl;
				}
				return false;
			}
					
			break;
			
		case CODE_VID:					
			if (toParse == '_')
			{
				++paramCount;
				++pos;
				if (paramCount == 3)
					return false;
				else
					return true;
			}	
			
			if (paramCount == 1)
			{
				switch (toParse)
				{
					case 'A':
						ep.iparams[0] = VON;
						break;
					case 'D':
					{
						ep.iparams[0] = VOFF;		
						++pos;
						return false;
						break;
					}
					default:
						pos = 0;
						ep.type = CODE_INVALID;
						return false;
						break;
				};
				++pos;
			}
			else if (paramCount == 2)
			{
				if ((toParse != '_'))
				{
					ep.sparam+=toParse;
					cout<< "getParams:		" << ep.sparam << endl;
				}
				++pos;		
			}						
			break;
		default:
			pos = 0;
			ep.type = CODE_INVALID;
			return false;
			break;
	};
	return true;

}

int Decoder::decode(const char& toParse, event_params_t* ep)
{
	static bool cont = false;
	static int stage = STG_FETCH;
	static int paramCount = 0;
	static int pos = 0;
	static event_params_t eventp = EP_DEFAULT(CODE_UNKNOWN);
	cout<< "code-argument = " << toParse;
	cout<< "Symbol = " << eventp.type << ", Pos = " << pos << endl;
	
	//needs to end when UNKNOWN Command not fully given
	//needs to resume when not fully staged yet
	if 	(stage != STG_VALID)
	{
		switch (stage)
		{
			case STG_FETCH:	
				//cont == true, parameter are expected
				cont = fetch(toParse, pos, eventp);
				//As long the code is unknown fetch more
				if ((eventp.type != CODE_INVALID) && (eventp.type != CODE_UNKNOWN))
				{	
					//if !cont skip the scann for parameters
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
			cout<< "-------------->Decoded Function: " << (int&)eventp.type << ", " << eventp.sparam << endl;
		
			*ep = eventp;
		
			pos = 0;
			stage = STG_FETCH;
			paramCount = 0;
			cont = false;
			eventp.type = CODE_UNKNOWN;
			for (int p = 0; p < IPARAM_LEN; ++p)
				eventp.iparams[p] = 0;
			eventp.sparam = "";
			
			return CODE_VALID;
		
			break;
		}
		case STG_ERROR:
		{
			cout << "-------------->Error Invalid CODE!" << endl;
			stage = STG_FETCH;
			paramCount = 0;
			cont = false;
			eventp.type = CODE_UNKNOWN;
			for (int p = 0; p < IPARAM_LEN; ++p)
				eventp.iparams[p] = 0;
			eventp.sparam = "";
			return CODE_INVALID;
			break;
		}
		default:
			break;	
	};
	return CODE_UNKNOWN;

}


int Decoder::manage(event_params_t* ep, boost::shared_ptr<NetNao> net, int& bat_count)
{
	int ms = 100;
	switch (ep->type)
	{
		case CODE_INVALID:
			break;
		case CODE_VID:
		{
			char combuf[2] = {0,};
			
			if (pipeWrite < 0)
				return CONN_RESUME;
			combuf[0] = (char)ep->iparams[0]; 
			if (ep->iparams[0] == VON)
			{
				cout<< ">>>CAMConfig<<< Ip: " << ip4 << " Port: " << port << endl;
				port = ep->sparam;
				writePipe(pipeWrite, combuf, 1);
				combuf[0] = VIP;
				writePipe(pipeWrite, combuf, 1);
				writePipe(pipeWrite, ip4.c_str(), ip4.length() + 1);
				combuf[0] = VPORT;
				writePipe(pipeWrite, combuf, 1);
				writePipe(pipeWrite, port.c_str(), port.length() + 1);				
			}
			else if (ep->iparams[0] == VOFF)
			{
				writePipe(pipeWrite, combuf, 1);
			}
			break;
		}
		case RESET_CONNECTION:	
		{
			char combuf[2] = {VOFF, 0};		
			writePipe(pipeWrite, combuf, 1);		
			try
			{		
				
				pproxyManager->callVoid<int, int, int, float, string, int>
				("addCom", ep->type, ep->iparams[0], ep->iparams[1], ep->fparam, ep->sparam, 0);	
				qi::os::msleep(ms);
			}
			catch (const AL::ALError& e)
			{
				cout<< "ERROR[manage]: " << endl << e.what() << endl;
			}
			return CONN_DISCONNECT;
			break;
		}
		case CODE_BAT:
			bat_count++;
			sendBatteryStatus(net);
			break;
		case CODE_STATE:
			break;
		case CODE_STOPALL:
			try
			{		
				pproxyManager->callVoid<int, int, int, float, string, int>
				("addCom", ep->type, ep->iparams[0], ep->iparams[1], ep->fparam, ep->sparam, 0);	
				qi::os::msleep(ms);
			}
			catch (const AL::ALError& e)
			{
				cout<< "ERROR[manage]: " << endl << e.what() << endl;
			}			
			break;
		case CODE_GEN:
			sendBehaviours(net);
			break;
		default:
			try
			{		
				pproxyManager->callVoid<int, int, int, float, string, int>
				("addCom", ep->type, ep->iparams[0], ep->iparams[1], ep->fparam, ep->sparam, 1);	
				qi::os::msleep(ms);
			}
			catch (const AL::ALError& e)
			{
				cout<< "ERROR[manage]: " << endl << e.what() << endl;
			}			
			break;
	};
	return CONN_RESUME;
}



void Decoder::sendBatteryStatus(boost::shared_ptr<NetNao> net)
 {
 	int sclient;
	int value = 0;
	int sent= 0;
	string buf = "BAT_";
	char str[15];
	AL::ALMemoryProxy mem = AL::ALMemoryProxy(MB_IP, MB_PORT);
	
	try
	{
		AL::ALBatteryProxy pbat = AL::ALBatteryProxy(MB_IP, MB_PORT);
		AL::DCMProxy dcm = AL::DCMProxy(MB_IP, MB_PORT);
	
		value=(int)((float&)mem.getData("Device/SubDeviceList/Battery/Charge/Sensor/Value") * 100);
	
		sclient = net->getClient_tcp(); 
		
		buf = buf + (char)(value) + (char)(255);
		cout << "[Decoder]<sendBatteryStatus>: buf.length() = " << (int)buf.length() << endl;

		while (sent < buf.length())
		{
			sent += net->sendString(sclient, buf, buf.length(), 0);
		}
	}
	catch (const AL::ALError& e)
	{
		cout<< "ERROR [Decoder]<sendBatteryStatus>:" << endl << e.what() << endl;
	}
 }


void Decoder::sendBehaviours(boost::shared_ptr<NetNao> net)
{
		AL::ALBehaviorManagerProxy pbehav(MB_IP, MB_PORT);
		AL::ALValue behav;
		string com = "GEN_";
		string curr = "";
		string name = "";
	 	int sclient;
		int size = 0;	
		int dlast = 0;
		int dots = 0;
		
		sclient = net->getClient_tcp();
		//behav = pbehav.getDefaultBehaviors();
		behav = pbehav.getInstalledBehaviors();
		size = behav.getSize();
		for (int i = 0; i < size; ++i)
		{
			curr = (string&)behav[i];
			dots = 0;
			for (int c = 0; c < curr.length(); ++c)
				if (curr[c] == '.')
				{
					++dots;
					if (dots == 2)
						dlast = c;
				}
			
			if (dots >= 2)
				name = curr.substr(dlast+1, curr.length() - (dlast+1));
			else 
				name = curr;
				
			if ((curr[0] != '.') && 
				(name.compare("stand") != 0) && 
				(name.compare("sit") != 0)	&& 
				(name.compare("hello") != 0) && 
				(name.compare("wipe") != 0)  && 
				(name.compare("stand") != 0) && 
				(name.compare("dance") != 0)) 
			{	
				com = "GEN_";
				com += name;
				com += "_";
				com += (char)(255);
				net->sendString(sclient, com, com.length(), 0);
			}
		}
		//behav = pbehav.getBehaviorNames();
}
