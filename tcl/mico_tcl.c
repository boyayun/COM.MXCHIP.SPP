#include "mico.h"

#include "StringUtils.h"
#include "SppProtocol.h"
#include "cfunctions.h"
#include "cppfunctions.h"
#include "MICOAPPDefine.h"
#include "mico_tcl_ota.h"
#include "mico_tcl.h"
// sendto( udp_fd, buf, len, 0, &addr, sizeof(struct sockaddr_t) );

struct users g_us[MAXUSERS];
struct MYCONFIGS g_myconfig;     //用户配置

struct MYGLOBALCON g_globalflag;   //业务标志

extern  uint8 g_mySendBuf[LEN_OF_RECE_360]; //接收到净化器的反馈信息 
#define iot_get_ms_time mico_get_time

#define mico_tcl_log(M, ...) custom_log("mico_tcl", M, ##__VA_ARGS__)


extern  uint8	g_mySendBuf_temp[LEN_OF_RECE_360]; //接收到净化器的反馈信息 

extern  int  crc_check(uint8 length, uint8 *address);
void return_device_status(void);
void get_uart_date(uint8 * buf,int len)
{
		
  char statusht[64];
  uint8 len_N=0;
  len_N=len/LEN_OF_RECE_360;
//  getUartStatus(statusht);
		if(len%LEN_OF_RECE_360==0)//如果是31的倍数，则直接校验最后31个字节
		{
			 if(crc_check(LEN_OF_RECE_360,buf+(len_N-1)*LEN_OF_RECE_360))
			 	{
			 memcpy(g_mySendBuf,buf+(len_N-1)*LEN_OF_RECE_360,LEN_OF_RECE_360);
    getUartStatus(statusht);
    mico_tcl_log("%s",statusht);
    //return_device_status();
			 	}

			 return ;
		}
	
		
		 if(len_N==0)//如果小于31
						{
								if(buf[0]==0xf5&&buf[1]==0xf5&&buf[2]==0xf5&&buf[3]==0xf5)
									{
													 memcpy(g_mySendBuf_temp,buf,len);
									}
								else if(g_mySendBuf_temp[0]==0xf5&&g_mySendBuf_temp[1]==0xf5&&g_mySendBuf_temp[2]==0xf5&&g_mySendBuf_temp[3]==0xf5)
										{
												 memcpy(g_mySendBuf_temp+LEN_OF_RECE_360-len,buf,len);
												 if(crc_check(LEN_OF_RECE_360,g_mySendBuf_temp))
												 	{
															memcpy(g_mySendBuf,g_mySendBuf_temp,LEN_OF_RECE_360);
									      getUartStatus(statusht);
									      mico_tcl_log("%s",statusht);
               //return_device_status();
									      
												 	}
										}
						}
					else//大于31个字节
					{
								if(buf[len-LEN_OF_RECE_360]==0xf5&&buf[len-LEN_OF_RECE_360+1]==0xf5&&buf[len-LEN_OF_RECE_360+2]==0xf5&&buf[len-LEN_OF_RECE_360+3]==0xf5)
									{
												 if(crc_check(LEN_OF_RECE_360,buf+len-LEN_OF_RECE_360))
												 	{
											 memcpy(g_mySendBuf,buf+len-LEN_OF_RECE_360,LEN_OF_RECE_360);							
												 getUartStatus(statusht);
									      mico_tcl_log("%s",statusht);
               //return_device_status();
												 	}
									}
								else if(buf[0]==0xf5&&buf[1]==0xf5&&buf[2]==0xf5&&buf[3]==0xf5)
									{
												 if(crc_check(LEN_OF_RECE_360,buf+(len_N-1)*LEN_OF_RECE_360))
												 	{
														 memcpy(g_mySendBuf,buf+(len_N-1)*LEN_OF_RECE_360,LEN_OF_RECE_360);				
														 getUartStatus(statusht);
									      mico_tcl_log("%s",statusht);
               //return_device_status();
												 	}
												// if(buf[len_N*LEN_OF_RECE_360]==0xf5&&buf[len_N*LEN_OF_RECE_360+1]==0xf5&&buf[len_N*LEN_OF_RECE_360+2]==0xf5&&buf[len_N*LEN_OF_RECE_360+3]==0xf5)
												 //	{
											//		 memcpy(g_mySendBuf_temp,buf,len);

												 //	}

									}


					}

}






void xor_change(char *data, int len)
{
	const char key = 'T';
	int i = 0;
	
	if (data == NULL)
		 return;

	for ( i = 0; i < len; i++)
	{
		 data[i] = data[i]^key;
	}
}



void netgetmac(char *mac)
{
		//uint8 hwaddr[6];
		//wifi_get_macaddr(STATION_IF, hwaddr);
		uint8 i;
	 IPStatusTypedef para;

		
	 micoWlanGetIPStatus(&para, Station);
	 formatMACAddr(mac, (char *)&para.mac);
	 for(i=0;i<32;i++)
	 	{
	 		if(mac[i]>='A'&&mac[i]<='Z')
	 			{
						mac[i]+=32;
	 			}

	 	}

/*
	if (g_macaddr[0] == 0 && g_macaddr[1] == 0 && g_macaddr[2] == 0 && g_macaddr[3] == 0 &&
		g_macaddr[4] == 0 && g_macaddr[5] == 0)
	{
		spi_flash_read(FLASH_EEP_CFG_BASE+EEPROM_MAC_12_OFFSET , g_macaddr, MAC_ADDR_LEN);
	}
	
	sprintf(mac,"%02x:%02x:%02x:%02x:%02x:%02x", 
			g_macaddr[0],
			g_macaddr[1],
			g_macaddr[2],
			g_macaddr[3],
			g_macaddr[4],
			g_macaddr[5]);*/
}






int  getv(char c)
{
	if (c == '0')
		return 0;

	if (c == '1')
		return 1;

	if (c == '2')
		return 2;

	if (c == '3')
		return 3;

	if (c == '4')
		return 4;

	if (c == '5')
		return 5;

	if (c == '6')
		return 6;

	if (c == '7')
		return 7;

	if (c == '8')
		return 8;

	if (c == '9')
		return 9;

	if (c == 'a' || c == 'A')
		return 10;

	if (c == 'b' || c == 'B')
		return 11;

	if (c == 'c' || c == 'C')
		return 12;

	if (c == 'd' || c== 'D')
		return 13;

	if (c == 'e' || c == 'E')
		return 14;

	if (c == 'f' || c == 'F')
		return 15;

	return 0;
}

int  getBytes(char *str, char *value)
{
	int i = 0;
	int len = strlen(str);
	if (len % 2 != 0)
	{
		return 0;
	}

	for (i = 0; i < len; i+=2)
	{
		int s1 = getv(str[i]);
		int s2 = getv(str[i + 1]);

		value[i/2] = (s1<<4) + s2;
	}

	return i/2;
}



void getControlStatus(void)
{
	
	uint16 crc = 0;
	int i = 0;
	uint8 cmd[21];

	for (i = 0; i < 21; i++)
	{
		cmd[i] = 0;
	}

	cmd[0] = 0xF5;
	cmd[1] = 0xF5;
	cmd[2] = 0xF5;
	cmd[3] = 0xF5;

	cmd[4] = 0x00;		//源地址	
	cmd[5] = 0x01;		//目标地址WIFI模块地址为0x00，主控板地址为0x01

	cmd[6] = 0x04;   		//功能码4表示查询
	cmd[7] = 11;			//数据区长度
	cmd[8] = 0x01;		//版本号高位
	cmd[9] = 0x00;		//版本号低位

	crc = crc_calculate(19, cmd);

	cmd[19] = crc >> 8;
	cmd[20] = crc & 0x00FF;
 MicoUartSend(UART_FOR_APP,cmd, 21);

//	iot_uart_output(cmd, 21);
	
	//my_printf("1234567890\r\n");
}



void urPass(char *pass)
{
	int stlen = 0;
	int s = 0;
	char stbytes[JSONLENGTH] = {0};

	stlen = getBytes(pass, stbytes);
	int check = crc_check(stlen, stbytes);
	
// my_printf("urPass:check==%d,stlen==%d",check,stlen);
	

	
	
	if (check == 0)
		return;

	
			MicoUartSend(UART_FOR_APP,stbytes, stlen);


}


void  trim(char *value)
{
	char buf[JSONLENGTH];
	int len = strlen(value);
	int i = 0;
	int s = 0;

	for (i = 0; i < len; i++)
	{
		if (value[i] != ' ' && value[i] != '\"')
			break;
	}

	strcpy(buf, value + i);
	len = strlen(buf);

	for (i = len - 1; i >= 0; i--)
	{
		if (buf[i] != ' ' && buf[i] != '\"')
			break;

		buf[i] = 0;
	}

	strcpy(value, buf);
}

int getJson(char *json, char *key, char *value)
{
	int j = 0;
	int i = 0;
	char keysp[160];
	int len = strlen(json);
	int keylen = strlen(key);
	
	if (keylen > 40)
	{
		value[0] = 0;
		return 0;
	}

	sprintf(keysp, "\"%s\":", key);
	keylen = strlen(keysp);
	
	strcpy(value, "");

	if (len - keylen <= 5)
	{
		value[0] = 0;
		return 0;
	}

	for (i = 0; i < len - keylen; i++)
	{
		for (j = 0; j < keylen; j++)
		{
			if (json[i + j] != keysp[j])
				break;
		}

		if (j == keylen)
			break;
	}

	//找到了key
	if (j == keylen)
	{
		int index = i + j;
		int s = 0;
		for (s = index; s < len; s++)
		{
			if (json[s] == ',' || json[s] == '}')
			{
				break;
			}
		}

		if (s != len && s - index < JSONLENGTH)
		{
			memcpy(value,  json + index, s - index);
			value[s - index] = 0;
			trim(value);
			return strlen(value);
		}
		else
		{
			value[0] = 0;
			return 0;
		}
	}
	else
	{
		value[0] = 0;
		return 0;
	}
}


void usAdd(char *from, uint32 ip, uint16 port)
{
	//查找是否已经在系统里面存在
	int index = -1;
	int i = 0;

	for (i = 0; i < MAXUSERS; i++)
	{
		if (strcmp(from, g_us[i].from) == 0 && g_us[i].flag == 1)
		{
			g_us[i].ip = ip;
			g_us[i].port = port;
			g_us[i].time = g_globalflag.timercounter;
			//my_printf("update index:%d, ip:%x port:%d time:%d \r\n", i, ip, port, g_us[i].time);
			return;
		}
	}

	//没有这个用户
	if (i == MAXUSERS)
	{
		for (i = 0; i < MAXUSERS; i++)
		{
			if (g_us[i].flag == 0)
			{
				strcpy(g_us[i].from, from);
				g_us[i].flag = 1;
				g_us[i].ip = ip;
				g_us[i].port = port;
				g_us[i].time = g_globalflag.timercounter;

				//my_printf("add client:%d, ip:%x, port:%d, %s\r\n", i, ip, port, from);
				return;
			}
		}

	}
}


int  getUartStatus(char *out)
{
	int i = 0;
	strcpy(out, "");
	for (i = 0; i < LEN_OF_RECE_360; i++)
	{
		char buf[12];
		sprintf(buf, "%02x", g_mySendBuf[i]);
		memcpy(out + i * 2, buf, 2);
	}

	out[2 * LEN_OF_RECE_360] = 0;
}


#if 1
int netfinddevice(char *buf, char *id)
{
	char mac[64];
	char statusht[64];
	int len = 0;

	if (id == NULL)
		return 0;

	len = strlen(id);
	if (len <= 0)
		strcpy(id, "222");


	getUartStatus(statusht);
	netgetmac(mac);

//	if (g_myconfig.ncity != 50)
	{
		strcpy(g_myconfig.szcity, "101280601");
	}

	sprintf(buf, "{\"protocol_type\":\"NC\",\"protocol_seq\":4,\"msgid\":\"servdev_probe\",\"msg_type\":\"resp\",\"msg_seq\":%s,\"action\":\"broadcast\",\"wifi_module_mac\":\"%s\",\"name\":\"VENM56m65rCU5YeA5YyW5Zmo\",\"pass\":\"%s\",\"wifi_module_firm_ver\":%d,\"indic_ver\":\"yy.nn\",\"device_type_id\":\"020201\",\"device_model\":\"%d\",\"citynum\":\"%s\",\"wifi_module_model\":\"%s\"}",
			id,
			mac,
			statusht,
			THISVERSION, 5, g_myconfig.szcity, WIFIMM);

	len = strlen(buf);
	return len;
}

#else
int netfinddevice(char *buf, char *id)
{
	char mac[64];
	char statusht[128];
	int len = 0;
// IPStatusTypedef para;

	if (id == NULL)
		return 0;

	len = strlen(id);
	if (len <= 0)
		strcpy(id, "222");


// 	getUartStatus(statusht);
getUartStatus(statusht);
 //micoWlanGetIPStatus(&para, Station);
// mac=para.mac;

	netgetmac(mac);

	sprintf(buf, "{\"protocol_type\":\"NC\",\"protocol_seq\":4,\"msgid\":\"servdev_probe\",\"msg_type\":\"resp\",\"msg_seq\":%s,\"wifi_module_model\":\"%s\",\"action\":\"broadcast\",\"wifi_module_mac\":\"%s\",\"name\":\"%s\",\"pass\":\"%s\",\"wifi_module_firm_ver\":%d,\"indic_ver\":\"yy.nn\",\"device_type_id\":\"%s\",\"device_model\":\"%s\"}",
			id,
			WIFIMM,
			mac,
 		WIFI_NAME,
			statusht,
			THISVERSION,
			DEVICE_ID,
			DEVICE_MODEL);

	len = strlen(buf);
	return len;
}
#endif

extern int local_udp_fd;
extern struct sockaddr_t g_gloab_addr;


void uip_sendps(void *data, uint16 len)
{
 char ip_address[16];

	if (USEPASS == 1)
	{
		xor_change((char *)data, len);
	}
 inet_ntoa( ip_address, g_gloab_addr.s_ip );

// mico_tcl_log( " %s:%d, len:%d", ip_address,g_gloab_addr.s_port, len );//wcz wcz_add

 sendto( local_udp_fd, data, len, 0, &g_gloab_addr, sizeof(struct sockaddr_t) );

	
//	uip_send(data, len);
}



void uip_send(void *data, uint16 len)
{
 char ip_address[16];


 inet_ntoa( ip_address, g_gloab_addr.s_ip );

// mico_tcl_log( " %s:%d, len:%d", ip_address,g_gloab_addr.s_port, len );//wcz wcz_add

 sendto( local_udp_fd, data, len, 0, &g_gloab_addr, sizeof(struct sockaddr_t) );

	
//	uip_send(data, len);
}

int netackremote(char *buf, char *id, char *action, char *from, char *to)
{
	char statusht[128];
	char mac[64];
	int len = 0;

	if (strlen(id) <= 0)
		strcpy(id, "222");

	getUartStatus(statusht);
	netgetmac(mac);

	sprintf(buf, "{\"protocol_type\":\"SC\",\"protocol_seq\":1,\"msgid\":\"dev_sc_remote\",\"msg_type\":\"resq\",\"msg_seq\":%s,\"action\":\"%s\",\"uicon_mac\":\"%s\",\"wifi_module_mac\":\"%s\",\"resp_result\":0,\"cmd_return\":{\"pass\":\"%s\",\"time\":%ld}}", 
		id, action, from, mac, statusht, iot_get_ms_time());

	len = strlen(buf);
	return len;
}

int netack(char *buf, char *id, char *action, char *from, char *to)
{
	char statusht[256];
	char mac[64];
	int len = 0;

	if (strlen(id) <= 0)
		strcpy(id, "222");
	
	getUartStatus(statusht);
	netgetmac(mac);

	sprintf(buf, "{\"protocol_type\":\"SC\",\"protocol_seq\":1,\"msgid\":\"dev_sc\",\"msg_type\":\"resq\",\"msg_seq\":%s,\"action\":\"%s\",\"uicon_mac\":\"%s\",\"wifi_module_mac\":\"%s\",\"resp_result\":0,\"cmd_return\":{\"pass\":\"%s\",\"time\":%ld}}", 
		id, action, from, mac, statusht, iot_get_ms_time());

	len = strlen(buf);
	return len;
	
}

void mystrcat(char *cat, char *str)
{
	int len1 = strlen(cat);
	int len2 = strlen(str);

	int i = len1;
	int clen = len1 + len2;

	for (i = len1; i < clen; i++)
	{
		cat[i] = str[i - len1];
	}

	cat[clen] = 0;
}


int getUartStatus2(char *outstring)
{

	//strcpy(outstring,"\"mode\":0");
	
	
	char *out = outstring;
	char temp[64];
	int fanlife = 0;
	int tempis = 0;
	int dust = 0;

	strcpy(out, "");

	//第11个字节为开关机,P3
	tempis = g_mySendBuf[11];
	sprintf(temp, "\"power\":%d,", tempis);
	mystrcat(out, temp);

	//P4, 工作模式设定状态：
	tempis = g_mySendBuf[12];
	sprintf(temp, "\"mode\":%d,", tempis);
	mystrcat(out, temp);

	//P5, 功能设定：
	tempis = g_mySendBuf[13];
	sprintf(temp, "\"fun\":%d,", tempis);
	mystrcat(out, temp);

	//P6风量选择：
	tempis = g_mySendBuf[14];
	sprintf(temp, "\"fan\":%d,", tempis);
	mystrcat(out, temp);

	//P7定时选择：(适应现在程序)
	tempis = g_mySendBuf[15];
	sprintf(temp, "\"timer\":%d,", tempis);
	mystrcat(out, temp);

	//tempis = g_mySendBuf[15];
	//sprintf(temp, "\"life_span_update\":%d,", tempis);
	//mystrcat(out, temp);

	//P8:空气质量检测结果：
	tempis = g_mySendBuf[16];
	sprintf(temp, "\"air_quality\":%d,", tempis);
	mystrcat(out, temp);

	//P9,滤网更换提醒：0x00  无更换；
	//g_mySendBuf[17];
	tempis = g_mySendBuf[17];
	sprintf(temp, "\"alert\":%d,", tempis);
	mystrcat(out, temp);


	//P10, P11滤网更换低字节和滤网更换高字节
	fanlife = g_mySendBuf[18] + g_mySendBuf[19] * 256;
	sprintf(temp, "\"used_span\":%d,",  fanlife);
	mystrcat(out, temp);

	//P12,P13, 灰尘传感器数值
	dust = g_mySendBuf[20] + g_mySendBuf[21] * 256;
	sprintf(temp, "\"dust_sensor\":%d,", dust);
	mystrcat(out, temp);

	//P14,气味传感器数据
	int flavor = g_mySendBuf[22];
	sprintf(temp, "\"flavor\":%d,", flavor);
	mystrcat(out, temp);

	//故障码P15
	int er = g_mySendBuf[23];
	sprintf(temp, "\"err_code\":%d,", er);
	mystrcat(out, temp);

	//P16 温度
	int tmp = g_mySendBuf[24];
	sprintf(temp, "\"tmp\":%d,", tmp);
	mystrcat(out, temp);

	//P17湿度
	int dit = g_mySendBuf[25];
	sprintf(temp, "\"rh\":%d,", dit);
	mystrcat(out, temp);

	//P18产品型号定义表：
	int md = g_mySendBuf[26];
	sprintf(temp, "\"md\":%d", md);
	mystrcat(out, temp);
	
}


int netstatusReq(char *outvalue, char *id)
{
	int i = 0;
	char statusht[64];
	char mac[64];
	char reid[JSONLENGTH];

	if (id == NULL)
		return 0;

	int len = strlen(id);
	if (len <= 0)
	{
		g_globalflag.stid++;
		sprintf(reid, "%d", g_globalflag.stid);
	}
	else
	{
		strncpy(reid, id, JSONLENGTH);
	}

	netgetmac(mac);
	getUartStatus(statusht);

//	char outstring[240];
//	getUartStatus2(outstring);
//	sprintf(outvalue, "{\"protocol_type\":\"UR\",\"protocol_seq\":1,\"msgid\":\"dev_se_upload\",\"msg_type\":\"notify\",\"msg_seq\":%s,\"wifi_module_mac\":\"%s\",\"resp_result\":0,\"cmd_return\":{\"pass\":\"%s\",%s}}",
	//		reid, mac, statusht, outstring);

	sprintf(outvalue, "{\"protocol_type\":\"UR\",\"protocol_seq\":1,\"msgid\":\"dev_se_upload\",\"msg_type\":\"notify\",\"msg_seq\":%s,\"wifi_module_mac\":\"%s\",\"resp_result\":0,\"event_list\":{\"pass\":\"%s\"}}",
		reid, mac, statusht);

	i = strlen(outvalue);
	
	return i;
}

void return_device_status(void)
{
	char buf[512];

int len;

 //OSStatus err;

 if(local_udp_fd==-1)
 	return ;
// err = bind(local_udp_fd, &g_gloab_addr, sizeof(g_gloab_addr));
//require_noerr( err, exit );

 len = netstatusReq(buf, "222");
 uip_sendps(buf, len); 
 exit:



}


void udp_server_call_back(const void *udp_buf,  size_t buf_len, 
               const struct sockaddr_t  *dest_addr)
{
	char *recvstring = NULL;
	int i = 0;
	char id[JSONLENGTH], from[JSONLENGTH], to[JSONLENGTH], pass[JSONLENGTH], action[JSONLENGTH];
	char msgid[JSONLENGTH];
	char type[4];
	int len = 0;
	char buf[512];
	uint8 *spp = NULL;

	uint32 souceip = 0;
	uint16  sourceport = 0;

	//char recvstring[600];    //不能复制


	//	uint32 rsip = *(uint32 *)g_server;
		int recvlen = buf_len;
	//	if (recvlen > 512 || recvlen <= 0)
	//		return;

	recvstring = (char *)udp_buf;
		if (USEPASS == 1)
		{
		xor_change(recvstring, recvlen);
		}
		recvstring[recvlen] = 0;


  g_gloab_addr=*dest_addr;

		souceip = dest_addr->s_ip;
		sourceport = dest_addr->s_port;

		spp = (uint8 *)&souceip;
		
		//my_printf("recv ip:%d.%d.%d.%d, port: %d len:%d\r\n", 
		//	spp[0], spp[1], spp[2], spp[3], sourceport, recvlen);
		//my_printf(recvstring);
		//my_printf("\r\n");
		
		getJson(recvstring, "msg_seq", id);
	    	getJson(recvstring, "uicon_mac", from);
	    	getJson(recvstring, "wifi_module_mac", to);
	   	getJson(recvstring, "action", action);
	    	getJson(recvstring, "pass", pass);
      getJson(recvstring, "msgid", msgid);

      
	
		i = strlen(pass);
		
		if (i > 0)
		{
			urPass(pass);
		
		}

		if (strcmp(action, "finddevice") == 0)
		{	
		//	usAdd(from, souceip, sourceport);

		//	netsetcitynum(recvstring);
			len = netfinddevice(buf, id);
			uip_sendps(buf, len);	

			//my_printf("find device return\r\n");
			return;
		}	
		else if (strcmp(action, "status_req") == 0)
		{
			len = netstatusReq(buf, id);
			uip_sendps(buf, len);	
			return;
		}
		else
		{
			
			//if (rsip != souceip)
										if (i > 0)
							{
										urPass(pass);
										len = netack(buf, id, action, from, to);
										uip_sendps(buf, len); 
										return ;
								}
							else
								{


        uip_send(recvstring, recvlen); 

								}

		}
		



	
}






