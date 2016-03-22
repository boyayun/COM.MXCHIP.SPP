

/*

2015.12.02 by zhaoyunhua


�޸ļ�¼��

2015.12.15  

1.������ota�İ汾������

2.������LOG����


�޸ļ�¼��

2015.12.18

�����ɾ���豸�������ĺ���


2016.01.23

�޸���ͷ����netid���ȣ���2���ֽ��޸�Ϊ4�ֽڡ�

DestinationID	Int	4	��Ϣ�����շ�NetID���㲥��ַʱ��ֵΪ0xFFFFFFFF
SourceID	Int	4	��Ϣ�����ͷ�NetID��δ����NetIDʱ��ʾ0x00000000


2016.01.24

	1. ��ģ���޸�Ϊ��˴���

	2. �޸��˷������֯��ʽ��ȫ��base64�� ȥ����Э��ͷF5F5F5F5


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

	��ͬ��ƽ̨���ʵ��޸�����ĺ���

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

	LOG����

*/
	#define IOTLOG			1
	#define LITTLEENDING	1

	/*
	log���
	*/
	void iotLog(const char *s);

	unsigned int iot_ntohl(unsigned int n);
	unsigned int iot_htonl(unsigned int n);
	unsigned short iot_ntohs(unsigned short n);
	unsigned short iot_htons(unsigned short n);

	/*
	base64����
	*/
	char * iotBase64encode( const char * bindataArray, int binlength, char * base64);
	int iotBase64decode( const char * base64, int base64len,  char * bindataArray);
 int iotBase64decode_str( const char * base64, unsigned char * bindata );

		/*
		2016-01-24 
		bindata �������			Ϊ��Ҫbase64����Ķ���������
		binlength  �������		Ϊ��Ҫbase64����Ķ��������ݵĳ���
		base64		�������	Ϊ���������
		base64len �������	Ϊ��������ݵĻ���������(base64len �������binlength��4/3��)

		����ֵ�� ʧ��Ϊ0��base64len ����̫С��
			�ɹ��򷵻ر����ĳ��ȣ�����^��&�ַ���

			���ݰ�ʹ�ô�Ctrl��data���ڴ�������н���Base64���룬
			Ȼ���ٰ�ͷ���ӡ�^�����ڰ�β���ӡ�&���γ�һ�����յ����ݰ�
	*/

	int iotDataToBase64(const  char * bindata, int binlength, char * base64, int base64len);


	/*
		2016-01-24 
		base64  �������			Ϊ��Ҫ�����base64�ַ���������^��&�ַ�)
		bindata  �������		  ������, ��Ҫ�㹻���ܹ����ɽ��������
		binlength   �������		���������ȣ�����С���ַ���base64�ĳ���

		����ֵ�� ʧ��Ϊ0��base64len ����̫С��
			�ɹ��򷵻ض��������ݳ���
	*/
	int iotBase64ToData(const char * base64,  char * bindata, int binlength);



/*

2015-12-15

	���������ݴ�С�����鲻Ҫ����1024

*/
	#define OTABLOCKSIZE	 (512)

	#define IDLENGTH				(36)
	#define	INFOLENGTH			(48)


	typedef struct {

		unsigned short							ctrl;							//������
		unsigned short							informationType;		//��Ϣ������
		unsigned int							dnetid;						//Ŀ��netid
		unsigned int							snetid;						//Դnetid
		unsigned int				sn;							//���кţ�
																		//1��ÿ����һ���������кż�1���ظ��������кŲ��䣻
																		//2�����ͺͷ��ص����кű�����ͬ��
																		//3�����к�ѭ��ʹ�ã�

		unsigned short			cmd;							//������
		unsigned short			len;							//������Ϣ�������ݳ��ȣ���������ͷ��Ϣ���ȡ���λ�ֽ�

	}IotHead;



/*

��ȡ��Ϣͷ��bufΪ�������Ͻ��յ�base64�ַ���

*/
IotHead *iotGetHead(const char *buf, int len);

unsigned short iotGetHeadCmd(const char *buf, int len);

void iotSetHead(char *buf, int len, unsigned short ctrl, unsigned short informationType, unsigned int dnetid, unsigned int snetid);

void iotSetHeadCmd(char *buf, int len, unsigned int sn, unsigned short	cmd, unsigned short datalen);


	//ͨѶģ����������
	typedef struct{
		
		unsigned int codeType;			//�����ļ�����
		unsigned int codeVer;				//��ǰ�汾
										//char *codeFileData;	//�����ļ���Ч����, ���2λΪУ������

		int codeTotal;			//�����ļ��ܰ���
		int codeNumber;		//���س����ļ���ǰ����,codeNumber=0ʱΪ��һ�����ݣ�

	}IotFota;
	
	//ͨѶģ����������ȷ��

	typedef struct{
	
		int codeType;		//�����ļ�����
		int codeVer;			//��ǰ�汾
		int codeNumber;   //���յ��������ļ���ǰ����
		
	}IotFotaAck;


	/*
		buf Ϊ����������,����IotHeadͷ��
		�������������ͺͰ汾
	*/
	void iotSetOtaTypeVer(char *buf, int len, unsigned int codeType, unsigned int codeVer);

	/*
		buf Ϊ����������,������IotHeadͷ��
		������ţ��ܰ����� ����������
		�������������ȣ����Ȳ�������ͷ)
	*/
	int iotSetOta(char *buf, int len, int codenumber, int codeTotal,  char *data, int datalen);

	/*
		buf Ϊ����������,����IotHeadͷ��
		���û�Ӧ����ţ����ͺͰ汾
		���� ��Ӧ�� ���ȣ����Ȳ�������ͷ)
	*/
	int iotSetOtaAck(char *buf, int len, int codenumber, int codeType, int codeVer);


	/*
		�Ӷ�������������ȡ��������
		char *buf, int len   ���������Ķ��������ݰ����䳤��

		datalen ��������� ������ĳ���
	*/
	char *iotGetOtaData(char *buf, int len, int *datalen, IotFota *fota);
	
	//��������豸, Ŀǰ�а������豸
	//���ܿ������Ľ������豸�󶨵Ĳ�����Ϣ������Ϣ��Ϊ�㲥����
	typedef struct{

		char deviceid[IDLENGTH];						//׼����ӵ��豸ID
		unsigned int deviceNetid;		//Ϊ�¼����豸�����NetID
		unsigned int gatewayNetid;		//Ϊ�¼����豸�����NetID
		int			gatewayVer;					//���صķ���汾

	}IotAddDevice;

	
	//��������豸ȷ��
	typedef struct{

		char deviceid[IDLENGTH];						//׼����ӵ��豸ID
		unsigned int deviceNetid;		//Ϊ�¼����豸�����NetID
		unsigned int gatewayNetid;		//Ϊ�¼����豸�����NetID

	}IotAddDeviceAck;



	/*

		����豸���

	*/

	int iotSetAddDevice(IotAddDevice *d, char *deviceid, unsigned int deviceNetid, unsigned int gatewayNetID, int gatewayVer);
	
	IotAddDevice *iotGetAddDevice(const char *buf, int len);

	int iotAddDeviceAck(IotAddDeviceAck *d, char *deviceid, unsigned int deviceNetid, unsigned int gatewayNetID);

	//ɾ�������豸
	typedef struct{

		char deviceid[IDLENGTH];	
		unsigned int deviceNetid;		//Ϊ�¼����豸�����NetID
		int		reserve;

	}IotDelDevice;


	//ɾ�������豸ȷ��
	typedef struct{

		char deviceid[IDLENGTH];	
		unsigned int		deviceNetid;
	unsigned	short reserve;

	}IotDelDeviceAck;

	//ͨѶģ��ǿ�Ƹ�λ
	typedef struct{

		char deviceid[IDLENGTH];	
		int reserve;

	}IotResetDevice;


	//state״̬
#define MESSAGE_DATA_GOOD		(0xa001)
#define MESSAGE_DATA_TIMEOUT	(0xa002)
#define MESSAGE_DATA_INVALID		(0xa003)
#define MESSAGE_DATA_ERROR		(0xa004)

	//ͨ��ģ��ǿ�Ƹ�λȷ��, ��������ȷ��
	typedef struct{

		int state;
		int reserve;

	}IotDataAck;


	//����
	typedef struct
	{
		char deviceid[IDLENGTH];	
		char	gatewayid[IDLENGTH];	
		int  reserve;

	}IotHeartbeat;


//�豸�������������
	typedef struct 
		{
		unsigned int SoftWareVersion_L;
		unsigned int SoftWareVersion_H;
		unsigned int reserve;

		}IotLinkQuestion;
	//����Ӧ��
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
		int		swh;			//swh��swlһ���ʾģ��汾 �汾�Ÿ�λ
		int		swl;			//�汾�ŵ�λ
		int  reserve;

	}IotHeart2;
/*
2015-12-16

����������deviceid��gatewayid��������IotHeartbeat�Ĵ�С

*/

	int iotSetHeartbeat(IotHeartbeat *d, char *deviceid, char *gatewayid);
	


	//����Ӧ��
	typedef struct
	{
		char deviceid[IDLENGTH];	
		char	gatewayid[IDLENGTH];	
		int		swl;			//swh��swlһ���ʾģ��汾 �汾�Ÿ�λ
		int		swh;			//�汾�ŵ�λ
		int  reserve;

	}IotHeartbeatAck;


	/*

	2015-12-21

	��������Ӧ��ĵ�deviceid��gatewayid���汾�ţ� ������IotHeartbeatAck�Ĵ�С

	*/
	int iotSetHeartbeatAck(IotHeartbeatAck *d, char *deviceid, char *gatewayid, int swh, int swl);


	//�豸����
	typedef struct
	{
		int reserve;
		unsigned int errorCode;

	}IotAlarm;


	//��������
	typedef struct
	{

		int reserve;
	//	unsigned short funNum;;
//unsigned short VarIndex;

	}IotData;


	//�豸�����ļ���Ϣ��
	typedef struct{

		int number; 

	}IotDeviceDes;
		

	//�豸�����ļ���Ϣ������
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



#define TYPE_DISCOVERY (0x1000)					//��δ����ʹ�õ��豸��������Ϣ����
#define TYPE_MANGERY	(0x2000)					//���ܼ�ͥ���������͹�������Ϣ����������Ϣֻ��ͨѶģ�����������֮����н�����
#define	TYPE_LINK (0x2000)			//ͨѶģ�������ܼ�ͥ����֮��������������Ϣ��
#define	TYPE_DATA (0x3000)		//���ܿ����������豸֮�����Ϣ�����ࣻ����Ϣ�����ɿ������ķ���ġ�������ͨѶģ�������ܼ�֮ͥ���Э���в�������ֻ������ݵ�ת����
#define	TYPE_AMAL (0x4000)		//�豸�������͵ı�������Ϣ��������Ϣ�����豸���𣬲�����Ҫ�����������Ϣ�ࡣ������Ϣ����ͨѶЭ�鲻������
#define	TYPE_REPORT (0x5000)	//�豸�����������Ϣ��������Ϣ����ͨѶЭ�鲻������

#define MANAGE_DELETE						(0x2005)	   //ɾ�������豸
#define MANAGE_DELETE_ACK				(0x2006)	   //ɾ�������豸ȷ��
#define MANAGE_UPGRADE_CODE		(0x2007)	   //ͨѶģ����������
#define MANAGE_UPGRADE_ACK			(0x2008)	   //ͨѶģ����������ȷ��
#define MANAGE_MODEL_REBOOT		(0x2009)	   //ͨѶģ��ǿ�Ƹ�λ


#define	LINK_GET_DEVICEFILE		0x2001	  //��ȡ�豸�����ļ�
#define	LINK_PUSH_DEVICEFILE	0x2002	  //�ϴ��豸�����ļ�
#define	LINK_DELETE					0x2005    //ɾ�������豸
#define	LINK_DELETE_ACK		0x2006    //ɾ�������豸ȷ��
#define	LINK_UPGRADE_CODE	0x2007	    //ͨѶģ����������
#define	LINK_UPGRADE_ACK		0x2008	    //ͨѶģ����������ȷ��
#define	LINK_MODEL_REBOOT		0x2009  //ͨѶģ��ǿ�Ƹ�λ
#define LINK_READ_INFORMATION (0x200A)  //��ȡ�豸��Ϣ
#define LINK_INFORMATION_ACK   (0x200B)//ACK
#define	LINK_ORDER_BUNDING		0x200C	//֪ͨ�������豸
#define	LINK_BUNDING					0x200C	//�������豸
#define	LINK_BUNDING_ACK			0x200D	//�豸��ȷ��
#define LINK_QUESTION  (0X2011)				// �豸���������
#define LINK_ANSWER     (0X2012)   //ACK

#define LINK_ACK   (0X20FF)			//ͨ��ACK 

#define DATA_DATA  (0X3001)  //������Ϣ

#define ALAM_DATA   (0X4001)   //�豸����
#define REPORT_DATA  (0x5001)  //�豸�㱨





/********���º궨�岻����************/
#define MANAGE_REBOOT_ACK				(0x200A)	   //ͨѶģ��ǿ�Ƹ�λȷ��
#define MANAGE_BROADCAST				(0x200B)	   //ͨѶģ��ǿ�Ƹ�λȷ��
#define MANAGE_BROADCAST_ACK				(0x200C)	   //ͨѶģ��ǿ�Ƹ�λȷ��
#define MANAGE_LINK_QUESTION			(0x3001)	   //������ѯ
#define MANAGE_LINK_ANSWER			(0x3002)	   //����Ӧ��
#define DATA_PUSH								(0x4001)	   //���ݴ���
#define DATA_PUSH_ACK						(0x4002)	   //���ݴ���ȷ��
#define DATA_ALAM								(0x5001)	   //�����豸������Ϣ
#define DATA_ALAM_ACK						(0x5002)	   //�����豸����ȷ��
#define DATA_REPORT							(0x6001)      //�����豸�㱨
#define DATA_REPORT_ACK					(0x6002)       //�����豸�㱨ȷ��
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

#define DATA_SET									(0x4003)     // �豸����




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
