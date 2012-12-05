#ifndef __DECODER__
#define __DECODER__

#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
//#include <boost/enable_shared_from_this.hpp>
#include <string>

using namespace std;

class Decoder:public AL::ALModule
{
	private:
		struct null_deleter{
			void operator()(void const*) const{}
		};
	public:
		//always get a shared_ptr from this via lock()	
		
		Decoder(boost::shared_ptr<AL::ALBroker> broker, const string& name);
		virtual ~Decoder();
		virtual void init();
		void decoderRespond();		
};

#endif
