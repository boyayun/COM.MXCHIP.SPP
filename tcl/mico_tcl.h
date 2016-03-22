#ifndef _MICO_TCL_H_
#define _MICO_TCL_H_

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */


#define uint32  uint32_t
#define uint16  uint16_t
#define uint8				uint8_t

#define MAXUSERS		12

#define JSONLENGTH 	84
#define SVERSION  1						//Ŀǰ��V1.1���������汾
#define THISVERSION  	21

#define USEPASS  1


#define MODEL_TYPE 		"YB7681"
#define WIFIMM			 "WTSC-QL01_VERAP02"			//wifi_module_model
//#define WIFI_NAME "VENM5pm66IO95rW06Zy4"		//name			TCL����ԡ��VENM56m65rCU5YeA5YyW5Zmo  
#define WIFI_NAME "VENM56m65rCU5YeA5YyW5Zmo"		//name			TCL����ԡ��VENM56m65rCU5YeA5YyW5Zmo  
#define WIFI_MV  "0100010003"		//master_vendor
#define DEVICE_ID "020201"		//device_type_id
#define DEVICE_MODEL "5"			//device_model



struct users
{
	int flag;
	char from[JSONLENGTH];
	uint32 ip;
	uint16 port;
	uint32 time;
};

struct MYCONFIGS
{
	//int smartconfigflag;

	int real;


	int 		serverflag;
	uint8 	remoteserver[4];

	int 		statusst;
	uint8 	random[4];

	uint32 	netconfigip;

	int 		configmode;

	uint8 test_module;		//wcz add ����ģ����Ա��λ
	uint8 test;				//

	uint8	first_register;

	uint8	szcity[21];		
	int 		ncity;			//���д���

	int 		mdconnect;		//����ʱ����moduletest��moduletest1�Ĳ���
	uint8 wifi_smart;
};


struct MYGLOBALCON
 {
	int stid;

#if SWAP_SERVER_IP
	int msg_id;
#endif

	
	int ssmartconfigtimer;
	
	int sendtimer;
	int timercounter;

	//char msg[512];
	int    msglen;
	int    starter;

	uint32  broadcastslap;

	int ledflag;

	int poweron;


	int resendflag;
	int resendcounter;
	int resendstid;
	uint32 resendtimeslapse;

	uint8 random;
	uint8 idtype;
#if SWAP_SERVER_IP
	uint8 reply_flag;
uint8 swap_flag;
#endif

	uint8 devicetype;			//�豸�ͺ�

	uint8 sync_flag;			//��λ���		
	uint32	sync_time;		//��λ��ʱ��
	uint32 	sync_sendtime;	//���͸�λ��ʱ��

	uint32 cloudhart;
	uint32 cloudstatus;

	uint32 networkstatus;

	#if SWAP_SERVER_IP
	uint32 reply_time;
	uint32 disconnect_time;//15���һ�Σ�
	uint32 get_second_time;		//ÿ��45��ȥ��FOTA�������ñ��÷�������IP��ַ��ֱ���õ�Ϊֹ
	#endif

	uint32 resendtimer;

	int 	first_registercount;
	uint32 	first_registertimer;

	uint32	powerontimer;
 };

#ifdef __cplusplus
}
#endif
#endif


