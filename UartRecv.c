/**
  ******************************************************************************
  * @file    UartRecv.c 
  * @author  William Xu
  * @version V1.0.0
  * @date    05-May-2014
  * @brief   This file create a UART recv thread.
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
//#include "mico_tcl.h"
#include "mico_tcl_ota.h"
#include "geniot.h"

#define uart_recv_log(M, ...) custom_log("UART RECV", M, ##__VA_ARGS__)
#define uart_recv_log_trace() custom_log_trace("UART RECV")


static size_t _uart_get_one_packet(uint8_t* buf, int maxlen);
//#define LEN_OF_RECE_360  1000
extern USER_CONFIG g_userconfig;
extern  uint8 g_mySendBuf[LEN_OF_RECE_360]; //接收到净化器的反馈信息 
extern struct sockaddr_t g_gloab_addr;
extern  int local_udp_fd;
extern  int netstatusReq(char *outvalue, char *id);
extern void get_uart_date(uint8 * buf,int len);
extern void return_device_status(void);
extern  uint8 g_mySendBuf_temp[LEN_OF_RECE_360]; //接收到净化器的反馈信息 
extern uint8	g_mySendBuf[LEN_OF_RECE_360]; //接收到净化器的反馈信息 
extern uint8	g_mySendBuf_last[LEN_OF_RECE_360]; //接收到净化器的反馈信息
extern void tcl_data_upload(unsigned char *buf, int len, struct sockaddr_t *paddr);
extern void tcl_data_tpyeid_ack(unsigned char *buf, int len, struct sockaddr_t *paddr);

extern G_USER_UART_STATUS g_user_uart_stats;
extern void status_to_control(void);
extern void get_tcl_uart_date_temp(unsigned char * buf,int len);
#if JOYLINK_ON
extern int joylink_server_upload_req();
#endif

#if EN_BASE64_TO_F5F5
     #define UART_ONE_PACKAGE_LENGTH             256
     	void uartRecv_thread(void *inContext)
     	{
     	  uart_recv_log_trace();
     	  app_context_t *Context = inContext;
     	  int recvlen;
     	  unsigned char buf[512];
     	  int buf_len;
     	  uint8_t *inDataBuffer;
     
     	  
     	 // struct sockaddr_t addr;
     
     	  int udp_fd=-1;
     
     
     	  
     	 // udp_fd = socket( AF_INET, SOCK_DGRM, IPPROTO_UDP );
     	  
     	  inDataBuffer = malloc(UART_ONE_PACKAGE_LENGTH);
     	  require(inDataBuffer, exit);
     	  
     	  while(1) {
     	    recvlen = _uart_get_one_packet(inDataBuffer, UART_ONE_PACKAGE_LENGTH);
     	    if (recvlen <= 0)
     	      continue; 
     	    uart_recv_log("uart_recv:recvlen==%d",recvlen);
     	    #if EN_BASE64_TO_F5F5
     
     	    #else
     	     continue;
     	    #endif
     
     	    get_uart_date(inDataBuffer,recvlen);//获取串口的数据放到g_mySendBuf
     	   if( memcmp(g_mySendBuf,g_mySendBuf_last,LEN_OF_RECE_360)!=0)
     	   	{
     							memcpy(g_mySendBuf_last,g_mySendBuf,LEN_OF_RECE_360);
     							g_user_uart_stats.flag=1;
     							
     	       status_to_control();
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
     						}
     	                                        
     	                    g_user_uart_stats.flag=0;
     
     	    	}
     	    if(local_udp_fd==-1)
     	    	continue;
     
     	    //	return_device_status();
     	   // sendto( local_udp_fd, inDataBuffer, recvlen, 0, &g_gloab_addr, sizeof(struct sockaddr_t) );
     
     	 //wcz del   sppUartCommandProcess(inDataBuffer, recvlen, Context);
     	  }
     	  
     	exit:
     	  if(inDataBuffer) free(inDataBuffer);
     	  mico_rtos_delete_thread(NULL);
     	}

#elif  TCL_WATER_CONDITION   //热水器串口处理函数
  
  			 
  			 #define UART_ONE_PACKAGE_LENGTH             256


  			 		void uartRecv_thread(void *inContext)
  			 		{
  			 		uart_recv_log_trace();
  			 		app_context_t *Context = inContext;
  			 		int recvlen;
  			 		unsigned char buf[512];
  			 		int buf_len;
  			 		uint8_t *inDataBuffer;
  			 		unsigned char i=0;
  			 
  			 
  			 		// struct sockaddr_t addr;
  			 
  			 		int udp_fd=-1;
  			 
  			 
  			 
  			 		// udp_fd = socket( AF_INET, SOCK_DGRM, IPPROTO_UDP );
  			 
  			 		inDataBuffer = malloc(UART_ONE_PACKAGE_LENGTH);
  			 		require(inDataBuffer, exit);
  			 
  			 		while(1) {
  			 		recvlen = _uart_get_one_packet(inDataBuffer, 16);
  			 		//  recvlen = _uart_get_one_packet(inDataBuffer, UART_ONE_PACKAGE_LENGTH);
  			 		if (recvlen <= 0)
  			 		continue; 
  			 		uart_recv_log("uart_recv:recvlen==%d",recvlen);
  			 		printf("\r\nuart_rec:");
  			 		for(i=0;i<recvlen;i++)
  			 		{
  			 		printf("%02x ",inDataBuffer[i]);
  			 		}
  			 		printf("\r\n");
  			 
  			 		get_tcl_uart_date_water(inDataBuffer,recvlen);//获取串口的数据放到g_mySendBuf
  			 		extern void uart_water_process(unsigned char *buf, int len, struct sockaddr_t *paddr);
  			 		uart_water_process(buf,512,&(g_userconfig.addr));
  			 		if(local_udp_fd==-1)
  			 		continue;
  			 
  			 		//	return_device_status();
  			 		// sendto( local_udp_fd, inDataBuffer, recvlen, 0, &g_gloab_addr, sizeof(struct sockaddr_t) );
  			 
  			 		//wcz del   sppUartCommandProcess(inDataBuffer, recvlen, Context);
  			 		}
  			 
  			 		exit:
  			 		if(inDataBuffer) free(inDataBuffer);
  			 		mico_rtos_delete_thread(NULL);
  			 		}

#else    //新协议
	 #if UART_DATE_OLD_NEW
				 #define UART_ONE_PACKAGE_LENGTH             256
	 #else
				 #define UART_ONE_PACKAGE_LENGTH             256
	 #endif

void uartRecv_thread(void *inContext)
{
	uart_recv_log_trace();
	app_context_t *Context = inContext;
	int recvlen;
	int buf_len;
	uint8_t *inDataBuffer;
	
	// struct sockaddr_t addr;
	
	int udp_fd=-1;
	
	// udp_fd = socket( AF_INET, SOCK_DGRM, IPPROTO_UDP );
	
	inDataBuffer = malloc(UART_ONE_PACKAGE_LENGTH);
	require(inDataBuffer, exit);
	
	while(1) {
		
		recvlen = MicoUartGetLengthInBuffer( UART_FOR_APP);
		FrameHandle();
		
//		recvlen = _uart_get_one_packet(inDataBuffer, UART_ONE_PACKAGE_LENGTH);
		if (recvlen <= 0)
			continue;
		
		uart_recv_log("uart_recv:recvlen==%d",recvlen);
		
		MicoUartRecv(UART_FOR_APP, inDataBuffer, recvlen, UART_RECV_TIMEOUT);
//		PrintfMem(inDataBuffer,recvlen);
#if EN_BASE64_TO_F5F5
		unsigned char buf[512];
        		uart_recv_len=recvlen;
		uart_recv_flag=1;
		get_uart_date(inDataBuffer,recvlen);//获取串口的数据放到g_mySendBuf
		if( memcmp(g_mySendBuf,g_mySendBuf_last,LEN_OF_RECE_360)!=0)
		{
			memcpy(g_mySendBuf_last,g_mySendBuf,LEN_OF_RECE_360);
			uart_recv_change=1;
			g_user_uart_stats.flag=1;
			
			status_to_control();
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
			}
			
			g_user_uart_stats.flag=0;
			
		}
		if(local_udp_fd==-1)
        {
        ;
        }
            				
		//	return_device_status();
		// sendto( local_udp_fd, inDataBuffer, recvlen, 0, &g_gloab_addr, sizeof(struct sockaddr_t) );
		
		//wcz del   sppUartCommandProcess(inDataBuffer, recvlen, Context);
#else
		RevDataWrFifo(inDataBuffer, recvlen);
#endif
	}
	
	exit:
	if(inDataBuffer) free(inDataBuffer);
	mico_rtos_delete_thread(NULL);
}

/* Packet format: BB 00 CMD(2B) Status(2B) datalen(2B) data(x) checksum(2B)
* copy to buf, return len = datalen+10
*/
  
#endif

/* Packet format: BB 00 CMD(2B) Status(2B) datalen(2B) data(x) checksum(2B)
* copy to buf, return len = datalen+10
*/
#if UART_DATE_OLD_NEW

size_t _uart_get_one_packet(uint8_t* inBuf, int inBufLen)
{
  uart_recv_log_trace();

  int datalen;
  
  while(1) {
    if( MicoUartRecv( UART_FOR_APP, inBuf, inBufLen, 50) == kNoErr){
     uart_recv_log("kNoErr:%d\n",inBufLen);//wcz wcz_trace
      return inBufLen;
    }
   else{
     datalen = MicoUartGetLengthInBuffer( UART_FOR_APP );
//     uart_recv_log("rece:datalen==%d,inBuffLen==%d\n",datalen,inBufLen);//wcz wcz_trace
     if(datalen){
       MicoUartRecv(UART_FOR_APP, inBuf, datalen, 50);
     //  MicoUartSend(UART_FOR_APP,inBuf,datalen);
       return datalen;
     }
   }
  }
  
}

#else
size_t _uart_get_one_packet(uint8_t* inBuf, int inBufLen)
{
  uart_recv_log_trace();

  int datalen;
  
  while(1) {
    if( MicoUartRecv( UART_FOR_APP, inBuf, inBufLen, UART_RECV_TIMEOUT) == kNoErr){
     uart_recv_log("kNoErr:%d\n",inBufLen);//wcz wcz_trace
      return inBufLen;
    }
   else{
     datalen = MicoUartGetLengthInBuffer( UART_FOR_APP );
//     uart_recv_log("rece:datalen==%d,inBuffLen==%d\n",datalen,inBufLen);//wcz wcz_trace
     if(datalen){
       MicoUartRecv(UART_FOR_APP, inBuf, datalen, UART_RECV_TIMEOUT);
     //  MicoUartSend(UART_FOR_APP,inBuf,datalen);
       return datalen;
     }
   }
  }
  
}
#endif

