#ifndef __DECODER__
#define __DECODER__

#include <alvalue/alvalue.h>
#include <alerror/alerror.h>
#include <alcommon/alproxy.h>

#include "gen.h"
#include "eventlist.h"
#include "netNao.h"


/**
\brief Arguments for the timer thread Decoder::timer(void* args).
*/
struct timer_arg
{
	pthread_t id; ///< id of the thread
	int tnum; ///< thread number, identifies allocated space in an array for this structure.
	boost::shared_ptr<int> bat_count; ///< pointer to the count of received BAT messages 
	boost::shared_ptr<NetNao> net; ///< pointer to the NetNao module
};

/**
\brief Decoder: Parsing -- Timeouts -- Managing the Camera module -- partly sending feedback to the App
*/
class Decoder 
{
	private:
		int pipeWrite; ///< writing end of the pipe to the Camera process
		string ip4; ///< ip of nao
		string port; ///< port Control Process
		AL::ALProxy* pproxyManager;
		
		/**
		\brief First step of parsing: Looks for valid comand specifier.
		
		\param toParse next lexem to be analysed.
		\param pos current position in the parsed string, keep it updated.
		\param ep is filled with the identified comand specifier TOKEN.\n
		       The TOKENs are defined in gen.h
		\return true, when params are expected\n
				false, otherwise
		*/
		bool fetch(const char& toParse, int& pos, event_params_t& ep); 
		
		/**
		\brief Extracts the expected parameters to a given comand.
		
		\param toParse next lexem to be analysed.
		\param pos current position in the parsed string, keep it updated.
		\param ep is filled with the corresponding param values, if valid.\n
			   ep will change the code specifier TOKEN to CODE_INVALID if the parameter is invalid.
		\param paramCount Current count of scanned parameters
		\return true, when more parameters or more input for the current parameter is expected.\n
				false, when no further input is expected.
		*/
		bool getParams(const char& toParse, int& pos, event_params_t& ep, int& paramCount);
		
		/**
		\brief Write content of \a buf to \a writer
		*/		
		void writePipe(const int& writer, const char* buf, const int& len);
		
		/**
		\brief send remaining battery power to Android App
		
		This function uses a proxy to NetNao in broker NET_BROKER
		*/
		void sendBatteryStatus(boost::shared_ptr<NetNao> net);
		
		/**
		\brief send installed behaviors to the Android App
		
		This function uses a proxy to NetNao in broker NET_BROKER
		*/
		void sendBehaviours(boost::shared_ptr<NetNao> net);
	public:
		void setIp4(const string& ip);
		void setPort(const string& iport);
		void setPipe(const int& pw);
		void setManager(AL::ALProxy* ppm);
		
		///< Timer thread, timing out the connection after 20 seconds
		static void* timer(void* args);
		
		Decoder()
		{
			pipeWrite = -1;
			pproxyManager = 0;
		}
		~Decoder(){}
		
		/**
		\brief decode a command, done only in 1 character steps
		
		This function uses #fetch() and  #getParams().
		
		\param toParse next lexem to be analysed.
		\param ep ep.type will be != CODE_INVALID or CODE_UNKNOWN if successful
		
		\return can be CODE_UNKNOWN, CODE_VALID, CODE_INVALID 
		*/
		int decode(const char& toParse, event_params_t* ep);
		
		/**
		\brief After successfully decoding a comand it will be added to the Manager Eventlist via Manager::addCom
			   or already executed, if easy enough.
		*/
		int manage(event_params_t* ep, boost::shared_ptr<NetNao> net, int& bat_count);
};

#endif
