/* --------------------------------------------------
 * @brief: 
 *
 * @version: 1.0
 *
 * @date: 10/08/2015 09:28:27 AM
 *
 * --------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "joylink.h"
#include "joylink_packets.h"
#include "joylink_extern.h"
#include "joylink_json.h"
//#include "joylink_extern_json.h"
#include "joylink_ret_code.h"
#include "mico_tcl_ota.h"

typedef struct __attr_{
    char name[128];
    E_JL_DEV_ATTR_GET_CB get;
    E_JL_DEV_ATTR_SET_CB set;
}Attr_t;

typedef struct _attr_manage_{
    Attr_t wlan24g;
    Attr_t subdev;
    Attr_t wlanspeed;
    Attr_t uuid;
    Attr_t feedid;
    Attr_t accesskey;
    Attr_t localkey;
    Attr_t server_st;
	Attr_t macaddr;	
	Attr_t server_info;	
/*
 *ssid and pass_word is option for GW.
 */
	Attr_t ssid;	
	Attr_t pass_word;	

}WIFIManage_t;

WIFIManage_t _g_am, *_g_pam = &_g_am;


TCL_USER_JOYLLINK  g_flash_joylink;


void JoyLink_flash_write(uint8_t* inBuffer ,uint32_t inBufferLength)
{
 uint32_t flashaddr = 0xc00;

 MicoFlashWrite(MICO_PARTITION_PARAMETER_1, &flashaddr, inBuffer, inBufferLength);

}

void JoyLink_flash_read(uint8_t* inBuffer ,uint32_t inBufferLength)
{
 uint32_t flashaddr = 0xc00;

 MicoFlashRead(MICO_PARTITION_PARAMETER_1, &flashaddr, inBuffer, inBufferLength);

}




/**
 * brief:
 * Check dev net is ok.
 * @Param: st
 *
 * @Returns: 
 *  E_RET_TRUE
 *  E_RET_FAIL
 */
E_JLRetCode_t
joylink_dev_is_net_ok()
{
    /**
     *FIXME:must to do
     */
 int ret = E_RET_TRUE;//wcz wcz_del
// int ret = E_RET_FAIL;//wcz add
    return ret;
}

/**
 * brief:
 * When connecting server st changed,
 * this fun will be called.
 *
 * @Param: st
 * JL_SERVER_ST_INIT      (0)
 * JL_SERVER_ST_AUTH      (1)
 * JL_SERVER_ST_WORK      (2)
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
E_JLRetCode_t
joylink_dev_set_connect_st(int st)
{
    /**
     *FIXME:must to do
     */
    int ret = E_RET_OK;
    return ret;
}

/**
 * brief:
 * Save joylink protocol info in flash.
 *
 * @Param:jlp 
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */

void cle_easylink_joy(void)
{
g_flash_joylink.flag=0;

JoyLink_flash_write((unsigned char *)&g_flash_joylink,sizeof(TCL_USER_JOYLLINK));

}


E_JLRetCode_t
joylink_dev_set_attr_jlp(JLPInfo_t *jlp)
{
    if(NULL == jlp){
        return E_RET_ERROR;
    }
    /**
     *FIXME:must to do
     */

    strcpy(g_flash_joylink.feedid,jlp->feedid);
    strcpy(g_flash_joylink.accesskey,jlp->accesskey);
    strcpy(g_flash_joylink.localkey,jlp->localkey);
				g_flash_joylink.flag=1;
    JoyLink_flash_write((unsigned char *)&g_flash_joylink,sizeof(TCL_USER_JOYLLINK));

   
    int ret = E_RET_OK;

    return ret;
}

/**
 * brief:
 * get joylink protocol info.
 *
 * @Param:jlp 
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
E_JLRetCode_t
joylink_dev_get_jlp_info(JLPInfo_t *jlp)
{
    if(NULL == jlp){
        return E_RET_ERROR;
    }
    /**
     *FIXME:must to do
     */

    
    JoyLink_flash_read((unsigned char *)&g_flash_joylink,sizeof(TCL_USER_JOYLLINK));
				if(g_flash_joylink.flag==1)
					{
					strcpy(jlp->feedid,g_flash_joylink.feedid);
					strcpy(jlp->accesskey,g_flash_joylink.accesskey);
					strcpy(jlp->localkey,g_flash_joylink.localkey);
					}
    
    int ret = E_RET_ERROR;

    return ret;
}

/**
 * brief:
 * Save dev attr info to flash.
 *
 * @Param:wi 
 * "wi" is only a example, replace with dev attr.
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
E_JLRetCode_t
joylink_dev_set_attr(WIFICtrl_t *wi)
{
    if(NULL == wi){
        return -1;
    }
    /**
     *FIXME:must to do
     */
    int ret = E_RET_ERROR;

    return ret;
}

/**
 * brief:
 * Get dev snap shot.
 *
 * @Param:out_snap
 *
 * @Returns: snap shot len.
 */
 extern G_USER_UART_STATUS g_user_uart_stats;
extern  unsigned char g_mySendBuf_last[LEN_OF_RECE_360]; //接收到净化器的反馈信息 
int
joylink_dev_get_snap_shot(char *out_snap, int32_t out_max)
{
    if(NULL == out_snap || out_max < 0){
        return 0;
    }
    /**
     *FIXME:must to do
     */
    int len = 0;

    if(g_user_uart_stats.rece_flag==1)
    	{
    	len=g_mySendBuf_last[3]+sizeof(TCL_UART_HEAD);
					memcpy(out_snap,g_mySendBuf_last,len);
    	}

    return len;
}

/**
 * brief:
 * Get dev snap shot.
 *
 * @Param:out_snap
 *
 * @Returns: snap shot len.
 */
int
joylink_dev_get_json_snap_shot(char *out_snap, int32_t out_max, int code, char *feedid)
{
    /**
     *FIXME:must to do
     */
    sprintf(out_snap, "{\"code\":%d, \"feedid\":\"%s\"}", code, feedid);

    return strlen(out_snap);
}

/**
 * brief:
 * json ctrl.
 *
 * @Param:json_cmd
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
E_JLRetCode_t 
joylink_dev_lan_json_ctrl(const char *json_cmd)
{
    /**
     *FIXME:must to do
     */
    log_debug("json ctrl:%s", json_cmd);

    return E_RET_OK;
}

/**
 * brief:
 * script control.
 * @Param: 
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
 
extern unsigned short crc_calculate(int length, unsigned char *address);
extern  int  crc_check(unsigned char length, unsigned char *address);

void joylink_date_to_uart(unsigned char * date)
{
  unsigned int len=date[3]+sizeof(TCL_UART_HEAD)-2;
  unsigned char cmd[64];
  unsigned short crc;
  memcpy(cmd,date,len);
  if(len>128)
  	{
  	log_debug("len>128");
		  	return ;
  	}
  crc = crc_calculate(19, cmd);
  
  cmd[len] = crc >> 8;
  cmd[len+1] = crc & 0x00FF;
  iot_uart_output(cmd, len+2);

  



}

E_JLRetCode_t 
joylink_dev_lan_script_ctrl(const char *recPainText, JLContrl_t *ctr,int n)
{
 
 if(NULL == recPainText || NULL == ctr){
     return -1;
 }
 /**
  *FIXME:must to do
  */
 int ret = -1;
 ctr->biz_code = (int)(*((int *)(recPainText + 4)));
 ctr->serial = (int)(*((int *)(recPainText +8)));
 

// int tt = mico_time();
// log_info("bcode:%d:server:%d:time:%ld", ctr->biz_code, n,(long)tt);
 
 if(ctr->biz_code == JL_BZCODE_GET_SNAPSHOT){
     /*
      *Nothing to do!
      */
     ret = 0;
 }else if(ctr->biz_code == JL_BZCODE_CTRL){
     if(n){
        // joylink_dev_parse_wifictrl_from_server( recPainText + 12);
     }else{
        // joylink_dev_parse_wifi_ctrl( recPainText + 12);
     }
 joylink_date_to_uart((unsigned char *)(recPainText + 12));

     if(!joylink_dev_set_attr( NULL)){
         log_info("set atrr ok");
     }else{
         log_error("set attr error");
     }
     
     ret = 0;
 }else{
     log_error("unKown biz_code:%d", ctr->biz_code);
 }
 
 return ret;
 
}

/**
 * brief:
 * dev ota update
 * @Param: JLOtaOrder_t *otaOrder
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
E_JLRetCode_t
joylink_dev_ota(JLOtaOrder_t *otaOrder)
{
    if(NULL == otaOrder){
        return -1;
    }
    /**
     *FIXME:must to do
     */
    int ret = E_RET_OK;
    log_debug("serial:%d | feedid:%s | productuuid:%s | version:%d | versionname:%s | crc32:%d | url:%s\n",
     otaOrder->serial, otaOrder->feedid, otaOrder->productuuid, otaOrder->version, 
     otaOrder->versionname, otaOrder->crc32, otaOrder->url);

    return ret;
}

/**
 * brief:
 * dev ota status upload
 * @Param: 
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
void
joylink_dev_ota_status_upload()
{
    JLOtaUpload_t otaUpload;
    int i;
    strcpy(otaUpload.feedid, _g_pdev->jlp.feedid);
    strcpy(otaUpload.productuuid, _g_pdev->jlp.uuid);

    /**
     *FIXME:must to do
     *status,status_desc, progress
     */
    joylink_server_ota_status_upload_req(&otaUpload);
}

/**
 * brief:1 start a softap with ap_ssid 
 *       2 option to do it.
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
E_JLRetCode_t
joylink_dev_start_softap(char *ap_ssid)
{
    if(NULL == ap_ssid){
        return E_RET_ERROR;
    }
    int ret = E_RET_OK;
    return ret;
}

/**
 * brief:1 stop softap
 *       2 option to do it.
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
E_JLRetCode_t
joylink_dev_stop_softap()
{
    int ret = E_RET_OK;
    return ret;
}

/**
 * brief:
 * attr set and get callback register.
 * @Param:
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
int 
joylink_dev_register_attr_cb(
        const char *name,
        E_JL_DEV_ATTR_TYPE type,
        E_JL_DEV_ATTR_GET_CB attr_get_cb,
        E_JL_DEV_ATTR_SET_CB attr_set_cb)
{
    if(NULL == name){
        return -1;
    }
    int ret = -1;
    log_debug("regster %s", name);
    if(!strcmp(name, JL_ATTR_WLAN24G)){
       _g_pam->wlan24g.get = attr_get_cb; 
       _g_pam->wlan24g.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, JL_ATTR_SUBDEVS)){
       _g_pam->subdev.get = attr_get_cb; 
       _g_pam->subdev.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, JL_ATTR_WAN_SPEED)){
       _g_pam->wlanspeed.get = attr_get_cb; 
       _g_pam->wlanspeed.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, JL_ATTR_UUID)){
       _g_pam->uuid.get = attr_get_cb; 
       _g_pam->uuid.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, JL_ATTR_FEEDID)){
       _g_pam->feedid.get = attr_get_cb; 
       _g_pam->feedid.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, JL_ATTR_ACCESSKEY)){
       _g_pam->accesskey.get = attr_get_cb; 
       _g_pam->accesskey.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, JL_ATTR_LOCALKEY)){
       _g_pam->localkey.get = attr_get_cb; 
       _g_pam->localkey.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, JL_ATTR_CONN_STATUS)){
       _g_pam->server_st.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, JL_ATTR_MACADDR)){
       _g_pam->macaddr.get = attr_get_cb; 
       ret = 0;
    }

    log_info("regster %s:ret:%d", name, ret);
    return ret;
}
