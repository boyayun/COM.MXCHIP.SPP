

/*

2015.12.02 by zhaoyunhua


修改记录：

2015.12.15  

1.增加了ota的版本和类型

2.增加了LOG开关


修改记录：

2015.12.18

完成了删除设备和心跳的函数


2016.01.23

修改了头部的netid长度，从2个字节修改为4字节。

DestinationID	Int	4	信息包接收方NetID；广播地址时该值为0xFFFFFFFF
SourceID	Int	4	信息包发送发NetID；未分配NetID时显示0x00000000


2016.01.24

	1. 将模块修改为大端传输

	2. 修改了封包的组织方式，全部base64， 去掉了协议头F5F5F5F5


*/



#ifndef GENIOT_H_
#define GENIOT_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL (0)
#endif








/*

2015-12-14

	不同的平台可适当修改下面的函数

*/


	#define IOTSTRCPY	strcpy
	#define IOTMEMCPY	memcpy
	#define IOTMEMSET	memset
	#define IOTPSRINTF	sprintf
	#define IOTPRINTF	printf
	#define IOTSTRLEN	strlen
	

	#define JSONLENGTH (72)

	#define MAXCONTROLLEN  (120)
	#define MAXSTATUSLEN	(200)




/*

2015-12-15

	LOG开关

*/
	#define IOTLOG			1
	#define LITTLEENDING	1

	/*
	log输出
	*/
	void iotLog(const char *s);

	unsigned int iot_ntohl(unsigned int n);
	unsigned int iot_htonl(unsigned int n);
	unsigned short iot_ntohs(unsigned short n);
	unsigned short iot_htons(unsigned short n);

	/*
	base64函数
	*/
	char * iotBase64encode( const char * bindataArray, int binlength, char * base64);
	int iotBase64decode( const char * base64, int base64len,  char * bindataArray);
 int iotBase64decode_str( const char * base64, unsigned char * bindata );

		/*
		2016-01-24 
		bindata 输入参数			为需要base64编码的二进制数据
		binlength  输入参数		为需要base64编码的二进制数据的长度
		base64		输出参数	为编码后数据
		base64len 输入参数	为编码后数据的缓冲区长度(base64len 必须大于binlength的4/3倍)

		返回值， 失败为0（base64len 长度太小）
			成功则返回编码后的长度（包含^和&字符）

			数据包使用从Ctrl域到data域在传输过程中进行Base64编码，
			然后再包头增加“^”，在包尾增加“&”形成一个最终的数据包
	*/

	int iotDataToBase64(const  char * bindata, int binlength, char * base64, int base64len);


	/*
		2016-01-24 
		base64  输入参数			为需要解码的base64字符串（包含^和&字符)
		bindata  输入参数		  缓冲区, 需要足够大，能够容纳解码的数据
		binlength   输入参数		缓冲区长度，不能小于字符串base64的长度

		返回值， 失败为0（base64len 长度太小）
			成功则返回二进制数据长度
	*/
	int iotBase64ToData(const char * base64,  char * bindata, int binlength);



/*

2015-12-15

	升级包数据大小，建议不要超过1024

*/
	#define OTABLOCKSIZE	 (512)

	#define IDLENGTH				(36)
	#define	INFOLENGTH			(48)


	typedef struct {

		unsigned short							ctrl;							//控制字
		unsigned short							informationType;		//信息包类型
		unsigned int							dnetid;						//目标netid
		unsigned int							snetid;						//源netid
		unsigned int				sn;							//序列号：
																		//1．每发送一包数据序列号加1；重复发送序列号不变；
																		//2．发送和返回的序列号必须相同；
																		//3．序列号循环使用；

		unsigned short			cmd;							//命令字
		unsigned short			len;							//描述消息体总数据长度，不包括包头信息长度。单位字节

	}IotHead;



/*

获取信息头，buf为从网络上接收的base64字符串

*/
IotHead *iotGetHead(const char *buf, int len);

unsigned short iotGetHeadCmd(const char *buf, int len);

void iotSetHead(char *buf, int len, unsigned short ctrl, unsigned short informationType, unsigned int dnetid, unsigned int snetid);

void iotSetHeadCmd(char *buf, int len, unsigned int sn, unsigned short	cmd, unsigned short datalen);


	//通讯模块在线升级
	typedef struct{
		
		unsigned int codeType;			//程序文件类型
		unsigned int codeVer;				//当前版本
										//char *codeFileData;	//程序文件有效数据, 最后2位为校验数据

		int codeTotal;			//程序文件总包数
		int codeNumber;		//下载程序文件当前包号,codeNumber=0时为第一包数据；

	}IotFota;
	
	//通讯模块在线升级确认

	typedef struct{
	
		int codeType;		//程序文件类型
		int codeVer;			//当前版本
		int codeNumber;   //接收到的升级文件当前包号
		
	}IotFotaAck;


	/*
		buf 为缓冲区长度,包含IotHead头部
		设置升级包类型和版本
	*/
	void iotSetOtaTypeVer(char *buf, int len, unsigned int codeType, unsigned int codeVer);

	/*
		buf 为缓冲区长度,包含了IotHead头部
		设置序号，总包数， 和升级数据
		返回升级包长度（长度不包含包头)
	*/
	int iotSetOta(char *buf, int len, int codenumber, int codeTotal,  char *data, int datalen);

	/*
		buf 为缓冲区长度,包含IotHead头部
		设置回应的序号，类型和版本
		返回 回应包 长度（长度不包含包头)
	*/
	int iotSetOtaAck(char *buf, int len, int codenumber, int codeType, int codeVer);


	/*
		从二进制数据中提取升级数据
		char *buf, int len   解析出来的二进制数据包及其长度

		datalen 输出参数， 升级块的长度
	*/
	char *iotGetOtaData(char *buf, int len, int *datalen, IotFota *fota);
	
	//添加智能设备, 目前叫绑定智能设备
	//智能控制中心将智能设备绑定的操作信息；该信息包为广播包；
	typedef struct{

		char deviceid[IDLENGTH];						//准备添加的设备ID
		unsigned int deviceNetid;		//为新加入设备分配的NetID
		unsigned int gatewayNetid;		//为新加入设备分配的NetID
		int			gatewayVer;					//网关的服务版本

	}IotAddDevice;

	
	//添加智能设备确认
	typedef struct{

		char deviceid[IDLENGTH];						//准备添加的设备ID
		unsigned int deviceNetid;		//为新加入设备分配的NetID
		unsigned int gatewayNetid;		//为新加入设备分配的NetID

	}IotAddDeviceAck;



	/*

		添加设备相关

	*/

	int iotSetAddDevice(IotAddDevice *d, char *deviceid, unsigned int deviceNetid, unsigned int gatewayNetID, int gatewayVer);
	
	IotAddDevice *iotGetAddDevice(const char *buf, int len);

	int iotAddDeviceAck(IotAddDeviceAck *d, char *deviceid, unsigned int deviceNetid, unsigned int gatewayNetID);

	//删除智能设备
	typedef struct{

		char deviceid[IDLENGTH];	
		unsigned int deviceNetid;		//为新加入设备分配的NetID
		int		reserve;

	}IotDelDevice;


	//删除智能设备确认
	typedef struct{

		char deviceid[IDLENGTH];	
		unsigned int		deviceNetid;
	unsigned	short reserve;

	}IotDelDeviceAck;

	//通讯模块强制复位
	typedef struct{

		char deviceid[IDLENGTH];	
		int reserve;

	}IotResetDevice;


	//state状态
#define MESSAGE_DATA_GOOD		(0xa001)
#define MESSAGE_DATA_TIMEOUT	(0xa002)
#define MESSAGE_DATA_INVALID		(0xa003)
#define MESSAGE_DATA_ERROR		(0xa004)

	//通信模块强制复位确认, 接收数据确认
	typedef struct{

		int state;
		int reserve;

	}IotDataAck;


	//心跳
	typedef struct
	{
		char deviceid[IDLENGTH];	
		char	gatewayid[IDLENGTH];	
		int  reserve;

	}IotHeartbeat;


//设备主动发起的心跳
	typedef struct 
		{
		unsigned int SoftWareVersion_L;
		unsigned int SoftWareVersion_H;
		unsigned int reserve;

		}IotLinkQuestion;
	//心跳应答
	typedef struct
	{
		//char deviceid[IDLENGTH];	
		//char	gatewayid[IDLENGTH];	
		int  reserve;
	}IotHear2Ack;
	typedef struct
	{
		//char deviceid[IDLENGTH];	
		//char	gatewayid[IDLENGTH];	
		int		swh;			//swh和swl一起表示模块版本 版本号高位
		int		swl;			//版本号低位
		int  reserve;

	}IotHeart2;
/*
2015-12-16

设置心跳的deviceid，gatewayid，并返回IotHeartbeat的大小

*/

	int iotSetHeartbeat(IotHeartbeat *d, char *deviceid, char *gatewayid);
	


	//心跳应答
	typedef struct
	{
		char deviceid[IDLENGTH];	
		char	gatewayid[IDLENGTH];	
		int		swl;			//swh和swl一起表示模块版本 版本号高位
		int		swh;			//版本号低位
		int  reserve;

	}IotHeartbeatAck;


	/*

	2015-12-21

	设置心跳应答的的deviceid，gatewayid，版本号， 并返回IotHeartbeatAck的大小

	*/
	int iotSetHeartbeatAck(IotHeartbeatAck *d, char *deviceid, char *gatewayid, int swh, int swl);


	//设备报警
	typedef struct
	{
		int reserve;
		unsigned int errorCode;

	}IotAlarm;


	//传输数据
	typedef struct
	{

		int reserve;
	//	unsigned short funNum;;
//unsigned short VarIndex;

	}IotData;


	//设备描述文件信息包
	typedef struct{

		int number; 

	}IotDeviceDes;
		

	//设备描述文件信息包数据
	typedef struct{
		int total;
		int number;
	}IotDeviceDesData;

	typedef struct {
//unsigned char ModelTpye;
//unsigned char ProtocolVer;
unsigned int SoftwareVer;
unsigned int HarewareVer;
unsigned char EncryptedType;
unsigned char reserve;
unsigned short EncryptedParameter;

}IotDeviceInfo;



#define TYPE_DISCOVERY (0x1000)					//在未组网使用的设备发现类信息包；
#define TYPE_MANGERY	(0x2000)					//智能家庭子网组网和管理类信息包；该类信息只在通讯模块与控制中心之间进行交换；
#define	TYPE_LINK (0x2000)			//通讯模块与智能家庭网关之间心跳链接类信息包
#define	TYPE_DATA (0x3000)		//智能控制中心与设备之间的信息交换类；该信息包是由控制中心发起的。并且在通讯模块与智能家庭之间的协议中不作处理，只完成数据的转发。
#define	TYPE_AMAL (0x4000)		//设备主动发送的报警类信息；该类信息是由设备发起，并且需要紧急处理的信息类。该类信息子网通讯协议不做处理；
#define	TYPE_REPORT (0x5000)	//设备主动发起的信息；该类信息子网通讯协议不做处理；

#define MANAGE_DELETE						(0x2005)	   //删除智能设备
#define MANAGE_DELETE_ACK				(0x2006)	   //删除智能设备确认
#define MANAGE_UPGRADE_CODE		(0x2007)	   //通讯模块在线升级
#define MANAGE_UPGRADE_ACK			(0x2008)	   //通讯模块在线升级确认
#define MANAGE_MODEL_REBOOT		(0x2009)	   //通讯模块强制复位


#define	LINK_GET_DEVICEFILE		0x2001	  //获取设备描述文件
#define	LINK_PUSH_DEVICEFILE	0x2002	  //上传设备描述文件
#define	LINK_DELETE					0x2005    //删除智能设备
#define	LINK_DELETE_ACK		0x2006    //删除智能设备确认
#define	LINK_UPGRADE_CODE	0x2007	    //通讯模块在线升级
#define	LINK_UPGRADE_ACK		0x2008	    //通讯模块在线升级确认
#define	LINK_MODEL_REBOOT		0x2009  //通讯模块强制复位
#define LINK_READ_INFORMATION (0x200A)  //读取设备信息
#define LINK_INFORMATION_ACK   (0x200B)//ACK
#define	LINK_ORDER_BUNDING		0x200C	//通知绑定智能设备
#define	LINK_BUNDING					0x200C	//绑定智能设备
#define	LINK_BUNDING_ACK			0x200D	//设备绑定确认
#define LINK_QUESTION  (0X2011)				// 设备发起的心跳
#define LINK_ANSWER     (0X2012)   //ACK

#define LINK_ACK   (0X20FF)			//通用ACK 

#define DATA_DATA  (0X3001)  //数据信息

#define ALAM_DATA   (0X4001)   //设备报警
#define REPORT_DATA  (0x5001)  //设备汇报





/********以下宏定义不用了************/
#define MANAGE_REBOOT_ACK				(0x200A)	   //通讯模块强制复位确认
#define MANAGE_BROADCAST				(0x200B)	   //通讯模块强制复位确认
#define MANAGE_BROADCAST_ACK				(0x200C)	   //通讯模块强制复位确认
#define MANAGE_LINK_QUESTION			(0x3001)	   //心跳查询
#define MANAGE_LINK_ANSWER			(0x3002)	   //心跳应答
#define DATA_PUSH								(0x4001)	   //数据传输
#define DATA_PUSH_ACK						(0x4002)	   //数据传输确认
#define DATA_ALAM								(0x5001)	   //智能设备报警信息
#define DATA_ALAM_ACK						(0x5002)	   //智能设备报警确认
#define DATA_REPORT							(0x6001)      //智能设备汇报
#define DATA_REPORT_ACK					(0x6002)       //智能设备汇报确认
/******************************************/

 typedef enum
 {
     HEAD_HEAD,
     HEAD_CTRL,
     HEAD_INFORMATIONTYPE,
     HEAD_DNETID,
     HEAD_SNETID,
     HEAD_SN,
     HEAD_CMD,
     HEAD_LEN
 } IotHead_enum;

#define DATA_SET									(0x4003)     // 设备控制




	typedef struct{

		int reserve;
		unsigned char    status[MAXSTATUSLEN];

	}IotStatus;


	typedef struct{

		int reserve;
		unsigned char    ctl[MAXCONTROLLEN];

	}IotContorl;


	typedef struct{

		unsigned short	fn;
		unsigned short	len;
		unsigned char    value[MAXSTATUSLEN];

	}IotContorlParam;


#ifdef __cplusplus
}
#endif

#endif 
