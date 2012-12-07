#include <iostream>
#include "decoder.h"
#include <alproxies/altexttospeechproxy.h>

using namespace std;
	
Decoder::Decoder(boost::shared_ptr<AL::ALBroker> broker, const string& name)
: AL::ALModule(broker, name)
{
	setModuleDescription("This Module manages the Data comming from the remoteServer module");
	
	functionName("decoderRespond", getName(), "decoder Respond");
	BIND_METHOD(Decoder::decoderRespond);
	
	functionName("decoderRespond", getName(), "decoder Respond");
	BIND_METHOD(Decoder::decoderRespond);
}

Decoder::~Decoder()
{
}

void Decoder::init()
{

}

void Decoder::decoderRespond()
{
	AL::ALTextToSpeechProxy tts("127.0.0.1", 9559);
			tts.say(string("Decoder"));
}



