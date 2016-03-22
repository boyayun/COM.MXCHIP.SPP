
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>
#include <stdarg.h>
//#include <unistd.h>
//#include <sys/ioctl.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <net/if.h>
//#include <arpa/inet.h>
//#include <pthread.h>
//#include <errno.h>
//#include <ifaddrs.h>
//#include <netdb.h>

#include "joylink.h"
#include "joylink_utils.h"
#include "joylink_packets.h"
#include "joylink_crypt.h"
#include "joylink_json.h"
#include "joylink_extern.h"
#include "joylink_sub_dev.h"



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
JLDevice_t  _g_dev = {
				    .server_socket = -1,
				    .server_st = 0,
				    .hb_lost_count = 0,
				    .lan_socket = -1,
				    .local_port = 80,

			  //    .jlp.mac= "D0:BA:E4:08:E4:6E",
#if 1
					//    .jlp.accesskey = "c3d21e02d8fca8588128ca8310f8ccee",
		 //  .jlp.feedid = "145579771096459883",
					    .jlp.devtype = E_JLDEV_TYPE_NORMAL,
					    .jlp.joylink_server = "111.206.228.17",
					    .jlp.server_port = 2014, 
#else
					    .jlp.accesskey = "9b5ccce967e4a818f1fb3f94f306d851",
					    .jlp.feedid = "144739743326280530",
					    .jlp.devtype = E_JLDEV_TYPE_NORMAL,
					    .jlp.joylink_server = "111.206.228.17",
					    .jlp.server_port = 2014, 
#endif
					//   .jlp.localkey = "0b801902756d8e13a04967010fb9a14c",
					    .jlp.uuid = "ZT9X5R",
					    .jlp.lancon = 1,
					    .jlp.trantype = 1
};

#endif

//wcz wcz_del JLDevice_t  *_g_pdev = &_g_dev;
JLDevice_t  *_g_pdev =NULL;//wcz wcz_add  
void init_g_pdev(JLDevice_t * p)
{

  
 

		if(p!=NULL)
		{
			     p->server_socket = -1;
				    p->server_st = 0;
				    p->hb_lost_count = 0;
				    p->lan_socket = -1;
				    p->local_port = 80;

					    p->jlp.devtype = E_JLDEV_TYPE_NORMAL;
					    strcpy(p->jlp.joylink_server,"111.206.228.17");
			     
					    p->jlp.server_port = 2002;
					    strcpy(p->jlp.uuid,"ZT9X5R");

					    p->jlp.lancon = 1;
					    p->jlp.trantype = 1;
					    
					    strcpy(p->jlp.feedid,"");
					    strcpy(p->jlp.accesskey,"");
					    strcpy(p->jlp.localkey,"");
					    

		}

}

extern void joylink_proc_lan();
extern void joylink_proc_server();
extern int joylink_proc_server_st();
extern int joylink_ecc_contex_init(void);
static app_context_t *context;

void init_joylink_mac(void)
{
 IPStatusTypedef para;
 ////char mac[18];
 
 micoWlanGetIPStatus(&para, Station);
 formatMACAddr(_g_pdev->jlp.mac, (char *)&para.mac);

  strcpy(_g_pdev->jlp.mac,"D0:BA:E4:07:9B:1B");

 
 log_debug("joylink_mac==%s", _g_pdev->jlp.mac);
 //strcpy(g_userconfig.deviceid,mac);





}


void 
joylink_main_loop(void *inContext)
{
      int ret;
      int err;
    context = inContext;
          struct sockaddr_in sin;
    //  memset((unsigned char *)&sin,0, sizeof(sockaddr_in));
    init_joylink_mac();

          struct timeval_t  selectTimeOut;
          static uint32_t serverTimer;
          joylink_util_timer_reset(&serverTimer);
          static int interval = 0;

          sin.s_ip = htonl(INADDR_ANY);
          //sin.sin_family = AF_INET;
          sin.s_port = (_g_pdev->local_port);
          _g_pdev->lan_socket = socket(AF_INET, SOCK_DGRM, IPPROTO_UDP);

          if (_g_pdev->lan_socket < 0){
                  printf("socket() failed!\n");
                  return;
          }

          int broadcastEnable = 1;
          #if 1//wcz del
          if (setsockopt(_g_pdev->lan_socket, 
                  SOL_SOCKET, SO_BROADCAST, 
                  (uint8_t *)&broadcastEnable, 
                  sizeof(broadcastEnable)) < 0){
                  log_error("SO_BROADCAST ERR");
      }
							#endif
          if(0 > bind(_g_pdev->lan_socket, &sin, sizeof(sin))){
                  printf("Bind lan socket error!");
      }

    while (1){
            if (joylink_util_is_time_out(serverTimer, interval)){
                    joylink_util_timer_reset(&serverTimer);
        if(joylink_dev_is_net_ok()){
            interval = joylink_proc_server_st();
        }else{
            interval = 1000 * 100;
        }
            }
//  log_error("Ã»Íø");

            fd_set  readfds;
            FD_ZERO(&readfds);
            FD_SET(_g_pdev->lan_socket, &readfds);
    
    if (_g_pdev->server_socket != -1 
           && _g_pdev->server_st > 0){
        FD_SET(_g_pdev->server_socket, &readfds);
    }

    int maxFd = (int)_g_pdev->server_socket > 
        (int)_g_pdev->lan_socket ? 
        _g_pdev->server_socket : _g_pdev->lan_socket;

    selectTimeOut.tv_usec = 0L;
    selectTimeOut.tv_sec = (long)1;

           ret = select(maxFd + 1, &readfds, NULL, NULL, &selectTimeOut);//wcz wcz_del	
           // ret = select(_g_pdev->lan_socket, &readfds, NULL, NULL, 0);//wcz wcz_add
            if (ret < 0){
            //wcz del	printf("Select ERR: %s!\r\n", strerror(errno));
                    continue;
            }else if (ret == 0){
                 // wcz wcz_del  continue;
            }

            if (FD_ISSET(_g_pdev->lan_socket, &readfds)){
        joylink_proc_lan();
            }else if((_g_pdev->server_socket != -1) && 
        (FD_ISSET(_g_pdev->server_socket, &readfds))){
        joylink_proc_server();
            }
    }


    exit:
    server_log("Exit: Local controller exit with err = %d", err);
    mico_rtos_delete_thread(NULL);
    return;
}

 void 
joylink_dev_init()
{
    joylink_dev_get_jlp_info(&_g_pdev->jlp);
    log_debug("--->feedid:%s", _g_pdev->jlp.feedid);
    log_debug("--->uuid:%s", _g_pdev->jlp.uuid);
    log_debug("--->accesskey:%s", _g_pdev->jlp.accesskey);
    log_debug("--->localkey:%s", _g_pdev->jlp.localkey);
}

int 
joylink_main_start()
{
    joylink_ecc_contex_init();
    joylink_dev_init();
//	joylink_main_loop();
	return 0;
}

#ifdef _TEST_
int 
main()
{
    joylink_main_start();
	return 0;
}
#else

#endif
