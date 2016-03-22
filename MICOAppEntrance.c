/**
  ******************************************************************************
  * @file    MICOAppEntrance.c 
  * @author  William Xu
  * @version V1.0.0
  * @date    05-May-2014
  * @brief   Mico application entrance, addd user application functons and threads.
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

#include "mico.h"

#include "StringUtils.h"
#include "SppProtocol.h"
#include "cfunctions.h"
#include "cppfunctions.h"
#include "MICOAPPDefine.h"
#include "geniot.h"
//#include "mico_tcl.h"
#include "mico_tcl_ota.h"
#include "mico_tcl_joylink.h"
#include "joylink.h"

#define app_log(M, ...) custom_log("APP", M, ##__VA_ARGS__)
#define app_log_trace() custom_log_trace("APP")

volatile ring_buffer_t  rx_buffer;
volatile uint8_t        rx_data[UART_BUFFER_LENGTH];

#define udp_unicast_log(M, ...) custom_log("UDP", M, ##__VA_ARGS__)

#define LOCAL_UDP_PORT 30025
extern void localUdpServer_thread(void *inContext);//wcz wcz_add 




//#define LEN_OF_RECE_360  1000

char at_net_speed=0;


extern const platform_gpio_t            platform_gpio_pins[];



extern void localTcpServer_thread(void *inContext);

extern void remoteTcpClient_thread(void *inContext);


extern void uartRecv_thread(void *inContext);

extern OSStatus MICOStartBonjourService ( WiFi_Interface interface, app_context_t * const inContext );

uint32_t currentSecond_last;

static mico_timer_t udp_date_timer;//wccz wcz_add
static mico_timer_t led_on_off_timer;//wccz wcz_add

#define UDP_DATE_PRINTF_FREQ    1000 
#define LED_ON_OFF_FREQ    250

int32_t udp_date_num;
uint32_t tcp_date_num;

#define KB_LEN  1024

#define  X_KB_LEN   25
//uint8 ram_max[KB_LEN*X_KB_LEN];

extern void return_device_status(void);
extern  void getControlStatus(void);
uint8 test_data[20]={0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf8,0xf9,
	0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf8,0xf9
	};
void LedStatus(void);
static void udp_date_printf_call_back( void* arg )
{
  (void)(arg);
  char read_date[100];
  uint32 flashddr=0x41000;
  OSStatus err;
 static uint8 flag=1;
  LedStatus();
  if(at_net_speed==1)
  	{
//udp_unicast_log( "udp recv from %s:%d, len:%d", ip_address,addr.s_port, len );
  udp_unicast_log("udp_date==%ld udp:date %ld.%03d kb/s   tcp:date %d.%03d kb/s\n",udp_date_num,udp_date_num/1000,udp_date_num%1000,tcp_date_num/1000,tcp_date_num%1000);
//return_device_status();
   
   udp_date_num=0;
   tcp_date_num=0;
return;
  	}
  
  if(at_net_speed==2)
  	{
 //  err=MicoFlashWrite(MICO_PARTITION_APPLICATION, &flashddr, test_data, 20);
 //  app_log("MicoFlashWrite:err==%d",err); 
				 if(0)
						 	{

						 err = MicoFlashErase( MICO_PARTITION_APPLICATION, flashddr, 20 );
						 app_log("MicoFlashErase:err==%d",err); 

						 err=MicoFlashWrite(MICO_PARTITION_APPLICATION, &flashddr, test_data, 20);
						 app_log("MicoFlashWrite:err==%d",err); 
						 flag=0;
						 	}

  	}

udp_date_num=0;
tcp_date_num=0;

//getControlStatus();


}

void test_flash(void)
{
 uint8 read_date[10];
 uint32 flashddr=0;
 uint32 read_ddr=0;
 uint16 result_write=0;
 uint16 result_read=0;
 OSStatus err;

if(at_net_speed==2)
 {
 udp_unicast_log("flashddr==%x:",flashddr);
 						 err = MicoFlashErase( MICO_PARTITION_APPLICATION, flashddr, 20 );
						 app_log("MicoFlashErase:err==%d",err); 

						 err=MicoFlashWrite(MICO_PARTITION_APPLICATION, &flashddr, test_data, 20);
						 app_log("MicoFlashWrite:err==%d",err); 
//result_write= MicoFlashWrite(MICO_PARTITION_APPLICATION, &flashddr, test_data, 10);
err= MicoFlashRead(MICO_PARTITION_APPLICATION, &read_ddr, (uint8*)read_date, 10);
udp_unicast_log("read_ddr==%x write==%d,err==%d",read_ddr,result_write,err);
MicoUartSend(UART_FOR_APP,read_date, 10);
 }
if(at_net_speed==3)
{
 err=MicoFlashWrite(MICO_PARTITION_PARAMETER_1, &flashddr, test_data, 20);
 app_log("MicoFlashWrite:err==%d",err); 
 err= MicoFlashRead(MICO_PARTITION_PARAMETER_1, &read_ddr, (uint8*)read_date, 10);
 app_log("MicoFlashRead:err==%d,read_date[0]==%x,read_date[1]==%x,read_date[2]==%x,read_date[3]==%x",err,read_date[0],read_date[1],read_date[2],read_date[3]); 
 MicoUartSend(UART_FOR_APP,read_date, 10);


}
if(at_net_speed==4)
{
// err = MicoFlashDisableSecurity( MICO_PARTITION_PARAMETER_1, 0x0, 20 );
// app_log("MicoFlashDisableSecurity:err==%d",err); 
 err=MicoFlashWrite(MICO_PARTITION_PARAMETER_2, &flashddr, test_data, 20); 
 app_log("MicoFlashWrite:err==%d",err); 
 err= MicoFlashRead(MICO_PARTITION_PARAMETER_2, &read_ddr, (uint8*)read_date, 10);
 app_log("MicoFlashRead:err==%d,read_date[0]==%x,read_date[1]==%x,read_date[2]==%x,read_date[3]==%x",err,read_date[0],read_date[1],read_date[2],read_date[3]); 
 MicoUartSend(UART_FOR_APP,read_date, 10);


}



}
struct sockaddr_t g_gloab_addr;

int local_udp_fd=-1;

extern void udp_server_call_back(const void *udp_buf,  size_t buf_len,
              const struct sockaddr_t  *dest_addr);


void udp_unicast_thread(void *arg)
{
  UNUSED_PARAMETER(arg);

  OSStatus err;
  struct sockaddr_t addr;
  fd_set readfds;
  socklen_t addrLen = sizeof(addr);
  int udp_fd = -1 , len;
  char ip_address[16];
  uint8_t *buf = NULL;

  
  mico_rtc_time_t time;
  uint32_t currentSecond;

  buf = malloc(1024);
  require_action(buf, exit, err = kNoMemoryErr);

    MicoRtcGetTime(&time);
  currentSecond = time.hr*3600 + time.min*60 + time.sec;
  
  /*Establish a UDP port to receive any data sent to this port*/
  udp_fd = socket( AF_INET, SOCK_DGRM, IPPROTO_UDP );
  require_action( IsValidSocket( udp_fd ), exit, err = kNoResourcesErr );

  addr.s_ip = INADDR_ANY;/*local ip address*/
  addr.s_port = LOCAL_UDP_PORT;/*20000*/
  err = bind( udp_fd, &addr, sizeof(addr) );
  require_noerr( err, exit );
  local_udp_fd=udp_fd;

  udp_unicast_log("Open local UDP port %d", LOCAL_UDP_PORT);

  

  while(1)
  {
    FD_ZERO(&readfds);
    FD_SET(udp_fd, &readfds);
    /*
    if(uart_recv_flag)
    	{
     uart_recv_flag=0;
     udp_unicast_log("uart_recv_flag==1");


    	}*/

    require_action( select(udp_fd + 1, &readfds, NULL, NULL, NULL) >= 0, exit, err = kConnectionErr );

    /*Read data from udp and send data back */ 
    if (FD_ISSET( udp_fd, &readfds )) 
    {
      len = recvfrom(udp_fd, buf, 1024, 0, &addr, &addrLen);


	

      
      udp_date_num+=len;
      require_action( len >= 0, exit, err = kConnectionErr );

      inet_ntoa( ip_address, addr.s_ip );
    //  udp_unicast_log( "udp recv from %s:%d, len:%d", ip_address,addr.s_port, len );//wcz wcz_add
  
 g_gloab_addr=addr;//wcz wcz_add
 udp_unicast_log( "udp recv from %s:%d, len:%d", ip_address,addr.s_port, len );//wcz wcz_add
			//		 MicoUartSend(UART_FOR_APP,buf,len);//wcz wcz_add
      //    sendto( udp_fd, buf, len, 0, &addr, sizeof(struct sockaddr_t) );
       udp_server_call_back(buf, len ,&addr);
      

    }


  }
  
exit:
  if( err != kNoErr )
    udp_unicast_log("UDP thread exit with err: %d", err);
  if( buf != NULL ) free(buf);
  mico_rtos_delete_thread(NULL);
}

#define TCL_VERSION   16

/* MICO system callback: Restore default configuration provided by application */
void appRestoreDefault_callback(void * const user_config_data, uint32_t size)
{
  UNUSED_PARAMETER(size);
  application_config_t* appConfig = user_config_data;
  appConfig->configDataVer = CONFIGURATION_VERSION;
  appConfig->localServerPort = LOCAL_PORT;
  appConfig->localServerEnable = true;
  appConfig->USART_BaudRate = 9600; 
  appConfig->remoteServerEnable = true;
  sprintf(appConfig->remoteServerDomain, "10.128.229.68");
  appConfig->remoteServerPort = DEFAULT_REMOTE_SERVER_PORT;
}


void InitGPIO_PB12()
{
 platform_gpio_init(&platform_gpio_pins[MICO_GPIO_19], OUTPUT_PUSH_PULL);
 // platform_gpio_output_high(&platform_gpio_pins[MICO_GPIO_19]);
 platform_gpio_output_low(&platform_gpio_pins[MICO_GPIO_19]);
 
 mico_stop_timer(&led_on_off_timer);
  app_log("\r\nset led on\r\n");
}
 
 
 
static void LedOn()
{
  platform_gpio_output_low(&platform_gpio_pins[MICO_GPIO_19]);
}
 
static void LedOff()
{
  platform_gpio_output_high(&platform_gpio_pins[MICO_GPIO_19]);
}
 
 
 void LedStatus()
{
  static unsigned int sns = 0;
  sns++;
 
 // LedOff();
 
  if (sns % 2 == 0)
  {
    LedOff();
 }
 else
  {
    LedOn();
  }
 
}

static void led_on_off( void* arg )
{
  (void)(arg);

   static unsigned int sns = 0;
   sns++;
  
  // LedOff();
  
   if (sns % 2 == 0)
   {
     LedOff();
  }
  else
   {
     LedOn();
   }



}


void start_led_timer(void)
{
 platform_gpio_init(&platform_gpio_pins[MICO_GPIO_19], OUTPUT_PUSH_PULL);

 mico_init_timer(&led_on_off_timer, LED_ON_OFF_FREQ, led_on_off, NULL);
 mico_start_timer(&led_on_off_timer);

}
void stop_led_timer(void)
{
 LedOff();


mico_stop_timer(&led_on_off_timer);

}

int application_start(void)
{
  app_log_trace();
  OSStatus err = kNoErr;
  mico_uart_config_t uart_config;
  app_context_t* app_context;
  mico_Context_t* mico_context;
  uint8 *calloc_len;
uint32 flashddr=0x41000;
  int i;
  int j;

  /* Create application context */
  app_context = ( app_context_t *)calloc(1, sizeof(app_context_t) );
  require_action( app_context, exit, err = kNoMemoryErr );

  /* Create mico system context and read application's config data from flash */
  mico_context = mico_system_context_init( sizeof( application_config_t) );
  app_context->appConfig = mico_system_context_get_user_data( mico_context );

  /* mico system initialize */
  err = mico_system_init( mico_context );
  require_noerr( err, exit );

  /* Bonjour for service searching */
//wcz wcz_del  MICOStartBonjourService( Station, app_context );

  /* Protocol initialize */
  sppProtocolInit( app_context );

  /*UART receive thread*/
 // uart_config.baud_rate    = app_context->appConfig->USART_BaudRate;
  app_log("Free memory %d bytes", MicoGetMemoryInfo()->free_memory); 
  app_log("TCL_VERSION==%d", TCL_VERSION); 

  

  InitGPIO_PB12();
  DevParaInit();

  //calloc_len
  uart_config.baud_rate    = 9600;
  uart_config.data_width   = DATA_WIDTH_8BIT;
  uart_config.parity       = NO_PARITY;
  uart_config.stop_bits    = STOP_BITS_1;
  uart_config.flow_control = FLOW_CONTROL_DISABLED;
  if(mico_context->flashContentInRam.micoSystemConfig.mcuPowerSaveEnable == true)
    uart_config.flags = UART_WAKEUP_ENABLE;
  else
    uart_config.flags = UART_WAKEUP_DISABLE;
  ring_buffer_init  ( (ring_buffer_t *)&rx_buffer, (uint8_t *)rx_data, UART_BUFFER_LENGTH );
  MicoUartInitialize( UART_FOR_APP, &uart_config, (ring_buffer_t *)&rx_buffer );
  err = mico_rtos_create_thread(NULL, MICO_APPLICATION_PRIORITY, "UART Recv", uartRecv_thread, 0x1500, (void*)app_context );
  require_noerr_action( err, exit, app_log("ERROR: Unable to start the uart recv thread.") );
 // memset(test_data,0xf5,100*sizeof(uint8));

 /****************udp printf date ******************/
   mico_init_timer(&udp_date_timer, UDP_DATE_PRINTF_FREQ, udp_date_printf_call_back, NULL);
   mico_start_timer(&udp_date_timer);
 /****************udp printf date ******************/


 /* Local TCP server thread */
// if(app_context->appConfig->localServerEnable == true){
 //  err = mico_rtos_create_thread(NULL, MICO_APPLICATION_PRIORITY, "Local Server", localTcpServer_thread, STACK_SIZE_LOCAL_TCP_SERVER_THREAD, (void*)app_context );
  // require_noerr_action( err, exit, app_log("ERROR: Unable to start the local server thread.") );
 //}

  /* Remote TCP client thread */
 //if(app_context->appConfig->remoteServerEnable == true){
  // err = mico_rtos_create_thread(NULL, MICO_APPLICATION_PRIORITY, "Remote Client", remoteTcpClient_thread, STACK_SIZE_REMOTE_TCP_CLIENT_THREAD, (void*)app_context );
  // require_noerr_action( err, exit, app_log("ERROR: Unable to start the remote client thread.") );
// }


 /* Local UDP server thread */


  err = mico_rtos_create_thread(NULL, MICO_APPLICATION_PRIORITY, "udp_unicast", localUdpServer_thread, 0x2500, NULL );
  require_noerr_string( err, exit, "ERROR: Unable to start the UDP thread." );

#if JOYLINK_ON
 extern int joylink_ecc_contex_init(void);
 extern int joylink_dev_init();
 extern   JLDevice_t  *_g_pdev;//wcz wcz_add  
 extern   void init_g_pdev(JLDevice_t * p);
 _g_pdev = (JLDevice_t *)malloc(sizeof(JLDevice_t));
 memset((unsigned char *)_g_pdev,0,sizeof(JLDevice_t));
 
 require(_g_pdev, exit);
// memset((unsigned char *)&_g_pdev,0,sizeof(JLDevice_t));
  init_g_pdev( _g_pdev);

 joylink_ecc_contex_init();
 joylink_dev_init();

 err = mico_rtos_create_thread(NULL, MICO_APPLICATION_PRIORITY, "udp_joylink", joylink_main_loop, 0x3000, NULL );
  require_noerr_string( err, exit, "ERROR: Unable to start the UDP thread." );
#endif 
exit:
  mico_rtos_delete_thread(NULL);
  return err;
}
