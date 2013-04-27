#ifndef __GEN__
#define __GEN__


/*=========== Jpeg config ===========*/
#define JWIDTH 160
#define JHEIGHT 120
#define JQUAL 75
/*===================================*/

/*============ Interprocess via pipe =============*/
#define VEND 255
#define VON 254
#define VOFF 253
#define VPORT 252
#define VIP 251
/*================================================*/

#define SPACE_TORSO 0
#define SPACE_WORLD 1
#define SPACE_NAO 2

#define IP_LEN 15
#define PORT_LEN 5
#define MB_IP "127.0.0.1"
#define MB_PORT 9559
#define CB_IP "127.0.0.1"
#define CB_PORT 50000

#define RM_PORT "32768"
#define RAD (2*3.1415926535897932384626/360.0)

#define SOCK_CLOSED -2
#define SOCK_LOST	-3
#define CONN_RESUME 1
#define CONN_ACTIVE 1
#define CONN_DISCONNECT 2
#define CONN_INACTIVE 2
#define CONN_UNBIND 3


#define IPARAM_LEN 2

/*=========== Decoding Stages ============*/
#define STG_ERROR 0
#define STG_FETCH 1
#define STG_PARAM 2
#define STG_VALID 3
/*========================================*/

/*============ Comand Specfier TOKENs ============*/
#define CODE_VALID -2
#define CODE_INVALID -1
#define CODE_UNKNOWN 0
#define CODE_ABSOLUT 10
#define INIT_WALK 10
#define INIT_REST 11
#define INIT_SIT 12
#define INIT_UP 13
#define INIT_WAVE 14
#define INIT_DANCE 15
#define INIT_WIPE 16
#define CODE_MOV 20
#define CODE_STOP 21
#define CODE_STOPALL 22
#define CODE_VID 23
#define RESET_CONNECTION 42
#define CODE_PARALLEL 1000
#define CODE_SPK 1001
#define CODE_HEAD 1003
#define CODE_ARM 1004
#define CODE_BAT 1100
#define CODE_STATE 1101
/*=================================================*/

#define MOV_STOP -1
#define MOV_FORWARD 1
#define MOV_BACKWARD 2
#define MOV_LEFT 3
#define MOV_RIGHT 4
#define ARM_LEFT 11
#define ARM_RIGHT 12

/*=========== Event Classification ===============*/
#define EVT_PENDING 1
#define EVT_PENDINGABS 2
#define EVT_PENDINGPAR 3
#define EVT_BUSY 4
#define EVT_DONE 5
#define EVT_CONFLICTED 6
/*================================================*/

typedef int state_t;

enum naoStates {
	STATE_ABSOLUT = 0x00FF,
	STATE_UNKNOWN = 0x0040,
	STATE_KNOWN   = 0x0080,
	STATE_CROUCHING = 0x0001,
	STATE_STANDING = 0x0002,
	STATE_SITTING = 0x0004,
	STATE_WALKING = 0x0008,
	STATE_STOPPING = 0x0010,
	STATE_MOVING = 0x0020,
	STATE_STOPPINGALL = 0x0040,
	STATE_STOPPED = 0x0080,
	//parallel states
	STATE_PARALLEL = 0xFF00,
	STATE_HEADMOVE = 0x8000,
	STATE_SPEAKING = 0x4000,
	STATE_ARMMOVE  = 0x2000,
	STATE_REFUSING,
	STATE_ALL = 0xFFFF 
};

enum{
	ORD_PAR = 0,
	ORD_ABS,
	ORD_STRICT
};

//with all automated collision avoiding this angle will be reduced
//to approx 23° per 2 steps
#define TURN_ANGLE 90 // 18
//therefor 30 steps are neede total to turn 360
#define TURN_STEPS 2*15

#define HEAD_YAW 0.6
#define HEAD_PITCH 0.3
#define ARM_SHOULDER_PITCH 45.0f
#define ARM_SHOULDER_ROLL 35.0f




#endif
