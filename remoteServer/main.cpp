#include <iostream>
#include <string.h>
#include <string>

#include <fcntl.h> 
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "netNao.h"
#include "manager.h"
#include "decoder.h"
#include "eventlist.h"
#include "gen.h"

#include <alcommon/almodule.h>
#include <alcommon/almodulecore.h>
#include <alcommon/albroker.h>
#include <alcommon/albrokermanager.h>
#include <alerror/alerror.h>
#include <alcommon/alproxy.h>
#include <alproxies/altexttospeechproxy.h>
#include <alproxies/alvideodeviceproxy.h>
#include <alvision/alvisiondefinitions.h>
#include <qi/os.hpp>

#include <jpeglib.h>


using namespace std;

#undef REAL
#define REAL
#ifndef REAL
	#define TEST
#endif



struct thread_arg
{
	pthread_t id;
	int tnum;
	char appIp[IP_LEN+1];
	unsigned short appPort;
	int sclient;
	AL::ALValue image;
	string nameId;
	AL::ALVideoDeviceProxy* proxyCam;
};
bool trun = false;


void* tcamSend(void* args);

int udp_create_client_socket(const char* port);
int udp_bind_host(const char* port);

int procControl(const string& pip, const int& pport, int* pipefd);
int procCam(const string& pip, const int& pport, int* pipefd);




int main(int argc, char* argv[])
{
	int pport = MB_PORT;
	string pip = MB_IP;
	bool validCon = false;
	bool enableCam = true;
	
	#ifdef TEST
	cout<< "[MAIN] TEST-Build" << endl;
	#endif
	#ifdef REAL
	cout<< "[MAIN] REAL-Build" << endl;
	#endif

//	if(argc > 2)
//	{
//		if(!string(argv[1]).compare("--pip"))
//		{
//			pip = argv[2];
//			if (argc > 4)
//			{
//				if(!string(argv[3]).compare("--pport"))
//				{
//					pport = atoi(argv[4]);
//				}
//			} 
//		}
//		else if (!string(argv[1]).compare("--pport"))
//		{
//			pport = atoi(argv[2]);
//			if (argc > 4)
//			{
//				if(!string(argv[3]).compare("--pip"))
//					pip = argv[4];
//			} 
//		}
//	}
//	else 
//	{
//		pport = MB_PORT;
//		pip = MB_IP;
//		enableCam = true;
//	}
	
	enableCam = true;
	if (argc > 1)
	{
		for (int a = 1; a<argc; ++a)
		{
			if (!string(argv[a]).compare("--nocam"))
				enableCam = false;
				
			if (!string(argv[a]).compare("--pip"))
			{
				if (a+1 < argc)
					pip = argv[a+1];
			}	
			
			if (!string(argv[a]).compare("--pport"))
			{
				if (a+1 < argc)
					pport = atoi(argv[a+1]);
			}
			
			if (!string(argv[a]).compare("--help") || !string(argv[a]).compare("-h") || 
			    !string(argv[a]).compare("--h") || !string(argv[a]).compare("-?"))
			{
				cout<< "Usage: remoteServer [--pip <ip>] [--pport <port>] [--nocam] [--help]" << endl
					<< "	>pip: Ip of the Parent Broker (default naoqi: 127.0.0.1)" << endl
					<< "	>pport: Port of the Parent Broker (default naoqi: 9559)" << endl
					<< "	>nocam: Deactivates Camera module for video streaming" <<  endl;
				return 0;
			}
		}
	}	
	
	cout<< "Zahl Argumente " << argc << endl
		<< "pip = " << pip << endl
		<< "pport = " << pport << endl;
	
	
	int pipefd[2] = {-1,};
	pid_t cpid;
	char buf;


	
	if (pipe(pipefd) == -1) 
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}
	
	cout<< "[MAIN] Leser = " << pipefd[0] << endl
		<< "[MAIN] Schreiber = " << pipefd[1] << endl;

	cpid = fork();
	if (cpid == -1) 
	{
		cerr<< "Fork failed!";
		exit(EXIT_FAILURE);
	}

	if (cpid == 0) 
	{

		if (enableCam)
		{
			cout<< "Camera module will be stated..." << endl;
			procCam(pip, pport, pipefd);
		}
		else
		{
			close(pipefd[0]);
			close(pipefd[1]);
			cout<< "Camera module won't be started..." << endl;
		}
		_exit(EXIT_SUCCESS);

	} 
	else 
	{       
		if(!enableCam)
		{
			close(pipefd[0]);
			close(pipefd[1]);
			pipefd[0] = -1;
			pipefd[1] = -1;		
		}
		cout<< "Control module will be started..." << endl;
		procControl(pip, pport, pipefd);
		wait(NULL);                /* Wait for child */

	}


	
	exit(0);
	

}

//==============================================================================
//===================== PROCCONTROL ============================================
//==============================================================================
int procControl(const string& pip, const int& pport, int* pipefd)
{	
	
	Decoder dec;
	struct timer_arg targ;
	int bat_count = 0;
	int pipeWrite = pipefd[1];
	if (pipefd[0] >= 0)
		close(pipefd[0]);
	

	//for SOAP serializations of floats
	//The call to setlocale is very important. Due to SOAP issues, you must make
	//sure your client and your server are using the same LC_NUMERIC settings
	setlocale(LC_NUMERIC, "C");	
	
	const string brokerName = "NET_BROKER";
	//assign open port via port 0 from os
	//will be the port of the newly created broker
	int brokerPort = 0;
	//ANYIP
	string brokerIp = "0.0.0.0";
	boost::shared_ptr<AL::ALBroker> broker;
	
	
	qi::os::sleep(2);
	try
	{
		broker = AL::ALBroker::createBroker(
			brokerName,
			brokerIp,
			brokerPort,
			pip,
			pport);
	}
	catch(...)
	{
		AL::ALBrokerManager::getInstance()->killAllBroker();
		//Reset the ALBrokerManager singleton
		AL::ALBrokerManager::kill();
	}
	
	cout<< "[MAIN] Broker Port >" << broker->getPort() << endl
		<< "[MAIN] Broker Ip >" << broker->getIP() << endl;
	
	brokerIp = broker->getIP();
	brokerPort = broker->getPort();
	
	//kills old BrokerManager Singleton and replaces it with a new one
	//fBrokerManager is weak pointer and converted to shared ptr via lock
	//lock additionally checks if there is one reference existing to AlBM
	AL::ALBrokerManager::setInstance(broker->fBrokerManager.lock());
	//Add Broker to the map!
	AL::ALBrokerManager::getInstance()->addBroker(broker);
	

	cout<< "[MAIN] Control-Server gestartet..." << endl
		<< "[MAIN] Schreiber = " << pipeWrite << endl;

	static boost::shared_ptr<NetNao> net = 
		AL::ALModule::createModule<NetNao>(broker, "RMNetNao");	
	
	//NetNao* net = new NetNao(broker, "NetNao");
	string port = RM_PORT;
	int taskID = 0;
	int sserver = 0;
	int sclient = 0;
	char buf[1024] = {0,};
	int bytesRead = 0; 
	int recvd = 0;
	int dresult = 0;
	int mresult = 0;
	event_params_t ep;
	
	// connect to local module RMManager
	
	cout << "[MAIN] Establish Proxy to RMManager"<< endl;
	AL::ALProxy proxyManager = AL::ALProxy(broker, string("RMManager")/*, pip, MB_PORT*/);
	cout << "[MAIN] Proxy to RMManager established" << endl;
				
	taskID = proxyManager.pCall(string("runExecuter")); //callVoid("runExecuter");
	cout<<"[MAIN] ID of Thread[runExecuter] = " << taskID << endl;
	proxyManager.callVoid<string, int>("setCB", brokerIp, brokerPort);
	
	
	targ.id = 0;
	targ.tnum = 0;
	targ.bat_count = boost::shared_ptr<int>(&bat_count);
	targ.net = net;
	
	pthread_create(&targ.id, 0, &Decoder::timer, &targ);	
	pthread_detach(targ.id);


	boost::shared_ptr<char*> buffer(new char*(buf));	
	sserver = net->bindTcp(port);
	cout<< "[MAIN] Server bound to port 32768" << endl;
	while(1)
	{
		net->singleListen(sserver);
		cout<< "[MAIN] Connection Request detected (listen ended)" << endl;
		sclient = net->acceptClient(sserver);
		proxyManager.callVoid<string>("initIp4", net->ip4);
		cout << "[MAIN] Connected from " << net->ip4 << endl;
		dec.setIp4(net->ip4);
		dec.setPipe(pipeWrite);
		dec.setManager(&proxyManager);
		cout<< "[MAIN] Client connected" << endl;
		
		try 
		{
			AL::ALTextToSpeechProxy tts(pip, pport);
			tts.say("Connected");
		}		
		catch(const AL::ALError& e)
		{
			cerr<< "[MAIN] EXCEPTION: " << e.what() << endl;
		}
		buf[0] = 0;
		cout<< "[MAIN] receive...\r\n";
		bytesRead = 0;
		do
		{
			recvd = net->recvData(sclient, buffer, 1, 0);
//			cout<< "[MAIN] Data received" << endl;
			if ((recvd == SOCK_CLOSED) || (recvd == SOCK_LOST))
			{
				string com = "DIS";
				for (int i = 0; i<com.length(); ++i)
					dec.decode(com[i], &ep);
				mresult = dec.manage(&ep, net, bat_count);	
//				com = "VID_D";
//				for (int i = 0; i<com.length(); ++i)
//					dec.decode(com[i], &ep);
//				mresult = dec.manage(&ep, net, bat_count);	
			}
			else
			{
				int i = 0;
				cout<< "[MAIN] received >" << string(buf) << endl;
				do 
				{
					dresult = dec.decode(buf[i], &ep);
					if (dresult == CODE_VALID)
						mresult = dec.manage(&ep, net, bat_count);		
					else 
						mresult = CONN_RESUME;
					++i;
				} while((i < recvd) && (mresult == CONN_RESUME));
			}
		}
		while ((recvd > 0) && (mresult == CONN_RESUME));
					
			
		if (recvd > 0)
		{		
			try 
			{
				AL::ALTextToSpeechProxy tts(pip, pport);
				tts.say("Disconnected");
			}
			catch(const AL::ALError& e)
			{
				cerr<< "[MAIN] EXCEPTION: " << e.what() << endl;
			}	
			net->disconnect(sclient);
			mresult = CONN_DISCONNECT;
		}
		else 
		{
			try 
			{
				AL::ALTextToSpeechProxy tts(pip, pport);
				tts.say("Connection lost");
			}
			catch(const AL::ALError& e)
			{
				cerr<< "[MAIN] EXCEPTION: " << e.what() << endl;
			}				
		}
	}
	
	
	net->unbind(sserver);
	//todo kill the proxy manager
	if (pipefd[1] >= 0)
		close(pipefd[1]);
	proxyManager.destroyConnection();
	broker->shutdown();
	//AL::ALBrokerManager::removeBroker(broker);

}
//==============================================================================
//===================== PROCCONTROL END ========================================
//==============================================================================

//==============================================================================
//===================== PROCCAM ================================================
//==============================================================================
int procCam(const string& pip, const int& pport, int* pipefd)
{

	int pipeRead = pipefd[0];
	close(pipefd[1]);
	//for SOAP serializations of floats
	//The call to setlocale is very important. Due to SOAP issues, you must make
	//sure your client and your server are using the same LC_NUMERIC settings
	
	
	setlocale(LC_NUMERIC, "C");	
	
	const string brokerName = "CAM_BROKER";
	//assign open port via port 0 from os
	//will be the port of the newly created broker
	int brokerPort = 0;
	//ANYIP
	const string brokerIp = "0.0.0.0";
	boost::shared_ptr<AL::ALBroker> broker;
	qi::os::sleep(4);
	try
	{
		broker = AL::ALBroker::createBroker(
			brokerName,
			brokerIp,
			brokerPort,
			pip,
			pport);
	}
	catch(...)
	{
		AL::ALBrokerManager::getInstance()->killAllBroker();
		//Reset the ALBrokerManager singleton
		AL::ALBrokerManager::kill();
	}
	
	

	//kills old BrokerManager Singleton and replaces it with a new one
	//fBrokerManager is weak pointer and converted to shared ptr via lock
	//lock additionally checks if there is one reference existing to AlBM
	AL::ALBrokerManager::setInstance(broker->fBrokerManager.lock());
	//Add Broker to the map!
	AL::ALBrokerManager::getInstance()->addBroker(broker);

	cout<< "[CAM] Cam-Client gestartet..." <<endl
		<< "[CAM] Leser = " << pipeRead << endl;

	//================ VIDEO DEVICE INITIALIZATION ======================================	
	
#ifdef REAL
	
	AL::ALVideoDeviceProxy proxyCam(pip, pport);
	//AL::ALProxy proxyCam = AL::ALProxy(broker, string("ALVideoDevice"))
	
	
	
	// First you have to choose a name for your Vision Module
	string nameId = "camVM";

	// Then specify the resolution among : kQQVGA (160x120), kQVGA (320x240),
	// kVGA (640x480) or k4VGA (1280x960, only with the HD camera).
	// (Definitions are available in alvisiondefinitions.h)
	int resolution = AL::kQQVGA;

	// Then specify the color space desired among : kYuvColorSpace, kYUVColorSpace,
	// kYUV422InterlacedColorSpace, kRGBColorSpace, etc.
	// (Definitions are available in alvisiondefinitions.h)
	int colorSpace = AL::kRGBColorSpace;

	// Finally, select the minimal number of frames per second (fps) that your
	// vision module requires up to 30fps.
	int fps = 20;

	// You only have to call the "subscribe" function with those parameters and
	// ALVideoDevice will be in charge of driver initialisation and buffer's management.
	nameId = proxyCam.subscribe(nameId, resolution, colorSpace, fps);
	
	AL::ALValue image;
	image.arraySetSize(12);
	image = proxyCam.getImageRemote(nameId);
	
	int width = (int) image[0];
	int height = (int) image[1];
	int nbLayers = (int) image[2];
	int color = (int) image[3];
	// image[4] is the number of seconds, image[5] the number of microseconds
	long long timeStamp = (long long)((int)image[4])*1000000LL + (int)image[5];
	// You can get the pointer to the image data and its size
	const unsigned char* bmp =  (unsigned char*)(image[6].GetBinary()); //<--------
	int size = image[6].getSize();

//	cout<< "Bildweite: " << width << endl
//		<< "Bildhöhe: " << height << endl
//		<< "Anzahl Layer: " << nbLayers << endl
//		<< "Farbraum: " << color << endl
//		<< "Timestamp in microseconds: " << timeStamp << endl
//		<< "Datengröße: " << size << endl;
#endif
	//================ VIDEO DEVICE INITIALIZATION ======================================	
		
    //================= Initiate Connection ============================================================
    bool end = false;
    bool vIp = false;
    bool vPort = false;
    bool restart = false;
    bool stop = false;
    string appIp;
    unsigned short appPort = 0;
    unsigned char buf[256] = {0,};
    int s = 0;
    int f = 0;
    int c = 0;
    int state = STG_FETCH;
    int rcom = VEND;
    int res = 0;
    

    int sclient = 0;
	sclient = udp_create_client_socket("32772");
    	
	struct thread_arg targ;
	cout<< "[CAM] Starte CAM-Controler" << endl;
    while (!end) 
    {	
		   res =  read(pipeRead, &buf[f], 1);
		   if (res > 0)	
		   		cout << "[CAM] " << buf[f] << endl;
		   else 
		   {
		   		end = true;
		   		stop = true;
		   }   			   
			if (state == STG_FETCH)
			{
				switch(buf[f])
				{
					case VON:
						rcom = buf[f];
						restart = true;
						break;
					case VOFF:
						rcom = buf[f];
						stop = true;
						break;
					case VIP:
						rcom = buf[f];
						state = STG_PARAM;
						s = f;
						break;
					case VPORT:
						rcom = buf[f];
						state = STG_PARAM;
						s = f;
						break;
					case VEND:
						rcom = buf[f];
						end = true;
						break;
					default:
						break;
			
				};
			}
			
			if (state == STG_PARAM)
			{
				switch(rcom)
				{
					case VPORT:   
						c = -1;    
						do
						{
							++c;
							res = read(pipeRead, &buf[c], 1);

						} while ((buf[c] != 0) && res && (c <= PORT_LEN));
						if (c <= PORT_LEN)
						{
							vPort = true;
							appPort = atoi((char*)buf);
							cout<< "[CAM] Port für neue Bildübertragung = " << appPort << endl;
						}
						else 
						{
							vPort = false;
						}
						state = STG_FETCH;
						f = s = 0;
						break;
					case VIP: 
						c = -1;    
						appIp = "";
						do
						{
							++c;
							res = read(pipeRead, &buf[c], 1);

						} while ((buf[c] != 0) && res && (c <= IP_LEN));
						if (c <= IP_LEN)
						{
							vIp = true;
							appIp = (char*)buf;
							cout<< "[CAM] IP für neue Bildübertragung = " << appIp << endl;
						}
						else
						{
							vIp = false;
						}
						state = STG_FETCH;
						f = s = 0;
						break;
					default: 
						state = STG_FETCH;
						f = s = 0;
						break;
				};
			}
		
			cout<< "[CAM] CONFIG DONE" << endl;

		   if(stop)
		   {
				if (trun)
				{
					cout<< "[CAM] Warte auf Thread Terminierung ..." << endl;
					trun = false;
					pthread_join(targ.id, 0);
				}		   
		   }
		   
		   if (restart && vIp && vPort)
		   {
		   		cout<< "[CAM] CAM ABOUT TO RESTART" << endl;
		   		targ.tnum = 0;
				cout<< "[CAM] tnum: " << targ.tnum <<endl;		   		
				strcpy(targ.appIp, appIp.c_str());
				cout<< "[CAM] appIp: " << targ.appIp <<endl;
				targ.appPort = appPort;
				cout<< "[CAM] appPort: " << targ.appPort <<endl;
				targ.sclient = sclient;    
				cout<< "[CAM] sclient: " << targ.sclient <<endl;
				
#ifdef REAL
				targ.image = image;		//<------
				cout<< "[CAM] image: " <<endl;
				targ.nameId = nameId;	//<------
				cout<< "[CAM] nameId: " << targ.nameId <<endl;
				targ.proxyCam = &proxyCam;
				cout<< "[CAM] proxyCam: " <<endl;
#endif
				if (trun)
				{
					cout<< "[CAM] Warte auf Thread Terminierung ..." << endl;
					trun = false;
					pthread_join(targ.id, 0);
				}
				
				trun = true;
				pthread_create(&targ.id, 0, &tcamSend, &targ);
					
				cout<< "[CAM] CAM RESTARTED" << endl;
				restart = false;
				vIp = false;
				vPort = false;	
				stop = false;					   
		   }
		    
    }	  
    
	cout<< "[CAM] Beende CAM-Controler" << endl;
	if (trun)
	{
		trun = false;
		pthread_join(targ.id, 0);	
	}
	
    //================= Initiate Connection ============================================================



	close(sclient);
	close(pipefd[0]);
		
#ifdef REAL		
	proxyCam.unsubscribe(nameId);
#endif





	broker->shutdown();
	//AL::ALBrokerManager::removeBroker(broker);
}
//==============================================================================
//===================== PROCCAM END ============================================
//==============================================================================

int udp_create_client_socket(const char* port)
{
	int status = 0;
	struct addrinfo hints;
	struct addrinfo *servinfo;
	int optval = 1;
	int sclient;
	
	memset (&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	
	status = getaddrinfo(NULL, port, &hints, &servinfo);
	sclient = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	freeaddrinfo(servinfo);
	
	return sclient;
}

int udp_bind_host(const char* port)
{
	int status = 0;
	struct addrinfo hints;
	struct addrinfo *servinfo;
	int optval = 1;
	int sserver;
	
	memset (&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	
	status = getaddrinfo(NULL, port, &hints, &servinfo);
	sserver = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	setsockopt(sserver, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
	status = bind(sserver, servinfo->ai_addr, servinfo->ai_addrlen);	
	freeaddrinfo(servinfo);
	
	return sserver;
}


void* tcamSend(void* args)
{
	cout<< ">Neuer Cam Thread gestartet..." << endl;
	struct thread_arg* aarg = (struct thread_arg*)args;
	
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = (htons(aarg->appPort)); 
	inet_pton(AF_INET, aarg->appIp, &(server.sin_addr));
	
	int& sclient = aarg->sclient;
	AL::ALValue& image = aarg->image;
	string& nameId = aarg->nameId;
	int size = 0;
	
	
	unsigned char* bmp = 0;
	unsigned char* jpeg = 0;
	unsigned int len = JHEIGHT * JWIDTH * 3;

	FILE* src;
	FILE* dst;
	FILE* jmem; 
	int written = 0;
	int count = 0;
	int offset = 0;

	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr       jerr;
	JSAMPROW row_pointer;          /* pointer to a single row */

#ifdef TEST
	bmp = (unsigned char*)malloc(JHEIGHT * JWIDTH * 3);
#endif
	jpeg = (unsigned char*)malloc(JHEIGHT * JWIDTH * 3);
	
	jmem = fmemopen(jpeg, len, "wb");
	//jmem = fopen(argv[2], "wb");
	//jmem = fdopen(pipefd[1], "wb");

#ifdef TEST
	unsigned char turn[3] = {0,128,255};
#endif
	while(trun)
	{
#ifdef TEST
		turn[0]+=5;
		turn[1]+=3;
		turn[2]-=5;
		for (int i = 0; i< (JHEIGHT * JWIDTH * 3); i+=3)
			bmp[i] = turn[0];
		for (int i = 1; i< (JHEIGHT * JWIDTH * 3); i+=3)
			bmp[i] = turn[1];
		for (int i = 2; i< (JHEIGHT * JWIDTH * 3); i+=3)
			bmp[i] = turn[2];
#endif

#ifdef REAL
		image = aarg->proxyCam->getImageRemote(nameId); 		//<------
		bmp =  (unsigned char*)(image[6].GetBinary());	//<------
		size = image[6].getSize();						//<------
#endif
			
		cinfo.err = jpeg_std_error(&jerr);
		jpeg_create_compress(&cinfo);
		jpeg_stdio_dest(&cinfo, jmem);

		cinfo.image_width      = JWIDTH;
		cinfo.image_height     = JHEIGHT;
		cinfo.input_components = 3;
		cinfo.in_color_space   = JCS_RGB;

		jpeg_set_defaults(&cinfo);
		/*set the quality [0..100]  */
		jpeg_set_quality (&cinfo, JQUAL, true);
		jpeg_start_compress(&cinfo, true);

		count = 0;
		written = 0;
		int line = 0;
		while (cinfo.next_scanline < cinfo.image_height) 
		{
			line++;
			//printf("%d:%d\n",line, count);
			row_pointer = (JSAMPROW) &(bmp+offset)[cinfo.next_scanline*JWIDTH*3];
			count = jpeg_write_scanlines(&cinfo, &row_pointer, 1);
		}
		
		jpeg_finish_compress(&cinfo);
	
		fflush(jmem);
		len = ftell(jmem);
#ifdef TEST
		printf(">Size of the jpeg file is %d bytes\n", len);	
#endif
	
		//================ SEND JPEG ======================================
	
		sendto(sclient, jpeg, len, 0, (const sockaddr*)(&server), sizeof(struct sockaddr_storage));
		//================ SEND JPEG ======================================
		
#ifdef REAL
		aarg->proxyCam->releaseImage(nameId);//<------
#endif
		qi::os::msleep(50);
		rewind(jmem); 
	}
	
//	dst = fopen("test.jpg", "wb");
//	fwrite(jpeg, len, 1, dst);
//	fclose(dst);

#ifdef TEST
	free(bmp);
#endif
	free(jpeg);
	fclose(jmem);
	
}
