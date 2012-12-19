#ifndef __GEN__
#define __GEN__

#define MB_IP "127.0.0.1"
#define MB_PORT 9559
#define CB_IP "127.0.0.1"
#define CB_PORT 50000

#define RM_PORT "32768"
#define RAD (2*3.1415926535897932384626/360.0)


#define IPARAM_LEN 2


#define STG_ERROR 0
#define STG_FETCH 1
#define STG_PARAM 2
#define STG_VALID 3

#define RESET_CONNECTION 42

#define INIT_WALK 10
#define INIT_REST 11

#define CODE_UNKNOWN 0
#define CODE_INVALID -1
#define CODE_MOV 20
#define CODE_STOP 21
#define CODE_SPK 22
#define CODE_BAT 23

#define MOV_STOP -1
#define MOV_FORWARD 1
#define MOV_BACKWARD 2
#define MOV_LEFT 3
#define MOV_RIGHT 4

#define EVT_PENDING 1
#define EVT_BUSY 2
#define EVT_DONE 3
#define EVT_CONFLICTED 4

//with all automated collision avoiding this angle will be reduced
//to approx 23° perr 2 steps
#define TURN_ANGLE 18
//therefor 30 steps are neede total to turn 360
#define TURN_STEPS 2*15




#endif
