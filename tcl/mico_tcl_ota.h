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

//#define PP 3					//正在登陆或者说配置成功
//#define SA 4					//登录成功
//#define AP 5					//可远程控制
//#define CH 2				//商检通过
//#define CF 1					//配置状态
//#define OF 6					//掉线了


#define CF    0x01  //  配置状态，模块未配置
#define PP    0x03  //配置成功，正在登陆
#define SA    0X04  //	登陆成功，连接上局域网
#define AP    0x05   //wifi连接到云端，外网可控

#define ALAM_STOP   (0X4002)   //设备报警

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
	unsigned int sn;//序列号
	unsigned int loc_sn;
	char uart_change;
	Device_type device_type;
 char deviceid[IDLENGTH];     //智能控制中心ID
 char gatewayid[IDLENGTH];     //智能控制中心ID
 unsigned int deviceNetid;  //为新加入设备分配的NetID
 unsigned int gatewayNetid;  //为新加入设备分配的NetID
 		unsigned int							dnetid;						//目标netid
		unsigned int							snetid;						//源netid
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
unsigned char rece_flag;		//表示是否接收到过有效数据0表示串口协议还未对接，1表示串口协议已经对接
unsigned char flag;
unsigned short status;

unsigned char mybuf_len;
unsigned char date_len;
unsigned char rece_status;//数据接收状态  1表示一个数据包接收中，0，表示接收完成
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


#define FRAME_CONTROL 								0X01		//控制帧
#define FRAME_STATUS  								0X02   //状态帧
#define FRAME_INVALID 								0X03   //无效帧
#define FRAME_WARN   								 0X04			// 报警帧
#define FRAME_COFIRMATION    0X05			// 确认帧

#define FRAME_UPLOAD 									0x07

#define FRAME_STOP_WARN      0X09
#define FRAME_READ_ID									0x10
#define FRAME_READ_ACK								0X11
#define FRAME_RESET										 0Xac
#define FRAME_TYPEID										0x0D
#define FRAME_WIFI_STATUS    0XAD































































































/********************以下定义的为热水器项目宏***************************/

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

#define STR_OR_0X  1  //0表示为16进制，1表示为字符串
/**************以下三个宏最多只能开一个，或者全关闭*******************/
#define EN_BASE64_TO_F5F5  0 //  1表示模块进行协议转换，对接净化器，0表示不做协议转换，按照最新协议走
#define TCL_WATER_CONDITION  0 //  1表示模块进行协议转换，对接空调，0表示不做协议转换，按照最新协议走
#define TCL_WATER_PURIFIER  0 //  1表示模块进行协议转换，对接净水器，0表示不做协议转换，按照最新协议走
/*****************************************************************************************/
#define BIG_LITTLE    1   //  1   表示为大端，0 表示为小端 mico系统是小端

#define BASE64_ALL 1//全部用base64编码

#define ADD_FRAME_TYPE   0  //  1表示app发送的时候带上帧类型，0表示不带，目前是不带的
#define TEST_0X01_25   0  //协助app测试相关数据
#define  CRC_ADD_F5_55  1
#define FILTER_F5_55   1
#define UART_DATE_OLD_NEW   1  //  1表示用新的串口获取函数

#ifdef __cplusplus
}
#endif

#endif


