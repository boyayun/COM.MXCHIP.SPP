/**
  ******************************************************************************
  * @file    LocalTcpServer.c 
  * @author  William Xu
  * @version V1.0.0
  * @date    05-May-2014
  * @brief   This file create a TCP listener thread, accept every TCP client
  *          connection and create thread for them.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, MXCHIP Inc. SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2014 MXCHIP Inc.</center></h2>
  ******************************************************************************
  */

#include "MICO.h"
#include "SppProtocol.h"
#include "SocketUtils.h"
#include "MICOAPPDefine.h"
#include "geniot.h"
//#include "mico_tcl.h"
#include "mico_tcl_ota.h"
#include "devproanaly.h"

#define server_log(M, ...) custom_log("UDP SERVER", M, ##__VA_ARGS__)
#define server_log_trace() custom_log_trace("TCP SERVER")
extern  void formatMACAddr(char *destAddr, char *srcAddr);

static app_context_t *context;
//extern void udp_server_call_back(const void *udp_buf,  size_t buf_len,      const struct sockaddr_t  *dest_addr);
extern void mico_system_delegate_config_will_start(void);
//unsigned int g_GatewayID[9];
void tcl_device_warning_ack();

 char * p_base=NULL;

USER_CONFIG g_userconfig;
G_USER_UART_STATUS g_user_uart_stats;


typedef struct 
{
	unsigned short flag ;//序列号
// char deviceid[IDLENGTH];     //智能控制中心ID
// char gatewayid[IDLENGTH];     //智能控制中心ID
	uint32_t ip;
}USER_EASY_LINK;

USER_EASY_LINK g_easy_link;





int                     g_serudpsock = 0;
//升级需要的变量
int                     g_codeType = 0;
int                     g_codeVer = 0;


int                     g_upgradeFlag = 0;
int                     g_upcodenumber = 0;
int                     g_NewImg2Addr = 0;
int                     g_checksum = 0;
unsigned int           g_filesum = 0;
int                     g_recvsize = 0;
int                     g_sec = 0;
struct sockaddr_t      g_otaaddr;
int                     g_resendtimes = 0;
extern  uint16 crc_calculate(int length, uint8 *address);


#if EN_BASE64_TO_F5F5
uint8	g_mySendBuf[LEN_OF_RECE_360]; //接收到净化器的反馈信息 
uint8	g_mySendBuf_last[LEN_OF_RECE_360]; //接收到净化器的反馈信息 
uint8	g_mySendBuf_temp[LEN_OF_RECE_360]; //接收到净化器的反馈信息 

uint8	g_myControl_uart[LEN_OF_CONR_360]; //净化器的控制信息
#elif TCL_WATER_CONDITION
uint8	g_myControl_uart[LEN_OF_CONR_WATER]; //净化器的控制信息
uint8	g_mySendBuf[UART_LEN_WATER_CONDITION]; //接收到净化器的反馈信息 
uint8	g_mySendBuf_last[UART_LEN_WATER_CONDITION]; //接收到净化器的反馈信息 
uint8	g_mySendBuf_temp[LEN_OF_TCL_DEVICE]; //接收到净化器的反馈信息 


#else
uint8	g_myControl_uart[LEN_OF_CONR_360]; //净化器的控制信息
uint8	g_mySendBuf[LEN_OF_TCL_DEVICE]; //接收到净化器的反馈信息 
uint8	g_mySendBuf_last[LEN_OF_TCL_DEVICE]; //接收到净化器的反馈信息 
uint8	g_mySendBuf_temp[LEN_OF_TCL_DEVICE]; //接收到净化器的反馈信息 

#endif


void tcl_data_upload(unsigned char *buf, int len, struct sockaddr_t *paddr);
void tcl_data_warning(unsigned char *buf, int len, struct sockaddr_t *paddr);
void water_wifi_reset_ack(void);

uint16 crc_calculate(int length, uint8 *address)
{
	int i,j;
	uint16 crc;
	crc = 0xffff;
	for(i=0;i<length;i++){
		crc^= *address++;
		for(j=0;j<8;j++){
			if(crc&0x01){
				crc=(crc>>1)^0x8408;
			}
			else{
				crc >>=0x01;
			}
		}
	}
	return ~crc;
}
#if CRC_ADD_F5_55
int  crc_check(uint8 length, uint8 *address)
{
	uint16 crc = 0;
	unsigned int crc32=0;
	uint8 h = 0;
	uint8 l = 0;

			if(*(unsigned int *)&address[length-4]==0x55f555f5)
				{
					  crc = crc_calculate(length - 4, address);
		     h = crc >> 8;
		     l = crc & 0x00FF;
		   
		     if (h == address[length-4] && l == address[length-2])
		       return 1;
				}
			else if(*(unsigned short *)&address[length-2]==0x55f5)
				{
							 crc = crc_calculate(length - 3, address);
		     h = crc >> 8;
		     l = crc & 0x00FF;
		   
		     if (h == address[length-3] && l == address[length-2])
		       return 1;

				}
				else if(*(unsigned short *)&address[length-3]==0x55f5)
				{
							 crc = crc_calculate(length - 3, address);
		     h = crc >> 8;
		     l = crc & 0x00FF;
		   
		     if (h == address[length-3] && l == address[length-1])
		       return 1;

				}
				else
					{
					crc = crc_calculate(length - 2, address);
				h = crc >> 8;
				l = crc & 0x00FF;
		
				if (h == address[length-2] && l == address[length-1])
						return 1;

					}
	
			

	return 0;
}

#else
int  crc_check(uint8 length, uint8 *address)
{
	uint16 crc = 0;
	uint8 h = 0;
	uint8 l = 0;
			crc = crc_calculate(length - 2, address);
			h = crc >> 8;
			l = crc & 0x00FF;
	
			if (h == address[length-2] && l == address[length-1])
					return 1;

	return 0;
}
#endif
//json处理函数, json 的key和value不能超过64个字符
//value长度不能超过64




void wifi_net_status(unsigned char date)
{
		unsigned char cmd[8];
		unsigned short crc=0;
		cmd[0]=0xf5;
		cmd[1]=0xf5;
		cmd[2]=0x00;
		cmd[3]=0x04;

		cmd[4]=FRAME_WIFI_STATUS;//帧类型
		cmd[5]=date;//数据，1 字节
		crc=crc_calculate(6,cmd);

		cmd[6]=crc>>8;
		cmd[7]=crc;

		

}


#if FILTER_F5_55

int del_f5_55(unsigned char *date,int len)
{
			unsigned short i=0,j=0;
			unsigned char temp[128];
			memcpy(temp,date,sizeof(TCL_UART_HEAD));
			
			for(i=sizeof(TCL_UART_HEAD),j=sizeof(TCL_UART_HEAD);i<len;i++)
				{
					if(0xf5==date[i]&&0x55==date[i+1])
						{
       temp[j++]=date[i];
       i++;
						}
					else
						{
      temp[j++]=date[i];
						}

				}
			memcpy(date,temp,j);

			return j;


}


int add_f5_55(unsigned char *date,int len)
{
			unsigned short i=0,j=0;
			unsigned char temp[128];
			memcpy(temp,date,sizeof(TCL_UART_HEAD));
			
			for(i=sizeof(TCL_UART_HEAD),j=sizeof(TCL_UART_HEAD);i<len;i++)
				{
					if(0xf5==date[i])
						{
       temp[j++]=date[i];
       temp[j++]=0x55;
						}
					else
						{
      temp[j++]=date[i];
						}

				}
			memcpy(date,temp,j);

			return j;


}

void find_f5_55(unsigned char *date,unsigned char *source,int len)
{
		unsigned short i;
		unsigned short j;
		date[0]=source[0];
		for(i=1,j=1;i<len;i++)
			{
				if(0x55==source[i]&&0xf5==source[i-1])
					{
					}
				else 
					{
     date[j++]=source[i];
					}
			}

}
#endif

static unsigned int synced_tcl_time = 0, synced_tcl_utc_time = 0;

void clear_Tcl_UTC()
{
  synced_tcl_utc_time = 0;
}

void set_Tcl_UTC(unsigned int t)
{
  
 // unsigned int t;
  
  //sscanf(str, "%d", &t);
  synced_tcl_time = mico_get_time();
  synced_tcl_utc_time = t;
  
  //    setDeviceTime(t);
  
}

unsigned int get_Tcl_UTC()
{
  
  return synced_tcl_utc_time + (mico_get_time() - synced_tcl_time)/1000;
  
}

int need_Tcl_SyncUTC()
{
  int now;
  if (synced_tcl_utc_time == 0)
    return 1;
  
  now = mico_get_time();
  if (now < synced_tcl_time)
    return 1;
  
  // require sync utc time every 30 mins
  if ((now - synced_tcl_time) / 1000 > (30 * 60))
    return 1;
  
  return 0;
  
}

void tcl_wifi_reset(void)
{
 wifi_net_status(CF);

PlatformEasyLinkButtonClickedCallback();//进入一键配置并且重启

}

unsigned char water_add_check(unsigned char * date)
{
  if(date==NULL)
  	return 0;
  int i,crc=0;
  unsigned char * buf=date;
  IotWaterHead_status *h=(IotWaterHead_status *)date;
  
  if(0xa5!=buf[sizeof(IotWaterHead_status)])
  	{
  	return 1;
  	}
  
  if(h->len>64)
  	return 0;
  buf=date+sizeof(IotWaterHead_status);
  for(i=1;i<h->len-2;i++)
  	{
				crc+=buf[i];
  	}
  	crc=crc%256;
		if(crc==buf[h->len-2]&&0xF5==buf[h->len-1])
			return 1;
}
void tcl_uart_process(unsigned char *buf, int len, struct sockaddr_t *paddr)
{

if(g_user_uart_stats.rece_flag&&(g_mySendBuf[sizeof(TCL_UART_HEAD)]==FRAME_STATUS||g_mySendBuf[sizeof(TCL_UART_HEAD)]==FRAME_UPLOAD))
   {
        if( memcmp(g_mySendBuf,g_mySendBuf_last,g_mySendBuf[3]+sizeof(TCL_UART_HEAD))!=0)
         {
           memcpy(g_mySendBuf_last,g_mySendBuf,g_mySendBuf[3]+sizeof(TCL_UART_HEAD));
           g_user_uart_stats.flag=1;
           
           // status_to_control();
         }

            if(g_user_uart_stats.flag)
          {
             memset(buf,0,sizeof(buf));
             if(g_user_uart_stats.status==0x0d)
             {
                  //  tcl_data_tpyeid_ack(buf,512,&(g_userconfig.addr));

             }
             else
             {
                       tcl_data_upload(buf,512,&(g_userconfig.addr));
              #if JOYLINK_ON
            joylink_server_upload_req();
              #endif
             }
                                                
          g_user_uart_stats.flag=0;

          }

   }else if(g_mySendBuf[sizeof(TCL_UART_HEAD)]==FRAME_WARN)
    {
    memset(buf,0,sizeof(buf));
    tcl_data_warning(buf,512,&(g_userconfig.addr));
    g_user_uart_stats.flag=0;
  //  extern void tcl_device_warning_ack(void);


    }else if(FRAME_RESET==g_mySendBuf[sizeof(TCL_UART_HEAD)])
     {
     tcl_wifi_reset();//进入一键配置并且重启


     }


}
void tcl_water_condition_control()
{

	IotWaterAll_Cmd * p=(IotWaterAll_Cmd *)g_myControl_uart;
 IotWaterAll_status *date=(IotWaterAll_status *)g_mySendBuf_last;
 
	 p->switcH=date->switcH;
	 p->model=date->model;
	 p->power=date->power;
	 p->set_temper=date->set_temper;
	 

}
#if TCL_WATER_CONDITION

void uart_water_process(unsigned char *buf, int len, struct sockaddr_t *paddr)
{
    if(buf==NULL)
    	return ;
    IotWaterHead_status * h=(IotWaterHead_status *)g_mySendBuf;
    
    switch (htons(h->cmd))
    	{
    				case WATER_CMD :
					    					if(memcmp(g_mySendBuf_last,g_mySendBuf,UART_LEN_WATER_CONDITION)!=0)
					    						{
					    						memcpy(g_mySendBuf_last,g_mySendBuf,UART_LEN_WATER_CONDITION);
					    						tcl_water_condition_control();
					    						g_user_uart_stats.flag=1;
					    						}
    					
				    					if(g_user_uart_stats.flag)
																{
																   memset(buf,0,sizeof(buf));
																   if(g_user_uart_stats.status==0x0d)
																   {
																        //  tcl_data_tpyeid_ack(buf,512,&(g_userconfig.addr));

																   }
																   else
																   {
																             tcl_data_upload(buf,512,&(g_userconfig.addr));
																    #if JOYLINK_ON
																  joylink_server_upload_req();
																    #endif
																   }
																                                      
																g_user_uart_stats.flag=0;

																}
			    					  break;

			    					case WATER_RESET :
             water_wifi_reset_ack();
          //   tcl_wifi_reset();//进入一键配置并且重启

			    						break;
			    						
             case WATER_DEVICE_INFO :
  //            water_wifi_reset_ack();
             // tcl_wifi_reset();//进入一键配置并且重启
             
              break;


								default:
								break;
    	}


}
#endif
void get_tcl_uart_date_water(unsigned char * source,int len)
{
		if(source==NULL||len>LEN_OF_TCL_DEVICE)
			{
					return ;
			}
		unsigned char * buf=source;
		IotWaterHead *head=(IotWaterHead *)buf;
     unsigned char i;
     unsigned char j;
  
  
  for(i=0;i<len;i++)
       { 
						  g_mySendBuf_temp[g_user_uart_stats.mybuf_len]=buf[i]; 
						  							if(0xA5==buf[i])
						  								{
																	g_user_uart_stats.status=UART_RECE_STATUS_ING;
						  								}
						  							if(0xF5==buf[i])
						  								{
																	g_user_uart_stats.status=UART_RECE_STATUS_END;
						  								}
					
						  
											   if(0x00==buf[i])
					          {
					          		if(g_user_uart_stats.date_len)//如果是在数据区，则数据长度加1
					          			{

					          			}
					           g_user_uart_stats.f5_len++;
					          }else if(0x03==buf[i]&&3==g_user_uart_stats.f5_len&&UART_RECE_STATUS_END==g_user_uart_stats.status)
					          	{
                g_mySendBuf_temp[0]=0x00;
                g_mySendBuf_temp[1]=0x00; 
                g_mySendBuf_temp[2]=0x00;
                g_mySendBuf_temp[3]=0x03;

                g_user_uart_stats.f5_len=0;
                g_user_uart_stats.mybuf_len=3;

					          	}
					         else
					          {
					           g_user_uart_stats.f5_len=0;
					          }
              if(5==g_user_uart_stats.mybuf_len&&0x03==g_mySendBuf_temp[3]&&0x00==g_mySendBuf_temp[2]&&0x00==g_mySendBuf_temp[1]&&0x00==g_mySendBuf_temp[0])
               {
               g_user_uart_stats.date_len=g_mySendBuf_temp[g_user_uart_stats.mybuf_len];
               if(g_user_uart_stats.date_len>64)
                {
                g_user_uart_stats.rece_status=UART_RECE_STATUS_END;
                g_user_uart_stats.date_len=0;
                g_user_uart_stats.mybuf_len=0;
                return;
                }
               }
              g_user_uart_stats.mybuf_len++;

					         
              if(g_user_uart_stats.date_len&&g_user_uart_stats.mybuf_len==(sizeof(TCL_UART_HEAD)+2+g_user_uart_stats.date_len))

              	{
              	if(water_add_check(g_mySendBuf_temp))
              		{
																	memcpy(g_mySendBuf,g_mySendBuf_temp,g_user_uart_stats.mybuf_len);
																	printf("get_uart:\r\n");
																	for(j=0;j<g_user_uart_stats.mybuf_len;j++)
																		{
																			printf("%02X ",g_mySendBuf[j]);
																		}
																	printf("\r\n");
																	
              		}

              	}
              if(g_user_uart_stats.mybuf_len>64)

              	{
               g_user_uart_stats.rece_status=UART_RECE_STATUS_END;
               g_user_uart_stats.date_len=0;
               g_user_uart_stats.mybuf_len=0;

              	}
 
													//	 if()

          
       }


  
  

		
		
		


}



void get_tcl_uart_date(uint8 * source,int len)//新协议的数据获取
{
		TCL_UART_HEAD *p;
		unsigned char * buf=source;
		p=(TCL_UART_HEAD *)buf;
		unsigned char i=0;
		unsigned char len_cpy=0;
		
		#if FILTER_F5_55
		unsigned char date[128];
		find_f5_55(date,source,len);
		buf=date;
		#endif
		
		if(len>127)
				return ;
		if(p->head==0xf5f5)
			{
					if(len==(p->len+sizeof(TCL_UART_HEAD)))
						{
						   if(buf[4]==0x02||buf[4]==0x07)
						   	{
														printf("\r\n");
													for(i=0;i<len;i++)
														{
															printf("%02x ",buf[i]);
														}
													if(crc_check(len,buf))
														{
																memcpy(g_mySendBuf,buf,len);
                tcl_device_warning_ack();
																g_user_uart_stats.rece_flag=1;
																server_log("g_user_uart_stats.rece_flag=1");
														}
						   	}
						   else if(buf[4]==0x04)		//报警
						   	{
		           printf("\r\n");
		          for(i=0;i<len;i++)
		           {
		            printf("%02x ",buf[i]);
		           }
		          if(crc_check(len,buf))
		           {
		             memcpy(g_mySendBuf,buf,len);
               tcl_device_warning_ack();
		            // g_user_uart_stats.rece_flag=1;
		             server_log("g_user_uart_stats.rece_flag=2");
		           }


						   	}
						}
					else if(len<(p->len+sizeof(TCL_UART_HEAD)))
						{
								memcpy(g_mySendBuf_temp,buf,len);
								
						}
					else if(len==2*(p->len+sizeof(TCL_UART_HEAD)))//  len>(p->len+sizeof(TCL_UART_HEAD))
						{
								if(buf[p->len+sizeof(TCL_UART_HEAD)+4]==0x02||buf[p->len+sizeof(TCL_UART_HEAD)+4]==0x07)
									{
								  if(crc_check(p->len+sizeof(TCL_UART_HEAD),&buf[p->len+sizeof(TCL_UART_HEAD)]))
								  	{
		         memcpy(g_mySendBuf,&buf[p->len+sizeof(TCL_UART_HEAD)],p->len+sizeof(TCL_UART_HEAD));
           tcl_device_warning_ack();
		         g_user_uart_stats.rece_flag=1;
		         server_log("g_user_uart_stats.rece_flag=3");

								  	}
									}

						}
					else
						{
	
		          if(crc_check(p->len+sizeof(TCL_UART_HEAD),buf))
		           {
		             memcpy(g_mySendBuf,buf,p->len+sizeof(TCL_UART_HEAD));
               tcl_device_warning_ack();
		            // g_user_uart_stats.rece_flag=1;
		             server_log("g_user_uart_stats.rece_flag=4");							
					            printf("\r\n");
					          for(i=0;i<p->len+sizeof(TCL_UART_HEAD);i++)
					           {
					            printf("%02x ",g_mySendBuf[i]);
					           }
		           }
		          
									
										if(((TCL_UART_HEAD *)&buf[p->len+sizeof(TCL_UART_HEAD)])->head==0xf5f5)
											{
           memcpy(g_mySendBuf_temp,&buf[p->len+sizeof(TCL_UART_HEAD)],len-(p->len+sizeof(TCL_UART_HEAD)));
           			if(crc_check(g_mySendBuf_temp[3]+sizeof(TCL_UART_HEAD),g_mySendBuf_temp)&&(g_mySendBuf_temp[4]==FRAME_STATUS||g_mySendBuf_temp[4]==FRAME_UPLOAD))
           				{
               tcl_device_warning_ack();
                memcpy(g_mySendBuf,g_mySendBuf_temp,g_mySendBuf_temp[3]+sizeof(TCL_UART_HEAD));		    
		                server_log("g_user_uart_stats.rece_flag=5");							
							            printf("\r\n");
							          for(i=0;i<g_mySendBuf_temp[3]+sizeof(TCL_UART_HEAD);i++)
							           {
							            printf("%02x ",g_mySendBuf[i]);
							           }
           				}

											}

						}
			}
		else
			{
					if(g_user_uart_stats.rece_flag==1)
						{
						  if(len<(g_mySendBuf_last[3]+sizeof(TCL_UART_HEAD)))
						  	{
										  memcpy(g_mySendBuf_temp+g_mySendBuf_last[3]+sizeof(TCL_UART_HEAD)-len,buf,len);
										  if(crc_check(len,g_mySendBuf_temp))
										  	{
														memcpy(g_mySendBuf,g_mySendBuf_temp,g_mySendBuf_last[3]+sizeof(TCL_UART_HEAD));
              server_log("g_user_uart_stats.rece_flag=6");
																		printf("\r\n");
                  for(i=0;i<(g_mySendBuf_last[3]+sizeof(TCL_UART_HEAD));i++)
                   {
                    printf("%02x ",g_mySendBuf[i]);
                   }

										  	}
						  	}
						  else if(len>(g_mySendBuf_last[3]+sizeof(TCL_UART_HEAD)))
						  	{
												if(((TCL_UART_HEAD *)&buf[len-(g_mySendBuf_last[3]+sizeof(TCL_UART_HEAD))])->head==0xf5f5&&(buf[len-g_mySendBuf_last[3]]==0x02||buf[len-g_mySendBuf_last[3]]==0x07))
													{
													  if(crc_check(g_mySendBuf_last[3]+sizeof(TCL_UART_HEAD),&buf[len-g_mySendBuf_last[3]-sizeof(TCL_UART_HEAD)]))
												  	{
						         memcpy(g_mySendBuf,&buf[len-g_mySendBuf_last[3]-sizeof(TCL_UART_HEAD)],g_mySendBuf_last[3]+sizeof(TCL_UART_HEAD));
						         g_user_uart_stats.rece_flag=1;
						         server_log("g_user_uart_stats.rece_flag=7");
                    printf("\r\n");
               for(i=0;i<(g_mySendBuf_last[3]+sizeof(TCL_UART_HEAD));i++)
                {
                 printf("%02x ",g_mySendBuf[i]);
                }

												  	}											

													}
						  	}

						}
						
			}
 		

}


void get_tcl_uart_date_new(unsigned char * buf,int len)
{

   unsigned char i=0;
   unsigned char j=0;
   for(i=0;i<len;i++)
   	{
   			if(UART_RECE_STATUS_END==g_user_uart_stats.rece_status)
   				{
          if(0xf5==buf[i])
          	{
												g_mySendBuf_temp[g_user_uart_stats.mybuf_len]=buf[i];
												g_user_uart_stats.mybuf_len++;
          	}
          else
          	{
												g_user_uart_stats.mybuf_len=0;
          	}
          
          if(2==g_user_uart_stats.mybuf_len)
          	{
												g_user_uart_stats.rece_status=UART_RECE_STATUS_END;
          	}
          
   				}
   			else if(UART_RECE_STATUS_ING==g_user_uart_stats.rece_status)
   				{
										g_mySendBuf_temp[g_user_uart_stats.mybuf_len]=buf[i];
										if(3==g_user_uart_stats.mybuf_len)
											{
											g_user_uart_stats.date_len=g_mySendBuf_temp[g_user_uart_stats.mybuf_len];
											if(g_user_uart_stats.date_len>64)
												{
												g_user_uart_stats.rece_status=UART_RECE_STATUS_END;
												return;
												}
											}
										if(0xf5==buf[i]&&g_user_uart_stats.date_len)
											{
												g_user_uart_stats.date_len++;
												g_user_uart_stats.f5_len++;
											}
										else
											{
												
											}
										
   				}
   	}





}

void get_tcl_uart_date_temp(unsigned char * buf,int len)
{

								unsigned char i;
								unsigned char j;
     for(i=0;i<len;i++)
     	{

          if(0xf5==buf[i])
          {
          		if(g_user_uart_stats.date_len)//如果是在数据区，则数据长度加1
          			{
          			#if FILTER_F5_55
														g_user_uart_stats.date_len++;
          			#endif
          			}
           g_user_uart_stats.f5_len++;
          }
         else
          {
           g_user_uart_stats.f5_len=0;
          }
         
										if(2==g_user_uart_stats.f5_len)
											{
												g_mySendBuf_temp[0]=0xf5;
												g_mySendBuf_temp[1]=0xf5;
												g_user_uart_stats.f5_len=0;
												g_user_uart_stats.mybuf_len=1;
											}
         
								 g_mySendBuf_temp[g_user_uart_stats.mybuf_len]=buf[i];	

								 
         if(3==g_user_uart_stats.mybuf_len&&0xf5==g_mySendBuf_temp[0]&&0xf5==g_mySendBuf_temp[1])
          {
          g_user_uart_stats.date_len=g_mySendBuf_temp[g_user_uart_stats.mybuf_len];
          if(g_user_uart_stats.date_len>64)
           {
           g_user_uart_stats.rece_status=UART_RECE_STATUS_END;
           g_user_uart_stats.date_len=0;
           g_user_uart_stats.mybuf_len=0;
           return;
           }
          }
         g_user_uart_stats.mybuf_len++;
         if(g_user_uart_stats.date_len&&g_user_uart_stats.mybuf_len==(sizeof(TCL_UART_HEAD)+g_user_uart_stats.date_len))
         	{
											if(crc_check(g_user_uart_stats.mybuf_len,g_mySendBuf_temp))
												{
												#if FILTER_F5_55
											g_user_uart_stats.mybuf_len=	del_f5_55(g_mySendBuf_temp,g_user_uart_stats.mybuf_len);
												#endif
															memcpy(g_mySendBuf,g_mySendBuf_temp,g_user_uart_stats.mybuf_len);
												if(0x02==g_mySendBuf[4]||0x07==g_mySendBuf[4])
													{
													g_user_uart_stats.rece_flag=1;		
													}
															tcl_device_warning_ack();
														printf("\r\n");
														for(j=0;j<g_user_uart_stats.mybuf_len;j++)
															{
																printf("%02x ",g_mySendBuf[j]);
															}
														printf("\r\n");
														server_log("rece_uart_date");
														g_user_uart_stats.date_len=0;
														g_user_uart_stats.f5_len=0;
														g_user_uart_stats.mybuf_len=0;
												//		return;
												}
         	}
     	}


}



void init_control_uart(void)
{
 
 uint16 crc = 0;
 int i = 0;
// uint8 g_myControl_uart[21];
 
 for (i = 0; i < 21; i++)
 {
  g_myControl_uart[i] = 0;
 }
 
 g_myControl_uart[0] = 0xF5;
 g_myControl_uart[1] = 0xF5;
 g_myControl_uart[2] = 0xF5;
 g_myControl_uart[3] = 0xF5;
 
 g_myControl_uart[4] = 0x00;  //源地址 
 g_myControl_uart[5] = 0x01;  //目标地址WIFI模块地址为0x00，主控板地址为0x01
 
 g_myControl_uart[6] = 0x03;   //功能码4表示查询
 g_myControl_uart[7] = 11;   //数据区长度
 
 g_myControl_uart[8] = 0x01;
 g_myControl_uart[9] = 0x00;
 
 g_myControl_uart[10] = 0x01;
 
 crc = crc_calculate(19, g_myControl_uart);
 
 g_myControl_uart[19] = crc >> 8;
 g_myControl_uart[20] = crc & 0x00FF;


}

void init_control_water_uart(void)
{
 IotWaterAll_Cmd *p=(IotWaterAll_Cmd *)g_myControl_uart;
 unsigned short crc=0;
 unsigned char i;
	memset(g_myControl_uart,0,LEN_OF_CONR_WATER);
 
	p->pro_ver=0x03000000;
	p->len=0x1800;
	p->cmd=0x0103;
	p->head=0xA5;
	p->version_h=0x00;
        p->version_l=0x01;
	p->type=0x01;
	g_myControl_uart[LEN_OF_CONR_WATER-1]=0xf5;

	printf("\r\n");
	printf("(IotWaterAll_Cmd)==%d,IotWaterHead==%d,IotWaterAll_status==%d,IotWaterStatus==%d",sizeof(IotWaterAll_Cmd),sizeof(IotWaterHead),sizeof(IotWaterAll_status),sizeof(IotWaterStatus));
	printf("\r\n");

}


void status_to_control(void)
{
 uint16 crc = 0;


g_myControl_uart[0] = 0xF5;
g_myControl_uart[1] = 0xF5;
g_myControl_uart[2] = 0xF5;
g_myControl_uart[3] = 0xF5;

g_myControl_uart[4] = 0x00;  //源地址 
g_myControl_uart[5] = 0x01;  //目标地址WIFI模块地址为0x00，主控板地址为0x01

g_myControl_uart[6] = 0x03;   //功能码4表示查询
g_myControl_uart[7] = 11;   //数据区长度

g_myControl_uart[P_UART_F300+0] = 0x01;//P0 版本号高位
g_myControl_uart[P_UART_F300+1] = 0x00;//P1版本号低位

g_myControl_uart[P_UART_F300+2] = g_mySendBuf_last[P_UART_F300+3];//P2开关机
g_myControl_uart[P_UART_F300+3] = g_mySendBuf_last[P_UART_F300+4];//工作模式
g_myControl_uart[P_UART_F300+4] = g_mySendBuf_last[P_UART_F300+5];//功能设定
g_myControl_uart[P_UART_F300+5] = g_mySendBuf_last[P_UART_F300+6];//风量选择
g_myControl_uart[P_UART_F300+6] = g_mySendBuf_last[P_UART_F300+7];//定时选择
g_myControl_uart[P_UART_F300+7] = 0x01;//保留
g_myControl_uart[P_UART_F300+8] = 0x01;//保留
g_myControl_uart[P_UART_F300+9] = 0x01;//保留
g_myControl_uart[P_UART_F300+10] = 0x00;//网络信息

crc = crc_calculate(19, g_myControl_uart);

g_myControl_uart[19] = crc >> 8;
g_myControl_uart[20] = crc & 0x00FF;

//iot_uart_output(g_myControl_uart, 21);




}

void set_control_to_uart(unsigned short cmd,unsigned char vaule)
{
 uint16 crc = 0;


 
 
 g_myControl_uart[0] = 0xF5;
 g_myControl_uart[1] = 0xF5;
 g_myControl_uart[2] = 0xF5;
 g_myControl_uart[3] = 0xF5;
 
 g_myControl_uart[4] = 0x00;  //源地址 
 g_myControl_uart[5] = 0x01;  //目标地址WIFI模块地址为0x00，主控板地址为0x01
 
 g_myControl_uart[6] = 0x03;   //功能码4表示查询
 g_myControl_uart[7] = 11;   //数据区长度
 
 g_myControl_uart[P_UART_F300+0] = 0x01;//P0 版本号高位
 g_myControl_uart[P_UART_F300+1] = 0x00;//P1版本号低位
 
 g_myControl_uart[P_UART_F300+2] = 0;//P2开关机
 g_myControl_uart[P_UART_F300+3] = 0;//工作模式
 g_myControl_uart[P_UART_F300+4] = 0;//功能设定
 g_myControl_uart[P_UART_F300+5] = 0;//风量选择
 g_myControl_uart[P_UART_F300+6] = 0;//定时选择
 g_myControl_uart[P_UART_F300+7] = 0x00;//保留
 g_myControl_uart[P_UART_F300+8] = 0x00;//保留
 g_myControl_uart[P_UART_F300+9] = 0x00;//保留
 g_myControl_uart[P_UART_F300+10] = 0x00;//网络信息

				switch(cmd)
				{
	

						case CON_AIR_LV:
       g_myControl_uart[P_UART_F300+3]=0;
							g_myControl_uart[P_UART_F300+5]=vaule;
							break;

							
       case CON_JIASHI:
       	
        g_myControl_uart[P_UART_F300+4]=0;
        g_myControl_uart[P_UART_F300+4]=vaule<<4;
        break;


      case CON_TIME:
      	g_myControl_uart[P_UART_F300+6]=vaule;
      	break;

      	case CON_AUTO:
        g_myControl_uart[P_UART_F300+5]=0;
        g_myControl_uart[P_UART_F300+3]=vaule;
      		break;
      		
        default:
              break;

				}
    crc = crc_calculate(19, g_myControl_uart);
    g_myControl_uart[19] = crc >> 8;
    g_myControl_uart[20] = crc & 0x00FF;

			
   iot_uart_output(g_myControl_uart, 21);
}


void power_on_TCL210(void)
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

	cmd[6] = 0x03;			//功能码4表示查询
	cmd[7] = 11;			//数据区长度

	cmd[8] = 0x01;
	cmd[9] = 0x00;

	cmd[10] = 0x01;

	crc = crc_calculate(19, cmd);

	cmd[19] = crc >> 8;
	cmd[20] = crc & 0x00FF;

	iot_uart_output(cmd, 21);
}


void power_off_TCL210(void)
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

	cmd[6] = 0x03;   		//功能码4表示查询
	cmd[7] = 11;			//数据区长度

	cmd[8] = 0x01;
	cmd[9] = 0x00;

	cmd[10] = 0x02;

	crc = crc_calculate(19, cmd);

	cmd[19] = crc >> 8;
	cmd[20] = crc & 0x00FF;
	

	iot_uart_output(cmd, 21);
}



void power_on_TCL_AIR_CONDITION(void)
{
	uint16 crc = 0;
	int i = 0;
	uint8 cmd[LEN_OF_AIR_CONDITION_CONTROL];

	for (i = 0; i < LEN_OF_AIR_CONDITION_CONTROL; i++)
	{
		cmd[i] = 0;
	}

	cmd[0] = 0xbb;
	cmd[1] = 0x00;
	cmd[2] = 0x01;
	cmd[3] = 0x03;

	cmd[4] = 0x19;		//长度
	cmd[5] = 0x01;		//目标地址WIFI模块地址为0x00，主控板地址为0x01

	cmd[6] = 0x00;			//功能码4表示查询
	cmd[7] = 0x64;			//数据区长度

	cmd[8] = 0x08;
	cmd[9] = 0x01;

cmd[LEN_OF_AIR_CONDITION_CONTROL-1]=0xcc;

	iot_uart_output(cmd, LEN_OF_AIR_CONDITION_CONTROL);
}


void power_off_TCL_AIR_CONDITION(void)
{
  int i = 0;
  uint8 cmd[LEN_OF_AIR_CONDITION_CONTROL];
 
  for (i = 0; i < LEN_OF_AIR_CONDITION_CONTROL; i++)
  {
   cmd[i] = 0;
  }
 
  cmd[0] = 0xbb;
  cmd[1] = 0x00;
  cmd[2] = 0x01;
  cmd[3] = 0x03;
 
  cmd[4] = 0x19;  //长度
  cmd[5] = 0x01;  //目标地址WIFI模块地址为0x00，主控板地址为0x01
 
  cmd[6] = 0x00;   //功能码4表示查询
  cmd[7] = 0x60;   //数据区长度
 
  cmd[8] = 0x01;
 
 cmd[LEN_OF_AIR_CONDITION_CONTROL-1]=0xcc;
 
  iot_uart_output(cmd, LEN_OF_AIR_CONDITION_CONTROL);

}



void getYBControlStatus()
{
	
	uint16 crc = 0;
	int i = 0;
	uint8 cmd[LEN_OF_RECE_YUBA];

	for (i = 0; i < LEN_OF_RECE_YUBA; i++)
	{
		cmd[i] = 0;
	}

 cmd[0] = 0xF5;
 cmd[1] = 0xf5;
 cmd[3] = 0x23;
 cmd[4] = 0x01;


	crc = crc_calculate(LEN_OF_RECE_YUBA-2, cmd);

	cmd[LEN_OF_RECE_YUBA-2] = crc >> 8;
	cmd[LEN_OF_RECE_YUBA-1] = crc & 0x00FF;

	iot_uart_output(cmd, LEN_OF_RECE_YUBA);
	
	//my_printf("1234567890\r\n");
}
#if TCL_WATER_CONDITION
void get_device_status(void)
{
unsigned char cmd[5]={0xa5,0x00,0x01,0x02,0xf5};

iot_uart_output(cmd, 5);

}

void water_wifi_reset_ack(void)
{

unsigned char cmd[9]={0x00,0x00,0x00,0x03,0x00,0x03,0x03,0x09,0x00};

iot_uart_output(cmd, 9);


}


#else
void get_device_status(void)
{

uint16 crc = 0;
int i = 0;
uint8 cmd[13];

for (i = 0; i < 13; i++)
{
 cmd[i] = 0;
}

cmd[0] = 0xF5;
cmd[1] = 0xF5;
cmd[2] = 0x00;
cmd[3] = 0x09;//长度

cmd[4] = 0x01;  //  帧类型

cmd[5] = 0x00;  //目标地址WIFI模块地址为0x00，主控板地址为0x01
cmd[6] = 0x00;     //功能码4表示查询

cmd[7] = 0x00;   //数据区长度
cmd[8] = 0x0b;  //版本号高位

cmd[9] = 0x00;  //版本号低位

crc = crc_calculate(11, cmd);

cmd[11] = crc >> 8;
cmd[12] = crc & 0x00FF;

iot_uart_output(cmd, 13);



}
#endif

void get4in1ControlStatus()
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

	iot_uart_output(cmd, 21);
	
	
	//my_printf("1234567890\r\n");
}
void get220BControlStatus()
{
	
	uint16 crc = 0;
	int i = 0;
	uint8 cmd[12];

	for (i = 0; i < 12; i++)
	{
		cmd[i] = 0;
	}

	cmd[0] = 0xF5;
	cmd[1] = 0x02;
	cmd[2] = 0x00;
	cmd[6] = 0x00;

	crc = crc_calculate(10, cmd);

	cmd[10] = crc >> 8;
	cmd[11] = crc & 0x00FF;

	iot_uart_output(cmd, 12);
	
	//my_printf("1234567890\r\n");
}


void check_device(void* arg)
{
//if(g_userconfig.flag)
#if EN_BASE64_TO_F5F5
 get4in1ControlStatus();
#else
//get_device_status();

#endif
 g_user_uart_stats.flag=1;



}

void check_device_status(void* arg)
{
#if EN_BASE64_TO_F5F5
if(g_userconfig.flag)
 get4in1ControlStatus();
#elif TCL_WATER_CONDITION


#else
if(g_userconfig.flag)
	get_device_status();
#endif


}

void tcl_manage_link_question(unsigned char *buf, int len, struct sockaddr_t *paddr);

void data_upload_timer(void* arg)
{
 (void)(arg);
 struct sockaddr_t paddr;
 paddr=g_userconfig.addr;
 unsigned char buf[sizeof(IotHead)+sizeof(IotLinkQuestion)];
 memset(buf,0,sizeof(IotHead)+sizeof(IotLinkQuestion));
 if(g_userconfig.flag)
 tcl_manage_link_question(buf,sizeof(IotHead)+sizeof(IotLinkQuestion),&(paddr));

  

}

int iotSetDataAck(IotDataAck *d, int state)
{
	if (d == NULL)
		return 0;
	
	d->reserve = 0;
	d->state = htonl(state);
	return sizeof(IotDataAck);
}

void init_device_id(void)
{

IPStatusTypedef para;
char mac[18];


micoWlanGetIPStatus(&para, Station);
#if STR_OR_0X
formatMACAddr(mac, (char *)&para.mac);
		// #if TEST_0X01_25
		// strcpy(g_userconfig.deviceid,"00:00:00:00:00:00");
		 //#else
		 	strcpy(g_userconfig.deviceid,mac);
		// #endif
#else
getBytes(para.mac,g_userconfig.deviceid);
#endif
//formatMACAddr(mac, (char *)&para.mac);
	// getBytes
//  memcpy(g_userconfig.deviceid,para.mac,16);
  server_log("g_userconfig.deviceid== %s, para.mac==: %s", g_userconfig.deviceid,para.mac);
	 server_log("para.ip==%s",para.ip);//
#if EN_BASE64_TO_F5F5
init_control_uart();
#elif TCL_WATER_CONDITION
init_control_water_uart();

#endif

}

void ota_start()
{
      server_log("Start OTA");
    mico_system_notify_remove_all(mico_notify_WIFI_STATUS_CHANGED);
    mico_system_notify_remove_all(mico_notify_WiFI_PARA_CHANGED);
    mico_system_notify_remove_all(mico_notify_DHCP_COMPLETED);
    mico_system_notify_remove_all(mico_notify_WIFI_CONNECT_FAILED);
	  mico_system_notify_remove_all(mico_notify_EASYLINK_WPS_COMPLETED);
    //mico_system_notify_register( mico_notify_WIFI_STATUS_CHANGED, (void *)FOTA_WifiStatusHandler, NULL );
    micoWlanStopEasyLink();
	  micoWlanStopEasyLinkPlus();
    micoWlanStopAirkiss();
	msleep(10);
}




void ota_end(int recvlen)
{
#define TMP_BUF_LEN (512)
  
    int left = 0;
    uint8_t md5_recv[16];
    uint8_t md5_calc[16];
    uint8_t *tmpbuf;
    md5_context ctx;
    uint8_t mac[6], sta_ip_addr[16];
    mico_logic_partition_t* ota_partition = MicoFlashGetInfo( MICO_PARTITION_OTA_TEMP );
    uint16_t crc = 0;
    int len = 0;

    mico_Context_t* context = NULL;
    
    int filelen = recvlen - 16; // remove md5.
    server_log("udp download image finished, OTA bin len %d", filelen);
    
    tmpbuf = (uint8_t*)malloc(TMP_BUF_LEN);
    if (tmpbuf == NULL)
    {
      server_log("malloc(TMP_BUF_LEN) error\r\n");
      return;
    }
    
    uint32_t flashaddr = filelen;
    MicoFlashRead(MICO_PARTITION_OTA_TEMP, &flashaddr, (uint8_t *)md5_recv, 16);
    InitMd5( &ctx );
    flashaddr = 0;
    left = filelen;
    while(left > 0) {
        if (left > TMP_BUF_LEN) {
            len = TMP_BUF_LEN;
        } else {
            len = left;
        }
        left -= len;
        MicoFlashRead(MICO_PARTITION_OTA_TEMP, &flashaddr, (uint8_t *)tmpbuf, len);
        Md5Update( &ctx, (uint8_t *)tmpbuf, len);
 
        
        server_log("image ckeck , offset:%d,len:%d\r\n", flashaddr, len);
    }
    Md5Final( &ctx, md5_calc );

  
    
    if(memcmp(md5_calc, md5_recv, 16) != 0) {
        server_log("ERROR!! MD5 Error.");
        server_log("RX:   %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
                 md5_recv[0],md5_recv[1],md5_recv[2],md5_recv[3],
                 md5_recv[4],md5_recv[5],md5_recv[6],md5_recv[7],
                 md5_recv[8],md5_recv[9],md5_recv[10],md5_recv[11],
                 md5_recv[12],md5_recv[13],md5_recv[14],md5_recv[15]);
        server_log("Need: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
                 md5_calc[0],md5_calc[1],md5_calc[2],md5_calc[3],
                 md5_calc[4],md5_calc[5],md5_calc[6],md5_calc[7],
                 md5_calc[8],md5_calc[9],md5_calc[10],md5_calc[11],
                 md5_calc[12],md5_calc[13],md5_calc[14],md5_calc[15]);
        mico_ota_finished(-3, NULL);
        return;
    }

    server_log("OTA bin md5 check success, CRC %x. upgrading...", crc);

    context = mico_system_context_get( );
    memset(&context->flashContentInRam.bootTable, 0, sizeof(boot_table_t));
    context->flashContentInRam.bootTable.length = filelen;
    context->flashContentInRam.bootTable.start_address = ota_partition->partition_start_addr;
    context->flashContentInRam.bootTable.type = 'A';
    context->flashContentInRam.bootTable.upgrade_type = 'U';
    context->flashContentInRam.bootTable.crc = crc;
    mico_system_context_update( mico_system_context_get( ) );
    
    mico_ota_finished(0, NULL);
  
}

void set_udp_data_0X(uint8 *buf,IotHead_enum head,uint8 *data)
{
	IotHead * p;
	p=(IotHead *)buf;

 switch(head)
 	{
	 	case HEAD_HEAD:
		//	p->head=*(unsigned int*)data;
			break;

	 	case HEAD_CTRL:
			p->ctrl=*(unsigned short *)data;
			break;

		case HEAD_INFORMATIONTYPE:
			p->informationType=*(unsigned short *)data;
			break;
		
		case HEAD_DNETID:
			p->dnetid=*(unsigned short *)data;
			break;

		case HEAD_SNETID:
			p->snetid=*(unsigned short *)data;
			break;
		
		case HEAD_SN:
			p->sn=*(unsigned int *)data;
			break;
		
		case HEAD_CMD:
			p->cmd=*(unsigned short *)data;
			break;

		case HEAD_LEN:
			p->len=*(unsigned short *)data;
			break;

		default:

			break;

 	}


}

void set_udp_data_0X_int(uint8 *buf,IotHead_enum head,uint32 data)
{
	IotHead * p;
	p=(IotHead *)buf;

 switch(head)
 	{
	 	case HEAD_HEAD:
		//	p->head=htonl((unsigned int)data);
			break;

	 	case HEAD_CTRL:
		 		#if BIG_LITTLE
						p->ctrl=htons((unsigned short )data);
		 		#else
		    p->ctrl=((unsigned short )data);
		 		#endif
			break;

		case HEAD_INFORMATIONTYPE:
						#if BIG_LITTLE
						p->informationType=htons((unsigned short )data);
						#else
						p->informationType=((unsigned short )data);
						#endif
			break;
		
		case HEAD_DNETID:
			p->dnetid=(unsigned short )data;
			break;

		case HEAD_SNETID:
			p->snetid=(unsigned short )data;
			break;
		
		case HEAD_SN:
			p->sn=(unsigned int )data;
			break;
		
		case HEAD_CMD:
						#if BIG_LITTLE
						p->cmd=htons((unsigned short )data);
						#else
						p->cmd=((unsigned short )data);
						#endif
			break;

		case HEAD_LEN:
					 #if BIG_LITTLE	
							p->len=htons((unsigned short )data);
					 #else
	      p->len=((unsigned short )data);
					 #endif
			break;

		default:

			break;

 	}


}


void  clear_gateinfo(void)
{
 memset(g_userconfig.gatewayid,0,IDLENGTH);
// memset(g_userconfig.deviceid,0,IDLENGTH);
// g_userconfig.deviceNetid=0;
//g_userconfig.dnetid=0;
g_userconfig.flag=0;


}


void swap_source_dest(unsigned int * dest,unsigned int * source)
{


			unsigned int temp;
			temp=*source;
			*source=*dest;
			*dest=temp;


}
extern char at_net_speed;
extern uint32 udp_date_num;

void  x_to_char(char *out,uint8 *source)
{
	int i = 0;
	uint16	len;
	IotHead * p;
	p=(IotHead *)source;
	len=sizeof(IotHead)+ntohs(p->len);
	if(len>512)
		return;

	strcpy(out, "");
	for (i = 0; i < len; i++)//长度不固定
	{
		char buf[12];
		sprintf(buf, "%02x", source[i]);
		memcpy(out + i * 2, buf, 2); 
	}

	out[2*len] = 0;
}
#if EN_BASE64_TO_F5F5
void base64_to_f5f5(IotDataHex* buf,unsigned char n)
{

		if(buf==NULL)
			return ;
		unsigned char * p=(unsigned char *)buf;
		unsigned short * p_data16=NULL;
		unsigned int *p_data32=NULL;
		buf->VarIndex=ntohs(buf->VarIndex);
		


	server_log("buf->VarIndex==%d,buf->vaule[0]==%d",buf->VarIndex,*(p+sizeof(IotDataHex)));
		switch(buf->VarIndex)
			{
				case POWER:
															if(*(p+sizeof(IotDataHex)))
																	power_on_TCL210();
															else
																	power_off_TCL210();	
															g_user_uart_stats.status=POWER;
									break;

				case QUERY_ALL:
											     get4in1ControlStatus();

									break;

				case TYPEID:
		    // get4in1ControlStatus();
		    g_user_uart_stats.status=TYPEID;

									break;

			default:
    set_control_to_uart(buf->VarIndex,*(p+sizeof(IotDataHex)));
									break;

			}
	get4in1ControlStatus();


}
#elif TCL_WATER_CONDITION

void base64_to_water_condition_uart(unsigned char* buf,unsigned char n)
{

		if(buf==NULL)
			return ;
		IotDataHex * h=(IotDataHex *)buf;
		IotWaterAll_Cmd *p=(IotWaterAll_Cmd *)g_myControl_uart;
		unsigned char i;
		unsigned short crc=0;
		switch(htons(h->VarIndex))
			{
			case POWER :
				p->switcH=buf[sizeof(IotDataHex)]?0x5a:0x00;
				break;

				case SET_TEMPER :
				p->set_temper=htons(*(unsigned short *)&buf[sizeof(IotDataHex)])/10;
				break;

			//	case 
				

				




								default:
				break;

			}

		for(i=1;i<16;i++)
			{
				crc+=g_myControl_uart[sizeof(IotWaterHead)+i];
			}
		g_myControl_uart[sizeof(IotWaterAll_Cmd)]=crc%256;

		iot_uart_output(g_myControl_uart,LEN_OF_CONR_WATER);


}

#endif

unsigned char base64_to_uart(unsigned char * date,unsigned char len)
{
unsigned short crc;
unsigned short i;
unsigned char * buf=date;

#if FILTER_F5_55
unsigned char temp_len;
//unsigned char temp[128];

#endif

buf[0]=0xf5;
buf[1]=0xf5;//帧头

buf[2]=0x00;//控制字目前是0

buf[3]=len+3;//数据长度数据区数据加上帧类型和校验位两个字节


buf[4]=0x01;//帧类型

#if FILTER_F5_55
temp_len=add_f5_55( buf,buf[3]+2);
crc = crc_calculate(temp_len, buf);
		 #if CRC_ADD_F5_55
		 
			 	if(crc>>8==0xf5)
			 		{
								
	       buf[temp_len++]=crc >> 8;
	       buf[temp_len]=0x55;
			 		}
			 	else
			 		{
							 buf[temp_len]=crc >> 8;
			 		}
			 	
			 	if(crc & 0x00FF==0xf5)
			 		{
			 		buf[temp_len++]=crc & 0x00FF;
	       buf[temp_len]=0x55;
			 		}
			 	else
			 		{
						 buf[temp_len+1]=crc & 0x00FF;
			 		}
			 	
				 iot_uart_output(buf, temp_len+2);
				 server_log("put_uart:len==%d",temp_len+2);
		 #else
				 buf[temp_len]=crc >> 8;
				 buf[temp_len+1]=crc & 0x00FF;
				 iot_uart_output(buf, temp_len+2);
				 server_log("put_uart:len==%d",temp_len+2);
		 #endif
#else

crc = crc_calculate(buf[3]+2, buf);



buf[2+buf[3]]=crc >> 8;
buf[2+buf[3]+1]=crc & 0x00FF;
iot_uart_output(buf, buf[3]+4);
server_log("put_uart:len==%d",buf[3]+4);

#endif



#if WCZ_LOG
for(i=0;i<buf[3]+4;i++)
{
printf("%02x ",buf[i]);
}

#endif


return len;




}



unsigned char app_to_device(unsigned char type,unsigned char * date,unsigned char len)
{
unsigned short crc;
unsigned short i;
unsigned char * buf=date;

#if FILTER_F5_55
unsigned char temp_len;

#endif

buf[0]=0xf5;
buf[1]=0xf5;//帧头

buf[2]=0x00;//控制字目前是0

buf[3]=len+3;//数据长度数据区数据加上帧类型和校验位两个字节


buf[4]=type;//帧类型

#if FILTER_F5_55
temp_len=add_f5_55( buf,buf[3]+2);
crc = crc_calculate(temp_len, buf);
buf[temp_len]=crc >> 8;
buf[temp_len+1]=crc & 0x00FF;
iot_uart_output(buf, temp_len+2);
server_log("put_uart:len==%d",temp_len+2);

#else

crc = crc_calculate(buf[3]+2, buf);



buf[2+buf[3]]=crc >> 8;
buf[2+buf[3]+1]=crc & 0x00FF;
iot_uart_output(buf, buf[3]+4);
server_log("put_uart:len==%d",buf[3]+4);

#endif



#if WCZ_LOG
for(i=0;i<buf[3]+4;i++)
{
printf("%02x ",buf[i]);
}

#endif


return len;




}


void mySend(char *dt, int len, struct sockaddr_t *addr)
{
//return ;
char tem[512];
    char *base64;
   IotHead *h=(IotHead*)dt;
    if(p_base==NULL)
    	{
			server_log("error");
			return;
    	
}
    	base64=p_base;

    if (g_serudpsock == -1)
      return;
    
    if (len <= 0)
      return;
    #if BASE64_ALL
    unsigned short base64_len;
    iotBase64encode(dt,len,&base64[1]);
    
    base64_len=strlen(&base64[1]);
    base64[0]='^';
    base64[base64_len+1]='&';
    
    
    int r = sendto(g_serudpsock, base64, base64_len+2, 0, addr, sizeof(struct sockaddr_t));  

    #else
    int r = sendto(g_serudpsock, dt, len, 0, addr, sizeof(struct sockaddr_t));  

    #endif
    
    unsigned char *spcc = (unsigned char*)&addr->s_ip;
    
    server_log("send to:ip==%d.%d.%d.%d port==%d base64_len==%d,hex_len==%d, %d\r\n", 
               spcc[3], spcc[2], spcc[1], spcc[0],
               addr->s_port,base64_len+2,len, r);
    
    server_log("len==%d,base64_len==%d,base64==%d\r\nh->ctrl==0x%x h->informationType==0x%x h->dnetid==0x%x h->snetid==0x%x h->sn==0x%x h->cmd==0x%x h->len==0x%x ",len,base64_len,base64,
															  h->ctrl,
                 h->informationType,
                 h->dnetid,
                 h->snetid,
                 h->sn,
                 h->cmd,
                 h->len	);
    
    
    if (r != len)
    {					if(at_net_speed==5)
						{
								if(len<256)
								{
									   x_to_char(tem,(uint8*)dt);

									  
															server_log("%s",tem);
								}

						}
   //   server_log("\r\n--------------ERROR!!!!!-----------end of send, len:%d , r:%d\r\n", len, r);
    }
    else
    {
   //  server_log("\r\nend of send, len:%d , r:%d\r\n", len, r);
//     server_log("\send to:ip==%d.%d.%d.%d port\r\n", (unsigned char)&(addr.ip), r);
					if(at_net_speed==5)
						{
								if(len<256)
								{
									   x_to_char(tem,(uint8*)dt);

									  
															server_log("%s",tem);
								}

						}
    }
    
}


void mySend_uart(char *dt, int len, struct sockaddr_t *addr)
{
//return ;
char tem[256];
    char base64[256];
    memset(base64 ,0,256);
   IotHead *h=(IotHead*)dt;int r =0;
  //  if(p_base==NULL)
 //   	{
	//		server_log("error");
//			return;
    	
//}
   // 	base64=p_base;

    if (g_serudpsock == -1)
      return;
    
    if (len <= 0)
      return;
    #if BASE64_ALL
    unsigned short base64_len;
    iotBase64encode(dt,len,&base64[1]);
    
    base64_len=strlen(&base64[1]);
    base64[0]='^';
    base64[base64_len+1]='&';
   
    
    r = sendto(g_serudpsock, base64, base64_len+2, 0, addr, sizeof(struct sockaddr_t));  

    #else
     r = sendto(g_serudpsock, dt, len, 0, addr, sizeof(struct sockaddr_t));  

    #endif
    
     
     unsigned char *spcc = (unsigned char*)&addr->s_ip;
    
   server_log("send to:ip==%d.%d.%d.%d port==%d base64_len==%d,hex_len==%d, %d\r\n", 
              spcc[3],spcc[2],spcc[1],spcc[0],
              addr->s_port,base64_len+2,len, r);
    server_log("len==%d,base64_len==%d,base64==%d\r\nh->ctrl==0x%x h->informationType==0x%x h->dnetid==0x%x h->snetid==0x%x h->sn==0x%x h->cmd==0x%x h->len==0x%x ",len,base64_len,base64,
															  h->ctrl,
                 h->informationType,
                 h->dnetid,
                 h->snetid,
                 h->sn,
                 h->cmd,
                 h->len	
    											);
    
    if (r != len)
    {					if(at_net_speed==5)
						{
								if(len<256)
								{
									   x_to_char(tem,(uint8*)dt);

									  
															server_log("%s",tem);
								}

						}
   //   server_log("\r\n--------------ERROR!!!!!-----------end of send, len:%d , r:%d\r\n", len, r);
    }
    else
    {
   //  server_log("\r\nend of send, len:%d , r:%d\r\n", len, r);
//     server_log("\send to:ip==%d.%d.%d.%d port\r\n", (unsigned char)&(addr.ip), r);
					if(at_net_speed==5)
						{
								if(len<256)
								{
									   x_to_char(tem,(uint8*)dt);

									  
															server_log("%s",tem);
								}

						}
    }
    
}





int tcl_manage_registered(unsigned char *buf, int len, struct sockaddr_t *paddr)
{
 int   uart_datalen = 0;
 IotAddDevice *p=NULL;
 unsigned short data_len=((IotHead*)buf)->len;//数据长度
 p=(IotAddDevice*)(buf+sizeof(IotHead));
// server_log("");
 server_log("\r\np->deviceid==%s\r\np",p->deviceid);
 g_userconfig.addr=*paddr;

// if(g_userconfig.flag==1)
 	server_log("register:return 1");
		// memcpy(g_userconfig.gatewayid,(p->gatewayid),IDLENGTH);
		if(0==strcmp(p->deviceid,g_userconfig.deviceid)&&g_userconfig.flag==0)
			{
		 g_userconfig.flag=1;
		 g_userconfig.deviceNetid=p->deviceNetid;
		 g_userconfig.gatewayNetid=p->gatewayNetid;
		 g_userconfig.snetid=p->deviceNetid;
		 g_userconfig.dnetid=p->gatewayNetid;
	 	return 1;
			}
		else
			{
			return 0;

			}

}




int tcl_manage_registered_ack(unsigned char *buf, int len, struct sockaddr_t *paddr)
{
		 IotHead *p;
		 IotAddDeviceAck *ack;
		 p=(IotHead*)buf;
		 ack=(IotAddDeviceAck *)(buf+sizeof(IotHead));
		 #if BIG_LITTLE
		 p->len=htons(sizeof(IotAddDeviceAck));//数据长度
		 #else
		 p->len=(sizeof(IotAddDeviceAck));//数据长度
		 #endif
		 swap_source_dest(&p->dnetid,&p->snetid);

		 
		 //memset(ack,0,sizeof(IotAddDeviceAck));
		/// memcpy(ack->gatewayid,g_userconfig.gatewayid,IDLENGTH);
		 memcpy(ack->deviceid,g_userconfig.deviceid,IDLENGTH);
   server_log("g_userconfig.deviceid== %s, ack->deviceid==%s", g_userconfig.deviceid,ack->deviceid);
//		 ack->deviceNetid=g_userconfig.deviceNetid;
		// ack->swh=SOFTWAREVERSION_H;
		// ack->swl=SOFTWAREVERSION_L;
		ack->deviceNetid=g_userconfig.deviceNetid;
ack->gatewayNetid=g_userconfig.gatewayNetid;
   set_udp_data_0X_int(buf,HEAD_CMD,LINK_BUNDING_ACK);

		 mySend(buf,sizeof(IotHead)+sizeof(IotAddDeviceAck),paddr);


}



int tcl_manage_broadcast(unsigned char *buf, int len, struct sockaddr_t *paddr)
{
}




int tcl_manage_broadcast_ack(unsigned char *buf, int len, struct sockaddr_t *paddr)
{
		 IotHead *p;
		 IotDeviceFindAck *ack;
		 p=(IotHead*)buf;
		 ack=(IotDeviceFindAck *)(buf+sizeof(IotHead));
		 p->len=sizeof(IotDeviceFindAck);//数据长度
		 swap_source_dest(&p->dnetid,&p->snetid);
		 memset(ack,0,sizeof(IotDeviceFindAck));

   set_udp_data_0X_int(buf,HEAD_CMD,MANAGE_BROADCAST_ACK);

		 mySend(buf,sizeof(IotHead)+p->len,paddr);


}


int tcl_discovery_device(unsigned char *buf, int len, struct sockaddr_t *paddr)
{
}




int tcl_discovery_device_ack(unsigned char *buf, int len, struct sockaddr_t *paddr)
{
		 IotHead *p;
		 IotDeviceFindAck *ack;
		 p=(IotHead*)buf;
		 ack=(IotDeviceFindAck *)(buf+sizeof(IotHead));
		 #if BIG_LITTLE
		 p->len=htons(sizeof(IotDeviceFindAck));//数据长度
			#else
		 p->len=sizeof(IotDeviceFindAck);//数据长度
			#endif
		 swap_source_dest(&p->dnetid,&p->snetid);
		 memset(ack,0,sizeof(IotDeviceFindAck));

   set_udp_data_0X_int(buf,HEAD_CMD,DEFINE_DISCOVERY_ANSWER);
   ack->sw_ver[0]=SOFTWAREVERSION_H;
   ack->sw_ver[1]=SOFTWAREVERSION_L;

   ack->hw_ver[0]=SOFTWAREVERSION_H;
   ack->hw_ver[1]=SOFTWAREVERSION_L;
   sprintf(ack->devicename,"%s",DEVICE_NAME);

    memcpy(ack->deviceid,g_userconfig.deviceid,IDLENGTH);

		 mySend(buf,sizeof(IotHead)+sizeof(IotDeviceFindAck),paddr);


}



int tcl_manage_delete(unsigned char *buf, int len, struct sockaddr_t *paddr)
{
 int   uart_datalen = 0;
 IotDelDevice *p=NULL;
 unsigned short data_len=((IotHead*)buf)->len;//数据长度
 p=(IotDelDevice*)(buf+sizeof(IotHead));
 
 if(0==strcmp(p->deviceid,g_userconfig.deviceid))
 	{
	 	return 1;
 	}
 else
 	{
//		 memcpy(g_userconfig.gatewayid,(p->gatewayid),IDLENGTH);
		// g_userconfig.deviceNetid=p->deviceNetid;
		 return 0;
 	}
}




int tcl_manage_delete_ack(unsigned char *buf, int len, struct sockaddr_t *paddr)
{
		 IotHead *p;
		 IotDelDeviceAck *ack;
		 p=(IotHead*)buf;
		 ack=(IotDelDeviceAck *)(buf+sizeof(IotHead));
		 #if BIG_LITTLE
		 p->len=htons(sizeof(IotDelDeviceAck));//数据长度
		 #else
		 p->len=(sizeof(IotDelDeviceAck));//数据长度
		 #endif
		 swap_source_dest(&p->dnetid,&p->snetid);
		 memset(ack,0,sizeof(IotDelDeviceAck));
	//	 memcpy(ack->gatewayid,g_userconfig.gatewayid,IDLENGTH);
		 memcpy(ack->deviceid,g_userconfig.deviceid,IDLENGTH);
		 ack->deviceNetid=g_userconfig.deviceNetid;
   set_udp_data_0X_int(buf,HEAD_CMD,MANAGE_DELETE_ACK);

		 mySend(buf,sizeof(IotHead)+sizeof(IotDelDeviceAck),paddr);


}


void tcl_modle_reboot(unsigned char *buf, int len, struct sockaddr_t *paddr)
{
		 unsigned char *data = NULL;
 int   uart_datalen = 0;
	IotHead *h = NULL;
 IotFota fota;
 int hs;
 unsigned char *uart_data = NULL;
 
 unsigned short crc = 0;
	h = (IotHead *)buf;
 
   server_log("h->sn==%d,g_userconfig.sn==%d",h->sn,g_userconfig.sn);
      if(h->sn>g_userconfig.sn)
   	{
    // g_userconfig.sn=h->sn;
    // MicoUartSend(UART_FOR_APP,uart_data, uart_datalen);
//    USED void PlatformEasyLinkButtonClickedCallback(void)
    //   mico_system_delegate_config_will_start();
 //  tcl_wifi_reset();
   	}
//   server_log("uart_datalen==%d,len==%d",uart_datalen,h->len);

}

void tcl_modle_reboot_ack(unsigned char *buf, int len, struct sockaddr_t *paddr)
{
		IotHead *p;
  IotDataAck *ack;
		p=(IotHead*)buf;
  swap_source_dest(&p->dnetid,&p->snetid);
  p->len=sizeof(IotDataAck);


	set_udp_data_0X_int(buf,HEAD_CMD,MANAGE_REBOOT_ACK);
	set_udp_data_0X_int(buf,HEAD_LEN,sizeof(IotDataAck));
	ack=(IotDataAck*)(buf+sizeof(IotHead));
	iotSetDataAck(ack,MESSAGE_DATA_GOOD);
	mySend(buf,sizeof(IotHead)+sizeof(IotDataAck),paddr);

}



void tcl_modle_link_ack(unsigned char *buf, int len, struct sockaddr_t *paddr)
{
		IotHead *p;
  IotDataAck *ack;
		p=(IotHead*)buf;
  swap_source_dest(&p->dnetid,&p->snetid);
  p->len=sizeof(IotDataAck);


	set_udp_data_0X_int(buf,HEAD_CMD,LINK_ACK);
	set_udp_data_0X_int(buf,HEAD_LEN,sizeof(IotDataAck));
	ack=(IotDataAck*)(buf+sizeof(IotHead));
	iotSetDataAck(ack,MESSAGE_DATA_GOOD);
	mySend(buf,sizeof(IotHead)+sizeof(IotDataAck),paddr);

}



void tcl_manage_read(unsigned char *buf, int len, struct sockaddr_t *paddr)
{
		
}

void tcl_manage_read_ack(unsigned char *buf, int len, struct sockaddr_t *paddr)
{
		IotHead *p;
  IotDeviceInfo *ack;
		p=(IotHead*)buf;
  swap_source_dest(&p->dnetid,&p->snetid);


	set_udp_data_0X_int(buf,HEAD_CMD,LINK_INFORMATION_ACK);
	set_udp_data_0X_int(buf,HEAD_LEN,sizeof(IotDeviceInfo)+2);
	ack=(IotDeviceInfo*)(buf+sizeof(IotHead)+2);
	*(buf+sizeof(IotHead))=MODELTYPE;
  *(buf+sizeof(IotHead)+1)=PROTOCOLVEL;
	#if BIG_LITTLE
	ack->EncryptedParameter=htons(ENCRYPTEDPARAMETER);
	ack->EncryptedType=ENCRYPTEDTYPE;
	ack->HarewareVer=htonl(HAREWAREVER);
	//ack->ModelTpye=MODELTYPE;
//	ack->ProtocolVer=PROTOCOLVEL;
	ack->reserve=0;
	ack->SoftwareVer=htonl(SOFTWAREVER);

	#else
	ack->EncryptedParameter=ENCRYPTEDPARAMETER;
	ack->EncryptedType=ENCRYPTEDTYPE;
	ack->HarewareVer=HAREWAREVER;
//	ack->ModelTpye=MODELTYPE;
	//ack->ProtocolVer=PROTOCOLVEL;
	ack->reserve=0;
	ack->SoftwareVer=SOFTWAREVER;
#endif
	
	
	mySend(buf,sizeof(IotHead)+sizeof(IotDeviceInfo)+2,paddr);

}



void tcl_data_push(unsigned char *buf, int len, struct sockaddr_t *paddr)
{
		 IotDataHex*data = NULL;
		 IotDataStr * vaule_str=NULL;
		 unsigned char vaule_n=0;
 int   uart_datalen = 0;
	IotHead *h = NULL;
 IotFota fota;
 int hs;
 unsigned char uart_data[64];
 char * base64=NULL;
 int i=0;
 
 unsigned short crc = 0;
	h = (IotHead *)buf; 
	server_log("h->sn==%d,g_userconfig.sn==%d",h->sn,g_userconfig.sn);
	for(i=0;i<len;i++)
	printf("%02x",buf[i]);
	
memset(uart_data,0,64);
			#if EN_BASE64_TO_F5F5
		 memcpy(uart_data,buf+sizeof(IotHead),h->len);
			#elif TCL_WATER_CONDITION
		 memcpy(uart_data,buf+sizeof(IotHead),h->len);
			#else
		 memcpy(&uart_data[5],buf+sizeof(IotHead),h->len);

			#endif



   	#if ADD_FRAME_TYPE
    data=(IotDataHex*)(uart_data+1);

   	#else
				  data=(IotDataHex*)uart_data;
    #endif
				  
					 #if EN_BASE64_TO_F5F5
					 				base64_to_f5f5(data,vaule_n);
					 #elif TCL_WATER_CONDITION
				      base64_to_water_condition_uart(uart_data,h->len);
					 #else

					 uart_data[4] = 0x01;
					 DataDownWr(&uart_data[4], (h->len+1));
//					 base64_to_uart(uart_data,h->len);
					 #endif

   g_user_uart_stats.flag=1;


   //  MicoUartSend(UART_FOR_APP,uart_data, uart_datalen);
 	}
   


void tcl_data_push_ack(unsigned char *buf, int len, struct sockaddr_t *paddr)
{
		// uint16 head_cmd
		IotHead *p;
  IotDataAck *ack;
		p=(IotHead*)buf;
  swap_source_dest(&p->dnetid,&p->snetid);


	set_udp_data_0X_int(buf,HEAD_CMD,LINK_ACK);
	set_udp_data_0X_int(buf,HEAD_LEN,sizeof(IotDataAck));
	ack=(IotDataAck*)(buf+sizeof(IotHead));
	#if BIG_LITTLE
	iotSetDataAck(ack,(MESSAGE_DATA_GOOD));
	#else
	iotSetDataAck(ack,(MESSAGE_DATA_GOOD));
	#endif
	mySend(buf,sizeof(IotHead)+sizeof(IotDataAck),paddr);

}



void tcl_manage_link_question(unsigned char *buf, int len, struct sockaddr_t *paddr)
{
 int   uart_datalen = 0;
 IotHead *p;
 p=(IotHead*)buf;
 IotLinkQuestion *pdata;
 pdata=(IotLinkQuestion*)(buf+sizeof(IotHead));
 pdata->reserve=0;
 pdata->SoftWareVersion_H=0x00;
 #if BIG_LITTLE
 pdata->SoftWareVersion_L=htonl(SOFTWAREVER);
 #else
 pdata->SoftWareVersion_L=(SOFTWAREVER);
 #endif

// p->head=0xf5f5f5f5;
#if BIG_LITTLE
p->sn=htonl(++(g_userconfig.loc_sn));
#else
 p->sn=++(g_userconfig.loc_sn);
 #endif
			 #if BIG_LITTLE
			 			p->informationType=htons(TYPE_LINK);
			 			p->cmd=htons(LINK_QUESTION);
			 			p->len=htons(sizeof(IotLinkQuestion));
			 #else
			 			p->informationType=(TYPE_LINK);
			 			p->cmd=(LINK_QUESTION);
			 			p->len=(sizeof(IotLinkQuestion));
			 #endif
			 
 p->snetid=g_userconfig.snetid;
 p->dnetid=g_userconfig.dnetid;

paddr->s_port=ROUT_UDP_PORT;
 
     char *base64;
     if(p_base==NULL)
      {
    server_log("error");
    return;
      
					 }
      base64=p_base;
 
     if (g_serudpsock == -1)
       return;
     
     if (len <= 0)
       return;
     unsigned short base64_len;
     iotBase64encode(buf,len,&base64[1]);
     
     base64_len=strlen(&base64[1]);
     base64[0]='^';
     base64[base64_len]='&';
    int r = sendto(g_serudpsock, base64, base64_len+1, 0, paddr, sizeof(struct sockaddr_t));  

    server_log("send heart len:%d ip:%x, port:%d\r\n", r, paddr->s_ip, paddr->s_port);
}

void tcl_manage_link_question_ack(unsigned char *buf, int len, struct sockaddr_t *paddr)
{
		// uint16 head_cmd
uint16 from,to;
		IotHead *p;
  IotHeartbeatAck *ack;
		p=(IotHead*)buf;
		p->len=sizeof(IotHeartbeatAck);
		ack=(IotHeartbeatAck*)(buf+sizeof(IotHead));
  swap_source_dest(&p->dnetid,&p->snetid);
  memset(ack,0,sizeof(IotHeartbeatAck));
  memcpy(ack->deviceid,g_userconfig.deviceid,IDLENGTH); 
  memcpy(ack->gatewayid,g_userconfig.gatewayid,IDLENGTH);
  ack->swh=SOFTWAREVERSION_H;
  ack->swl=SOFTWAREVERSION_L;


	set_udp_data_0X_int(buf,HEAD_CMD,MANAGE_LINK_ANSWER);
//	set_udp_data_0X_int(buf,HEAD_LEN,sizeof(IotDataAck));
	mySend(buf,sizeof(IotHead)+p->len,paddr);

}

void tcl_alam_stop(unsigned char *buf, int len, struct sockaddr_t *paddr)
{
		 IotDataHex*data = NULL;
		 IotDataStr * vaule_str=NULL;
		 unsigned char vaule_n=0;
 int   uart_datalen = 0;
	IotHead *h = NULL;
 IotFota fota;
 int hs;
 unsigned char uart_data[64];
 char * base64=NULL;
 int i=0;
 
 unsigned short crc = 0;
	h = (IotHead *)buf; 
	server_log("h->sn==%d,g_userconfig.sn==%d",h->sn,g_userconfig.sn);
	for(i=0;i<len;i++)
	printf("%02x",buf[i]);
	
memset(uart_data,0,64);



		 memcpy(&uart_data[5],buf+sizeof(IotHead),h->len);






	   	#if ADD_FRAME_TYPE
	    data=(IotDataHex*)(uart_data+1);

	   	#else
					  data=(IotDataHex*)uart_data;
	    #endif
				  

				 app_to_device(FRAME_STOP_WARN,uart_data,h->len);

   g_user_uart_stats.flag=1;


   //  MicoUartSend(UART_FOR_APP,uart_data, uart_datalen);
 	}
   


#define IOT_BIT_SET(x)  (1<<x)

#if EN_BASE64_TO_F5F5

void hex_uart_to_char(unsigned char * out,unsigned char * buf)
{

unsigned short * p10=NULL;
unsigned short *p12=NULL;
//   p12=(unsigned short *)&out[12]
 // p10=(unsigned short *)&buf[10];
unsigned short fiter_time=0;
  //  			p10=&fiter_time;



	 out[0]=FRAME_STATUS;//帧类型
	 if(buf[3]==1)
	 	{
	 out[1]=1;//电源
	 	}
	 else
	 	{
   out[1]=0;//电源

	 	}
	 out[2]=buf[4]&IOT_BIT_SET(0);//自动
	 out[3]=(buf[5]&IOT_BIT_SET(4))>>4;//加湿
	 out[4]=buf[6];//风量
	 out[5]=buf[7];//定时
	 out[6]=buf[8];//空气质量高
	 out[7]=buf[9];//滤网更换
	 
	 if((1200-*(unsigned short *)&buf[10])>0)
	 	{
	 fiter_time=1200-*(unsigned short *)&buf[10];
	 *(unsigned short *)(&out[8])=htons(fiter_time);//滤网时间搞
//	 out[9]=buf[10];//滤网时间低
	 	}
	 else
	 	{
   out[8]=0;//灰传感器高
   out[9]=0;//灰传感器低

	 	}
		#if BIG_LITTLE
    out[10]=buf[13];//灰传感器高
    out[11]=buf[12];//灰传感器低
    #else
    out[10]=buf[12];//灰传感器高
    out[11]=buf[13];//灰传感器低
    #endif

 #if BIG_LITTLE
     *(unsigned short *)&out[12]=htons((unsigned short)(buf[16]-50));
#else
    *(unsigned short *)&out[12]=(unsigned short)(buf[16]-50);
#endif

	 out[14]=0;//湿度高
	 out[15]=buf[17];//湿度低
	 out[16]=buf[15];//故障
	 


}

void tcl_data_upload(unsigned char *buf, int len, struct sockaddr_t *paddr)
{
		 IotDataHex*data = NULL;
 int   uart_datalen = 0;
	IotHead *h = NULL;
 IotFota fota;
 int hs;
 unsigned char *p;
	unsigned char  uart_send[31]; 
	memcpy(uart_send,g_mySendBuf_last,31);
	uart_send[7]=FRAME_STATUS;
	uart_datalen=g_mySendBuf_last[7];
 unsigned short crc = 0;
	h = (IotHead *)buf; 
	
 // h->head=0xf5f5f5f5;
  h->sn=g_userconfig.sn++;
  h->snetid=g_userconfig.snetid;
  h->dnetid=g_userconfig.dnetid;
  
  if (g_userconfig.flag == 0)
    return;

//  h->len=htons(sizeof(IotData));
 //  data = iotGetDataPush(buf, len);
 
  // uart_datalen=h->len-hs;

 //  iotBase64encode(&(g_mySendBuf_last[5]),g_mySendBuf_last[4],base64);
 //  h->len=strlen(base64);
 #if BASE64_ALL
    p=buf+sizeof(IotHead);
 hex_uart_to_char(uart_send,&g_mySendBuf_last[8]);
   memcpy(p,&uart_send[1],STATUS_UART_LEN);
 			
 #else
// iotSetStatus(buf,len,&(uart_send[7]),uart_datalen+1);
 #endif
  // data=(IotDataHex*)(uart_data+sizeof(IotData));
  h->len=htons(STATUS_UART_LEN);

  server_log("uart_datalen==%d,len==%d",g_mySendBuf_last[7],uart_datalen+1);
  server_log("base64==%s",&buf[sizeof(IotHead)]);
  paddr->s_port=ROUT_UDP_PORT;
		g_user_uart_stats.flag=0;
	  #if BIG_LITTLE
		//	  h->len=htons(h->len);
   h->cmd=htons(REPORT_DATA);
			  mySend_uart(buf,sizeof(IotHead)+STATUS_UART_LEN,paddr);
	  #else
			  h->len=(h->len);
   h->cmd=(REPORT_DATA);
			  mySend_uart(buf,sizeof(IotHead)+uart_datalen+sizeof(IotData)+1,paddr);
	  #endif


   


}

#elif  TCL_WATER_CONDITION

void hex_uart_to_char(unsigned char * out,unsigned char * buf)
{

unsigned short * p10=NULL;
unsigned short *p12=NULL;

unsigned short fiter_time=0;
IotWaterAll_status  *p=(IotWaterAll_status *)buf;
  IotTclWaterStatus *date=(IotTclWaterStatus *)out;

  date->switcH=p->switcH;   //电源状态
  date->time_h=0;							// 当前时间
  date->time_m=0;
  date->time_s=0;

  date->water_level=0x01;
  date->current_temper_h=(10*(p->water_temper))>>8;
  date->current_temper_l=htons(10*(p->water_temper));
  date->model=p->model;
  
  date->set_temper=htons(10*(p->set_temper));
  date->power=p->power;
  date->fix_time=0;

  date->countdown=0x0000;
  //date





}

void tcl_data_upload(unsigned char *buf, int len, struct sockaddr_t *paddr)
{
		 IotDataHex*data = NULL;
 int   uart_datalen = 0;
	IotHead *h = NULL;
 IotFota fota;
 int hs;
 unsigned char *p;
 unsigned char uart_send[48];

	uart_datalen=g_mySendBuf_last[7];
 unsigned short crc = 0;
	h = (IotHead *)buf; 
	
  h->sn=g_userconfig.sn++;
  h->snetid=g_userconfig.snetid;
  h->dnetid=g_userconfig.dnetid;
  
  if (g_userconfig.flag == 0)
    return;


    p=buf+sizeof(IotHead);
 hex_uart_to_char(uart_send,g_mySendBuf_last);
   memcpy(p,uart_send,sizeof(IotTclWaterStatus)-2);
 			

  h->len=htons(sizeof(IotTclWaterStatus)-2);

  server_log("uart_datalen==%d,len==%d",sizeof(IotTclWaterStatus)-2,sizeof(IotHead)+sizeof(IotTclWaterStatus)-2);
  server_log("base64==%s",&buf[sizeof(IotHead)]);
  paddr->s_port=ROUT_UDP_PORT;
		g_user_uart_stats.flag=0;
		
   h->cmd=htons(REPORT_DATA);
			  mySend_uart(buf,sizeof(IotHead)+sizeof(IotTclWaterStatus)-2,paddr);



   


}
#else
void tcl_data_upload(unsigned char *buf, int len, struct sockaddr_t *paddr)
{
		 IotDataHex*data = NULL;
	IotHead *h = NULL;
 IotFota fota;
 int hs;
 unsigned char *p;
 
 unsigned short crc = 0;
	h = (IotHead *)buf; 

	  h->sn=g_userconfig.sn++;
  h->snetid=g_userconfig.snetid;
  h->dnetid=g_userconfig.dnetid;
  
  if (g_userconfig.flag == 0)
    return;

 #if BASE64_ALL
    p=buf+sizeof(IotHead);
   memcpy(p,g_mySendBuf_last+sizeof(TCL_UART_HEAD)+1,g_mySendBuf_last[3]-2-1);
 			
 #else
// iotSetStatus(buf,len,&(uart_send[7]),uart_datalen+1);
 #endif
  // data=(IotDataHex*)(uart_data+sizeof(IotData));
  h->len=htons(g_mySendBuf_last[3]-2-1);

  server_log("uart_datalen==%d,len==%d",g_mySendBuf_last[3],g_mySendBuf_last[3]-2-1);
  server_log("base64==%s",&buf[sizeof(IotHead)]);
  paddr->s_port=ROUT_UDP_PORT;
		g_user_uart_stats.flag=0;
	  #if BIG_LITTLE
		//	  h->len=htons(h->len);
        if(g_mySendBuf_last[4] = FRAME_WARN)
        {
            h->cmd=htons(ALAM_DATA);
        }
        else
        {
            h->cmd=htons(REPORT_DATA);
        }
			  mySend_uart(buf,sizeof(IotHead)+g_mySendBuf_last[3]-2-1,paddr);
       
	  #else
			  h->len=(h->len);
                      if(g_mySendBuf_last[4] = FRAME_WARN)
        {
             h->cmd=(ALAM_DATA);
        }
        else
        {
   h->cmd=(REPORT_DATA);
        }
			  mySend_uart(buf,sizeof(IotHead)+g_mySendBuf_last[3]-2-1,paddr);
	  #endif


}


#endif
void tcl_device_warning_ack()
{
 uint16 crc = 0;
  int i = 0;
  uint8 cmd[7];
 
  for (i = 0; i < 7; i++)
  {
   cmd[i] = 0;
  }
 
  cmd[0] = 0xF5;
  cmd[1] = 0xF5;
  cmd[2] = 0x00;
  cmd[3] = 0x03;
 
  cmd[4] = FRAME_COFIRMATION;  //源地址 


 
  crc = crc_calculate(5, cmd);
 
  cmd[5] = crc >> 8;
  cmd[6] = crc & 0x00FF;
 
  iot_uart_output(cmd, 7);


}

void tcl_data_warning(unsigned char *buf, int len, struct sockaddr_t *paddr)
{
		 IotDataHex*data = NULL;
	IotHead *h = NULL;
 IotFota fota;
 int hs;
 unsigned char *p;
 
 unsigned short crc = 0;
	h = (IotHead *)buf; 

	  h->sn=g_userconfig.sn++;
  h->snetid=g_userconfig.snetid;
  h->dnetid=g_userconfig.dnetid;
  
  if (g_userconfig.flag == 0)
    return;

 #if BASE64_ALL
    p=buf+sizeof(IotHead);
   memcpy(p,g_mySendBuf+sizeof(TCL_UART_HEAD)+1,g_mySendBuf[3]-2-1);
 			
 #else
// iotSetStatus(buf,len,&(uart_send[7]),uart_datalen+1);
 #endif
  // data=(IotDataHex*)(uart_data+sizeof(IotData));
  h->len=htons(g_mySendBuf[3]-2-1);

  server_log("uart_datalen==%d,len==%d",g_mySendBuf[3],g_mySendBuf[3]-2-1);
  server_log("base64==%s",&buf[sizeof(IotHead)]);
  paddr->s_port=ROUT_UDP_PORT;
		g_user_uart_stats.flag=0;
	  #if BIG_LITTLE
		//	  h->len=htons(h->len);
   h->cmd=htons(ALAM_DATA);
			  mySend_uart(buf,sizeof(IotHead)+g_mySendBuf[3]-2-1,paddr);
	  #else
			  h->len=(h->len);
   h->cmd=(ALAM_DATA);
			  mySend_uart(buf,sizeof(IotHead)+g_mySendBuf[3]-2-1,paddr);
	  #endif


}



void tcl_data_upload_test(unsigned char *buf, int len, struct sockaddr_t *paddr)
{
		 IotDataHex*data = NULL;
	IotHead *h = NULL;
 IotFota fota;
 int hs;
 unsigned char *p;
 unsigned char test[25];
 unsigned char i=0;
 for(i=0;i<25;i++)
 	{
  test[i]=0x01;
 	}
 
 unsigned short crc = 0;
	h = (IotHead *)buf; 

	  h->sn=g_userconfig.sn;
  h->snetid=g_userconfig.snetid;
  h->dnetid=g_userconfig.dnetid;
  
  if (g_userconfig.flag == 0)
    return;

 #if BASE64_ALL
    p=buf+sizeof(IotHead);
   memcpy(p,test,25);
 			
 #else
// iotSetStatus(buf,len,&(uart_send[7]),uart_datalen+1);
 #endif
  // data=(IotDataHex*)(uart_data+sizeof(IotData));
  h->len=htons(25);

  server_log("uart_datalen==%d,len==%d",g_mySendBuf_last[3],g_mySendBuf_last[3]-2);
  server_log("base64==%s",&buf[sizeof(IotHead)]);
  paddr->s_port=ROUT_UDP_PORT;
		g_user_uart_stats.flag=0;
	  #if BIG_LITTLE
		//	  h->len=htons(h->len);
   h->cmd=htons(REPORT_DATA);
			  mySend_uart(buf,sizeof(IotHead)+25,paddr);
	  #else
			  h->len=(h->len);
   h->cmd=(REPORT_DATA);
			  mySend_uart(buf,sizeof(IotHead)+g_mySendBuf_last[3]-2,paddr);
	  #endif


}

void tcl_data_tpyeid_ack(unsigned char *buf, int len, struct sockaddr_t *paddr)
{
		 IotDataHex*data = NULL;
 int   uart_datalen = 0;
 unsigned char * p;
	IotHead *h = NULL;
 IotFota fota;
 int hs;
	unsigned char  uart_send[31]; 
	uart_send[0]=FRAME_TYPEID;
	strcpy(&uart_send[1],(unsigned char *)(g_userconfig.deviceid));
	h = (IotHead *)buf; 
	
 // h->head=0xf5f5f5f5;
  h->sn=g_userconfig.sn;
  h->snetid=g_userconfig.snetid;
  h->dnetid=g_userconfig.dnetid;
 server_log("deviceid==%s",g_userconfig.deviceid);
 server_log("uart_send==%s",&uart_send[1]);
 server_log("deviceid_len==%d",strlen(g_userconfig.deviceid));
	 #if BASE64_ALL
	 			p=buf;
     memcpy(p,uart_send,sizeof(g_userconfig.deviceid)+1);
     h->len=sizeof(g_userconfig.deviceid);
	    
	 #else
	 	  //iotSetStatus(buf,128,uart_send,sizeof(g_userconfig.deviceid)+1);
	 #endif
  server_log("len==%d",h->len);
  server_log("base64==%s",&buf[sizeof(IotHead)]);
 paddr->s_port=ROUT_UDP_PORT;
		g_user_uart_stats.flag=0;
		 #if BIG_LITTLE
		 					  h->len=htons(h->len);
		 		   h->cmd=htons(REPORT_DATA);
		 					  mySend(buf,sizeof(IotHead)+htons(h->len),paddr);
		 #else
		 					  h->len=(h->len);
		 		   h->cmd=(REPORT_DATA);
		 					  mySend(buf,sizeof(IotHead)+(h->len),paddr);
		 #endif

   


}

int tcl_ota_data(unsigned char *buf, int len, struct sockaddr_t *paddr)
{
 
 char ack[256];
 char ackbase64[256];
  
  unsigned char *data = NULL;
  int   datalen = 0;
  IotFota fota;
  unsigned short crc = 0;
  
  data = iotGetOtaData(buf, len, &datalen, &fota);
  if (data == NULL)
    return 0;
  
  g_otaaddr = *paddr;
  int codenumber = iot_ntohl(fota.codeNumber);
  int totalnumber = iot_ntohl(fota.codeTotal);
  
  if (codenumber != g_upcodenumber)
  {
    server_log("error  codenumber:%d, need %d\r\n", codenumber, g_upcodenumber);
    return 0;
  }
  
  unsigned short crcckeck = 0; //iotCrc(datalen, data);
  printf("recv codenumber:%d,neednumber %d,crc_:%d,crcckeck:%d,type:%d,Ver:%d,datalen:%d, type:%d t:%d\r\n", 
         codenumber, g_upcodenumber, crc, crcckeck, g_codeType, g_codeVer, datalen, iot_ntohl(fota.codeType), totalnumber);
  
  
  
  
  if (codenumber == 0 && g_upgradeFlag == 0)
  {
      g_sec = 0;
      g_recvsize = 0;
      g_upgradeFlag = 1; 
   //   g_otaaddr = *paddr;
      g_codeType = iot_ntohl(fota.codeType);
      g_codeVer = iot_ntohl(fota.codeVer);
      
      ota_start();
  }
  
  
  
  
  //写flash
  uint32_t flashaddr = g_recvsize;
  MicoFlashWrite(MICO_PARTITION_OTA_TEMP, &flashaddr, data, datalen);
  

    
  int tempnumber = g_upcodenumber;
  g_upcodenumber++;
  g_recvsize += datalen;
  

  char *rsp = (char *)buf;
  
  int hs = sizeof(IotHead);
  int fsize = iotSetOtaAck(ack, 256, tempnumber, g_codeType, g_codeVer);
  iotSetHeadCmd(ack, 256, 0, LINK_UPGRADE_ACK, fsize);
  iotSetHead(ack, 256, 0, 0, 0, 0);
  int pkgsize = fsize + hs;
  int sendsize = iotDataToBase64(ack, pkgsize, ackbase64, 256);
  //wcz del  mySend(ackbase64, sendsize, &g_otaaddr);
			mySend(ack, pkgsize, &g_otaaddr);
  

  
  server_log("recv fota total:%d\r\n",  g_recvsize);
  
  
  if ( (codenumber == totalnumber - 1) && g_upgradeFlag == 1 && g_recvsize > 1024)
  {  
    ota_end(g_recvsize);
  }
  
  


}
static mico_timer_t return_easy_timer;//wccz wcz_add
static mico_timer_t check_device_timer;//wccz wcz_add
static mico_timer_t check_device_status_time;//wccz wcz_add
static mico_timer_t heard_upload_timer;//wccz wcz_add
#define EASY_TIMER_AG   500
#define CHECK_DEVICE_TIMER_AG   15000
#if EN_BASE64_TO_F5F5
#define CHECK_DEVICE_STATUS_TIMER_AG   500
#else
#define CHECK_DEVICE_STATUS_TIMER_AG   2000
#endif
#define HEARD_UPLOAD_TIMER    10000

void return_easy_link(void* arg)
{
 (void)(arg);
char buf[128];
//char device_type[12]=""
struct sockaddr_t addr;
char mac[18];
IPStatusTypedef para;
addr.s_port=30744;
addr.s_ip=g_easy_link.ip; 
server_log("addr.ip==%ld",addr.s_ip);

micoWlanGetIPStatus(&para, Station);
//void formatMACAddr(mac, para.mac);
 formatMACAddr(mac,para.mac);

g_easy_link.flag--;
sprintf(buf,"{\"msg_seq\":1,\"action\":\"smartconfigdone\",\"wifi_module_mac\":\"%s\",\"device_ type\":\"%s\"}",mac,DEVICE_TYPE);

///mySend(buf, strlen(buf), &addr);

sendto(g_serudpsock, buf,  strlen(buf), 0, &addr, sizeof(struct sockaddr_t));  

if(g_easy_link.flag==0)
mico_stop_timer(&return_easy_timer);

}
 
void head_log_printf(unsigned char * buf,struct sockaddr_t *paddr,int len)
{
 IotHead *h = (IotHead*)buf;//得到数据头

char netip[16];


inet_ntoa(netip,paddr->s_ip);
h->cmd=ntohs(h->cmd);
h->len=ntohs(h->len);
h->sn=ntohs(h->sn);



server_log("rece from:ip==%s:%d hex_len==%d\r\n h->ctrl==0x%x\r\nh->informationType==0x%x\r\nh->dnetid==0x%x\r\nh->snetid==0x%x\r\nh->sn==0x%x\r\nh->cmd==0x%x\r\nh->len==0x%x\r\n",
            netip,
            paddr->s_port,
            len,
            h->ctrl,
            h->informationType,
            h->dnetid,
            h->snetid,
            h->sn,
            h->cmd,
            h->len);


}

int iotProcess(unsigned char *buffer, int len, struct sockaddr_t *paddr,unsigned char* temp)
{
  IotHead *h = iotGetHead(buffer, len);//得到数据头
  unsigned char *buf=buffer;
  unsigned char *tem=temp;
   head_log_printf(buf,paddr, len);

  switch((h->cmd))
  	{
   case MANAGE_UPGRADE_CODE:     //OTA升级DATA_PUSH

		//	server_log("buffer==%x,temp==%x,buf==%x",buffer,temp,buf);
        tcl_ota_data(buf,len,paddr);
        break;

        
   case DATA_DATA:     //数据传输
				    tcl_data_push(buf,len,paddr);
				    tcl_modle_link_ack(buf,len,paddr);
				    #if TEST_0X01_25
				    tcl_data_upload_test(buf,len,paddr);
				    #endif
				    if(g_user_uart_stats.flag==1&&g_user_uart_stats.status==TYPEID)
				    tcl_data_tpyeid_ack(buf,len,paddr);
        break;        

   case MANAGE_BROADCAST:
				    tcl_manage_broadcast(buf,len,paddr);//
				    tcl_manage_broadcast_ack(buf,len,paddr);//

   					break;

   case DEFINE_DISCOVERY_QUESTION:
				    tcl_discovery_device(buf,len,paddr);//
				    tcl_discovery_device_ack(buf,len,paddr);//

   					break;


   case LINK_ORDER_BUNDING:
				   	if(tcl_manage_registered(buf,len,paddr))
				   		{
										tcl_manage_registered_ack(buf,len,paddr);//注意，其中改变了buf里的数据
				   		}
   					break;

    case MANAGE_MODEL_REBOOT:
				    tcl_modle_reboot(buf,len,paddr);
				    tcl_modle_link_ack(buf,len,paddr);
				    
        tcl_wifi_reset();//进入一键配置并且重启
				    break;

		//		case MANAGE_LINK_QUESTION:
				//    tcl_manage_link_question(buf,len,paddr);
				//    tcl_manage_link_question_ack(buf,len,paddr);
				//    break;

				case MANAGE_DELETE:
			     if(tcl_manage_delete(buf,len,paddr))
			     	{
					      
					       tcl_manage_delete_ack(buf,len,paddr);//注意，其中改变了buf里的数据
					       clear_gateinfo();
			     	} 
								break;


					case LINK_READ_INFORMATION:
				     tcl_manage_read(buf,len,paddr);   
		       tcl_manage_read_ack(buf,len,paddr);//注意，其中改变了buf里的数据
		      
								break;

					case ALAM_STOP:
						   tcl_alam_stop(buf,len,paddr);
						   
    	    




				default:
				break;

  	}
  return 0;
  
  
  if (h->cmd != LINK_UPGRADE_CODE)
    return 0;
  
  #if 0
  unsigned char *data = NULL;
  int   datalen = 0;
  IotFota fota;
  unsigned short crc = 0;
  
  data = iotGetOtaData(buf, len, &datalen, &fota);
  if (data == NULL)
    return 0;
  
  int codenumber = iot_ntohl(fota.codeNumber);
  int totalnumber = iot_ntohl(fota.codeTotal);
  
  if (codenumber != g_upcodenumber)
  {
    server_log("error  codenumber:%d, need %d\r\n", codenumber, g_upcodenumber);
    return 0;
  }
  
  unsigned short crcckeck = 0; //iotCrc(datalen, data);
  printf("recv codenumber:%d,neednumber %d,crc_:%d,crcckeck:%d,type:%d,Ver:%d,datalen:%d, type:%d t:%d\r\n", 
         codenumber, g_upcodenumber, crc, crcckeck, g_codeType, g_codeVer, datalen, iot_ntohl(fota.codeType), totalnumber);
  
  
  
  
  if (codenumber == 0 && g_upgradeFlag == 0)
  {
      g_sec = 0;
      g_recvsize = 0;
      g_upgradeFlag = 1; 
      g_otaaddr = *paddr;
      g_codeType = iot_ntohl(fota.codeType);
      g_codeVer = iot_ntohl(fota.codeVer);
      
      ota_start();
  }
  
  
  
  
  //写flash
  uint32_t flashaddr = g_recvsize;
  MicoFlashWrite(MICO_PARTITION_OTA_TEMP, &flashaddr, data, datalen);
  

    
  int tempnumber = g_upcodenumber;
  g_upcodenumber++;
  g_recvsize += datalen;
  

  char *rsp = (char *)buf;
  
  int hs = sizeof(IotHead);
  int fsize = iotSetOtaAck(ack, 256, tempnumber, g_codeType, g_codeVer);
  iotSetHeadCmd(ack, 256, 0, LINK_UPGRADE_ACK, fsize);
  iotSetHead(ack, 256, 0, 0, 0, 0);
  int pkgsize = fsize + hs;
  int sendsize = iotDataToBase64(ack, pkgsize, ackbase64, 256);
  mySend(ackbase64, sendsize, &g_otaaddr);
  

  
  server_log("recv fota total:%d\r\n",  g_recvsize);
  
  
  if ( (codenumber == totalnumber - 1) && g_upgradeFlag == 1 && g_recvsize > 1024)
  {  
    ota_end(g_recvsize);
  }
  #endif
  
}





extern int local_udp_fd;
#define  UDP_LEN_PACK  1024

extern void LedStatus(void);
void localUdpServer_thread(void *inContext)
{
  server_log_trace();
  OSStatus err = kUnknownErr;
  int j;
  context = inContext;
  struct sockaddr_t addr;
  int sockaddr_t_size;
  
  char ip_address[16];
  
  int local_fd = -1;
  int   addrlen = 0;
  
  
  char *buffer = (char *)malloc(UDP_LEN_PACK);
  
  if (buffer == NULL)
    return;
  wifi_net_status(SA);

  
  p_base=buffer;
  
  char *base64 = (char *)malloc(1024);
  if (base64 == NULL)
    return;

  /*Establish a TCP server fd that accept the tcp clients connections*/ 
  local_fd = socket( AF_INET, SOCK_DGRM, IPPROTO_UDP );
  require_action(IsValidSocket( local_fd ), exit, err = kNoResourcesErr );
  addr.s_ip = INADDR_ANY;
  addr.s_port = LOC_UDP_PORT;
  err = bind(local_fd, &addr, sizeof(addr));
  require_noerr( err, exit );
  
  if(at_net_speed!=1)
  server_log("Server established at port: %d, fd: %d", addr.s_port, local_fd);

		init_device_id();

  g_serudpsock = local_fd;
  local_udp_fd = local_fd;
  if(g_easy_link.flag)
  	{
    // mico_init_timer(&return_easy_timer, EASY_TIMER_AG, return_easy_link, NULL);
  // mico_start_timer(&return_easy_timer);
  	}


       mico_init_timer(&check_device_timer, CHECK_DEVICE_TIMER_AG, check_device, NULL);
   mico_start_timer(&check_device_timer);


  //去掉定时查询状态的，因为设备会主动上报        mico_init_timer(&check_device_status_time, CHECK_DEVICE_STATUS_TIMER_AG, check_device_status, NULL);
  // mico_start_timer(&check_device_status_time);

          mico_init_timer(&heard_upload_timer, HEARD_UPLOAD_TIMER, data_upload_timer, NULL);
   mico_start_timer(&heard_upload_timer);
    int read_size = 0;
  struct timeval_t tv;
  fd_set readfds;
  
  tv.tv_sec = 3;
  tv.tv_usec = 0;

   
  while(1)
  {
    
   // select(1, &readfds, NULL, NULL,NULL);
memset(buffer,0,1024);
    
    FD_ZERO(&readfds);
    FD_SET(local_fd, &readfds);  
   select(1, &readfds, NULL, NULL, &tv);
 //  if(at_net_speed!=1)
 //  server_log("localUdpServer_thread");

    /*Check tcp connection requests */
    if( ! FD_ISSET(local_fd, &readfds))
    {
  
//  if(at_net_speed!=1)
//  server_log("! FD_ISSET(local_fd)  g_upcodenumber==%d",g_upcodenumber);
			 //  LedStatus();
     server_log("timer: %ld, free mem:%d\r\n", mico_get_time(), MicoGetMemoryInfo()->free_memory);
      extern void test_flash(void);
      //test_flash();
          if (g_upcodenumber > 0)
          {
                    int hs = sizeof(IotHead);
                    
                    memset(base64,0,1024);
                    int fsize = iotSetOtaAck(base64, 1024, g_upcodenumber - 1, g_codeType, g_codeVer);
                    iotSetHeadCmd(base64, 1024, 0, LINK_UPGRADE_ACK, fsize);
                    iotSetHead(base64, 1024, 0, 0, 0, 0);
                    
                    int pkgsize = fsize + sizeof(IotHead);
                    
                    //base64转换
                 //   int sendsize = iotDataToBase64(base64, pkgsize, base64, 1024);
                    //wcz del     mySend(base64, sendsize, &g_otaaddr);
                   mySend(base64, pkgsize, &g_otaaddr);
          }

                    
      continue;
    }
 //   else
    
			    addrlen = sizeof(addr);
			    read_size = recvfrom(local_fd, buffer, UDP_LEN_PACK, 0, &addr, &addrlen);
			    
			    if (read_size <= 0)
			    	{
        server_log("UDP Server read size error");
			      continue;
			    	}

			    if(at_net_speed==1)
			    	{
			     udp_date_num+=read_size;
			     sendto(local_fd, buffer, read_size, 0,  &addr, sizeof(struct sockaddr_t));

									continue;
			    	}
			    
			    server_log("UDP Server read size %d\r\n", read_size);
			    server_log("buffer=%x,base64==%x", buffer,base64);

			    if(buffer[0]=='^')
			    	{
			    	buffer[read_size-1]=0;
			    	memset(base64,0,1024);
        read_size=iotBase64decode_str(&buffer[1],base64);

        iotProcess(base64, read_size, &addr,buffer);


			    	}
			    else
			    	{
       // udp_server_call_back(buffer,read_size,&addr);//智能家庭老协议


			    	}
			   
			 //   iotProcess(buffer, read_size, &addr,base64);
    
   }

exit:
    server_log("Exit: Local controller exit with err = %d", err);
    mico_rtos_delete_thread(NULL);
    return;
}

/******************************************
Fun: 帧处理回调函数
Input: datalen:cmd+data的长度;data:cmd与data的buf地址
Output:void
Return:0:数据没处理 1:数据已处理
******************************************/
 _Bool AppHandleCallback(unsigned char *data, unsigned char datalen)
{
	/*用户自己填写*/
	PrintfMem((unsigned char*)data, datalen);
    unsigned char buf[512] = {0};
    g_mySendBuf_last[3] = datalen+2;
    memcpy(&g_mySendBuf_last[4], data, datalen);
    tcl_data_upload(buf,512,&(g_userconfig.addr));
	return 1;
}
