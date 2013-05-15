#ifndef __DECODER__
#define __DECODER__

#include <alvalue/alvalue.h>
#include <alerror/alerror.h>
#include <alcommon/alproxy.h>

#include "gen.h"
#include "eventlist.h"
#include "netNao.h"

struct timer_arg
{
	pthread_t id;
	int tnum;
	boost::shared_ptr<int> bat_count;
	boost::shared_ptr<NetNao> net;
};

class Decoder 
{
	private:
		int pipeWrite;
		string ip4;
		string port;
		AL::ALProxy* pproxyManager;
		bool fetch(const char& toParse, int& pos, event_params_t& ep);
		bool getParams(const char& toParse, int& pos, event_params_t& ep, int& paramCount);		
		void writePipe(const int& writer, const char* buf, const int& len);
		void sendBatteryStatus(boost::shared_ptr<NetNao> net);
		void sendBehaviours(boost::shared_ptr<NetNao> net);
	public:
		void setIp4(const string& ip);
		void setPort(const string& iport);
		void setPipe(const int& pw);
		void setManager(AL::ALProxy* ppm);
		static void* timer(void* args);
		
		Decoder()
		{
			pipeWrite = -1;
			pproxyManager = 0;
		}
		~Decoder(){}
		int decode(const char& toParse, event_params_t* ep);
		int manage(event_params_t* ep, boost::shared_ptr<NetNao> net, int& bat_count);
};

#endif
