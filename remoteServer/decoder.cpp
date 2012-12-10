#include <iostream>
#include <alproxies/altexttospeechproxy.h>

#include "decoder.h"
#include "gen.h"

using namespace std;
	
Decoder::Decoder(boost::shared_ptr<AL::ALBroker> broker, const string& name)
: AL::ALModule(broker, name)
{
	setModuleDescription("This Module manages the Data comming from the remoteServer module");
	
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
	AL::ALTextToSpeechProxy tts(MB_IP, MB_PORT);
			tts.say(string("Decoder"));
}



