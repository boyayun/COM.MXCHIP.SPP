#ifndef _MICO_TCL_OTA_H_
#define _MICO_TCL_OTA_H_

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */


#include "geniot.h"
#include "mico_socket.h"
#define DEVICE_TYPE "0305"
#define LEN_OF_RECE_360 31
#define LEN_OF_CONR_360 21
#define LEN_OF_CONR_WATER 30
#define LEN_OF_RECE_YUBA 39

#define LEN_OF_TCL_DEVICE   72


#define uint32  uint32_t
#define uint16  uint16_t
#define uint8				uint8_t

#define LEN_OF_AIR_CONDITION_CONTROL   31

#define P_UART_F300   8

#define JOYLINK_ON   0
 
#define STATUS_UART_LEN   16

#define iot_uart_output(x,y)  MicoUartSend(UART_FOR_APP,x,y)

#define LOC_UDP_PORT  8670
#define ROUT_UDP_PORT  8671

//#define PP 3					//���ڵ�½����˵���óɹ�
//#define SA 4					//��¼�ɹ�
//#define AP 5					//��Զ�̿���
//#define CH 2				//�̼�ͨ��
//#define CF 1					//����״̬
//#define OF 6					//������


#define CF    0x01  //  ����״̬��ģ��δ����
#define PP    0x03  //���óɹ������ڵ�½
#define SA    0X04  //	��½�ɹ��������Ͼ�����
#define AP    0x05   //wifi���ӵ��ƶˣ������ɿ�

#define ALAM_STOP   (0X4002)   //�豸����

#define MODELTYPE  												0X01
#define PROTOCOLVEL    								0X01
#define SOFTWAREVER  										0X1601230a
#define HAREWAREVER												0X1601230a
#define ENCRYPTEDTYPE											0x0
#define RESERVE																		0x0
#define ENCRYPTEDPARAMETER 				0x0


#define DEFINE_DISCOVERY_QUESTION 0x1001
#define DEFINE_DISCOVERY_ANSWER			 0x1002
#define LINK_ORDER_BUNDING         0x200c
#define LINK_BUNDING_ACK											0x200d

#define SOFTWAREVERSION_L						0x20160108
#define SOFTWAREVERSION_H						0x0000


#define DEVICE_NAME    "TCL_F300"




typedef struct 
{
unsigned short head;
unsigned char  ctrl;
unsigned char  len;

}TCL_UART_HEAD;



typedef enum
{			
				DEVICE_NONE,
    PURIFIER_220B,
    PURIFIER_4_IN_1,
    YB_1111,

} Device_type;


typedef struct 
{
	unsigned int sn;//���к�
	unsigned int loc_sn;
	char uart_change;
	Device_type device_type;
 char deviceid[IDLENGTH];     //���ܿ�������ID
 char gatewayid[IDLENGTH];     //���ܿ�������ID
 unsigned int deviceNetid;  //Ϊ�¼����豸�����NetID
 unsigned int gatewayNetid;  //Ϊ�¼����豸�����NetID
 		unsigned int							dnetid;						//Ŀ��netid
		unsigned int							snetid;						//Դnetid
		unsigned short     heart_count;
		unsigned short flag;
 struct sockaddr_t  addr;

}USER_CONFIG;

typedef struct 
{
unsigned short funNum;;
unsigned short VarIndex;
//unsigned char 		vaule;

}IotDataHex;

typedef struct 
{
unsigned short funNum;;
unsigned short VarIndex;
char 		vaule[IDLENGTH];

}IotDataStr;


#define POWER							31
#define CON_AUTO   5005
#define CON_JIASHI 5022
#define CON_AIR_LV 608
#define CON_TIME  5023
#define CON_AIR_QUR 5024
#define CON_TEMP 611
#define QUERY_ALL  11
#define TYPEID      0x0D

#define UART_RECE_STATUS_ING  1
#define UART_RECE_STATUS_STA  2
#define UART_RECE_STATUS_END  0

typedef struct 
{
unsigned char rece_flag;		//��ʾ�Ƿ���յ�����Ч����0��ʾ����Э�黹δ�Խӣ�1��ʾ����Э���Ѿ��Խ�
unsigned char flag;
unsigned short status;

unsigned char mybuf_len;
unsigned char date_len;
unsigned char rece_status;//���ݽ���״̬  1��ʾһ�����ݰ������У�0����ʾ�������
unsigned char f5_len;
}G_USER_UART_STATUS;

typedef struct
{
unsigned short version;
unsigned short reserve;
unsigned int   net;
 char deviceid[IDLENGTH];

}IotDeviceFind;


typedef struct
{
 char deviceid[IDLENGTH];
unsigned int ProtocolVersion[2];
unsigned int sw_ver[2];
unsigned int hw_ver[2];
char devicename[64];


}IotDeviceFindAck;


#define FRAME_CONTROL 								0X01		//����֡
#define FRAME_STATUS  								0X02   //״̬֡
#define FRAME_INVALID 								0X03   //��Ч֡
#define FRAME_WARN   								 0X04			// ����֡
#define FRAME_COFIRMATION    0X05			// ȷ��֡

#define FRAME_UPLOAD 									0x07

#define FRAME_STOP_WARN      0X09
#define FRAME_READ_ID									0x10
#define FRAME_READ_ACK								0X11
#define FRAME_RESET										 0Xac
#define FRAME_TYPEID										0x0D
#define FRAME_WIFI_STATUS    0XAD































































































/********************���¶����Ϊ��ˮ����Ŀ��***************************/

typedef struct
{
unsigned int pro_ver;
unsigned short len;
unsigned short cmd;
unsigned int socket_id;

}IotWaterHead;

typedef struct
{
	unsigned int pro_ver;
	unsigned char resver;
	unsigned char len;
	unsigned short cmd;
}IotWaterHead_status;

typedef struct
{
	unsigned int head_pro_ver;
	unsigned char resver;
	unsigned char len;
	unsigned short head_cmd;

unsigned char head;
unsigned char pro_ver_h;
unsigned char pro_ver_l;
unsigned char cmd;

unsigned char switcH;
unsigned char model;
unsigned char power;
unsigned char set_temper;

unsigned char status;
unsigned char time_water_h_1;
unsigned char time_water_m_1;
unsigned char time_water_h_2;

unsigned char time_water_m_2;
unsigned char hot_water;
unsigned char water_temper;
unsigned char error_code;


}IotWaterAll_status;

typedef struct 
{
unsigned int pro_ver;

unsigned short len;
unsigned short cmd;

unsigned int socket_id;

unsigned char head;
unsigned char version_h;
unsigned char version_l;
unsigned char type;

unsigned char switcH;
unsigned char model;
unsigned char power;
unsigned char set_temper;

unsigned char time_water_h_1;
unsigned char time_water_m_1;
unsigned char time_water_h_2;
unsigned char time_water_m_2;

unsigned char set_time_h;
unsigned char set_time_m;
unsigned char reserve1;
unsigned char reserve2;

}IotWaterAll_Cmd;

typedef struct
{
unsigned char head;
unsigned char pro_ver_h;
unsigned char pro_ver_l;
unsigned char cmd;

unsigned char switcH;
unsigned char model;
unsigned char power;
unsigned char set_temper;

unsigned char status;
unsigned char time_water_h_1;
unsigned char time_water_m_1;
unsigned char time_water_h_2;

unsigned char time_water_m_2;
unsigned char hot_water;
unsigned char water_temper;
unsigned char error_code;


}IotWaterStatus;


typedef struct 

	{
	unsigned char switcH;
	unsigned char time_h;
	unsigned char time_m;
	unsigned char time_s;
	
	unsigned char water_level;
	unsigned char current_temper_h;
	unsigned char current_temper_l;
	unsigned char model;

	unsigned short set_temper;
	unsigned char power;
	unsigned char fix_time;

	unsigned short countdown;
	unsigned char crc_h;
	unsigned char crc_l;


}IotTclWaterStatus;

#define WATER_CMD  														0X0305
#define WATER_DEVICE_INFO 					 0X0307
#define WATER_RESET 													0X0308

#define UART_LEN_WATER_CONDITION   0x18


#define SET_TEMPER          0X0353
#define FIX_TIME												0X0354
#define SET_TIME            0X0027
#define SET_MODEL											0X0359










/**********************end************************************************************/









#define WCZ_LOG    0  

#define STR_OR_0X  1  //0��ʾΪ16���ƣ�1��ʾΪ�ַ���
/**************�������������ֻ�ܿ�һ��������ȫ�ر�*******************/
#define EN_BASE64_TO_F5F5  0 //  1��ʾģ�����Э��ת�����ԽӾ�������0��ʾ����Э��ת������������Э����
#define TCL_WATER_CONDITION  0 //  1��ʾģ�����Э��ת�����Խӿյ���0��ʾ����Э��ת������������Э����
#define TCL_WATER_PURIFIER  0 //  1��ʾģ�����Э��ת�����ԽӾ�ˮ����0��ʾ����Э��ת������������Э����
/*****************************************************************************************/
#define BIG_LITTLE    1   //  1   ��ʾΪ��ˣ�0 ��ʾΪС�� micoϵͳ��С��

#define BASE64_ALL 1//ȫ����base64����

#define ADD_FRAME_TYPE   0  //  1��ʾapp���͵�ʱ�����֡���ͣ�0��ʾ������Ŀǰ�ǲ�����
#define TEST_0X01_25   0  //Э��app�����������
#define  CRC_ADD_F5_55  1
#define FILTER_F5_55   1
#define UART_DATE_OLD_NEW   1  //  1��ʾ���µĴ��ڻ�ȡ����

#ifdef __cplusplus
}
#endif

#endif


