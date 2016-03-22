
#include "mico.h"

#include "StringUtils.h"
#include "SppProtocol.h"
#include "cfunctions.h"
#include "cppfunctions.h"
#include "MICOAPPDefine.h"
#include "geniot.h"
//#include "mico_tcl.h"
#include "mico_tcl_ota.h"
#if 0
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
     mico_init_timer(&return_easy_timer, EASY_TIMER_AG, return_easy_link, NULL);
   mico_start_timer(&return_easy_timer);
  	}


       mico_init_timer(&check_device_timer, CHECK_DEVICE_TIMER_AG, check_device, NULL);
   mico_start_timer(&check_device_timer);
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
   if(at_net_speed!=1)
   server_log("localUdpServer_thread");

    /*Check tcp connection requests */
    if( ! FD_ISSET(local_fd, &readfds))
    {
  
  if(at_net_speed!=1)
  server_log("! FD_ISSET(local_fd)");
			   
   //   server_log("timer: %ld, free mem:%d\r\n", mico_get_time(), MicoGetMemoryInfo()->free_memory);
      extern void test_flash(void);
      test_flash();
          if (g_upcodenumber > 0)
          {
                    int hs = sizeof(IotHead);
                    
                    IotFotaAck *pf = (IotFotaAck *)(buffer + hs);
                    int fsize = iotSetOtaAck(pf, g_upcodenumber - 1, g_codeType, g_codeVer);
                    iotSetHeadDest((IotHead *)buffer, 0, 0, 0, MANAGE_UPGRADE_ACK, fsize);
                    iotSetHead((IotHead *)buffer, 0xF5F5F5F5, 0, 0);
                    int sendsize = fsize + sizeof(IotHead);
                    
                    mySend(buffer, sendsize, &g_otaaddr);
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
			   
			    iotProcess(buffer, read_size, &addr);
    
   }

exit:
    server_log("Exit: Local controller exit with err = %d", err);
    mico_rtos_delete_thread(NULL);
    return;
}
#endif

