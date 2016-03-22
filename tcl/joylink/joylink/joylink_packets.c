#include <stdio.h>
#include "joylink_packets.h"
#include "joylink_crypt.h"
#include "auth/uECC.h"
#include "auth/aes.h"
#include "auth/crc.h"
#include "joylink.h"
#include "joylink_extern.h"

#ifdef __LINUX_UB2__
#include <stdint.h>
#endif

//#include <unistd.h>

int 
joylink_packet_lan_scan_rsp(DevScan_t *scan)
{
	sprintf((char*)_g_pdev->dev_detail, 
            "{\"mac\":\"%s\",\"productuuid\":\"ZT9X5R\",\"feedid\":\"%s\",\"devkey\":\"%s\",\"lancon\":1,\"trantype\":1}",
		_g_pdev->jlp.mac,
		_g_pdev->jlp.feedid,
		_g_pdev->jlp.pubkeyS
	);
    
    char * scan_rsp_data = joylink_package_scan("scan ok", 0,
        scan->type, _g_pdev);

    if(scan_rsp_data != NULL){
        memcpy(_g_pdev->dev_detail, scan_rsp_data, strlen(scan_rsp_data));
        log_debug("new scan rsp data:%s", scan_rsp_data);
    }

	int len = joylink_encrypt_lan_basic(
            _g_pdev->send_buff, 
            JL_MAX_PACKET_LEN, ET_NOTHING, 
            PT_SCAN, (uint8_t*)1, 
            _g_pdev->dev_detail, 
            strlen((char*)_g_pdev->dev_detail));

    if(scan_rsp_data != NULL){
        free(scan_rsp_data);
    }

	return len;
}

int 
joylink_packet_lan_write_key_rsp(void)
{
	char data[] = "{\"code\":0}";

	int len = joylink_encrypt_lan_basic(
            _g_pdev->send_buff, 
            JL_MAX_PACKET_LEN, ET_NOTHING, 
            PT_WRITE_ACCESSKEY, 
            NULL, 
            (uint8_t*)&data, 
            strlen(data));

	return len;
}

int 
joylink_packet_server_auth_rsp(void)
{
	JLAuth_t auth;
    bzero(&auth, sizeof(auth));

	auth.random_unm = 1;
 log_debug("joylink_server_st_auth");
    auth.timestamp = mico_time();
    
    log_debug("accesskey key:%s  auth.timestamp==%ld",
            _g_pdev->jlp.accesskey,auth.timestamp);

	int len = joylink_encypt_server_rsp(
            _g_pdev->send_buff,
            JL_MAX_PACKET_LEN, PT_AUTH, 
            (uint8_t*)_g_pdev->jlp.accesskey, 
            (uint8_t*)&auth, sizeof(auth));

	return len;
}

int 
joylink_packet_server_hb_req(void)
{
	JLHearBeat_t heartbeat;
    bzero(&heartbeat, sizeof(heartbeat));

	heartbeat.rssi = 1;
	heartbeat.verion = 1;
	heartbeat.timestamp = 10;

	int len = joylink_encypt_server_rsp(
            _g_pdev->send_buff, 
            JL_MAX_PACKET_LEN, PT_BEAT, 
            _g_pdev->jlp.sessionKey, 
            (uint8_t*)&heartbeat, 
            sizeof(JLHearBeat_t));

	return len;
}

int 
joylink_packet_server_upload_req(void)
{
	JLDataUpload_t data = { 0 };
	data.timestamp = 10;

	int len = joylink_encypt_server_rsp(
            _g_pdev->send_buff, 
            JL_MAX_PACKET_LEN, PT_UPLOAD, 
            _g_pdev->jlp.sessionKey, 
            (uint8_t*)&data, 
            sizeof(data));

	return len;
}

int 
joylink_packet_lan_script_ctrl_rsp(char *data, int max, JLContrl_t *ctrl)
{
    if(NULL == data || NULL == ctrl){
        return -1;
    }
    int ret = -1;
    if(ctrl->biz_code == JL_BZCODE_GET_SNAPSHOT){
        ctrl->biz_code = 104;
    }else if(ctrl->biz_code == JL_BZCODE_CTRL){
        ctrl->biz_code = 102;
    }

    int time_v = mico_time();
    memcpy(data, &time_v, 4);
    memcpy(data + 4, &ctrl->biz_code , 4);
    memcpy(data + 8, &ctrl->serial, 4);

    ret = joylink_dev_get_snap_shot(data + 12, JL_MAX_PACKET_LEN - 12);

    printf("XXXX :len:%d:%s\n", ret, data+12);
    return ret > 0 ? ret + 12 : 0;
}

int
joylink_packet_server_ota_order_rsp(JLOtaOrder_t *otaOrder)
{
    int len = 0;
    char data[100] = {0};    
    time_t tt = mico_time();
    bzero(_g_pdev->dev_detail, sizeof(_g_pdev->dev_detail));

    memcpy((char*)_g_pdev->dev_detail, &tt, 4);
    
    sprintf(data, "{\"code\":0,\"serial\":%d,\"msg\":\"success\"}", otaOrder->serial);
    memcpy((char*)_g_pdev->dev_detail + 4, data, strlen(data));

    len = joylink_encypt_server_rsp(_g_pdev->send_buff, JL_MAX_PACKET_LEN,
            PT_OTA_ORDER, (uint8_t*)_g_pdev->jlp.sessionKey, _g_pdev->dev_detail, strlen(data) + 4);

    return len;
}

int 
joylink_package_ota_upload_req(JLOtaUpload_t *otaUpload)
{    
    time_t tt = mico_time();
    char * ota_upload_rsp_data = joylink_package_ota_upload(otaUpload);
    bzero(_g_pdev->dev_detail, sizeof(_g_pdev->dev_detail));
    bzero(_g_pdev->send_buff, sizeof(_g_pdev->send_buff));
    
    memcpy((char *)_g_pdev->dev_detail, &tt, 4);
    if(ota_upload_rsp_data != NULL){
        memcpy((char *)_g_pdev->dev_detail + 4, ota_upload_rsp_data, strlen(ota_upload_rsp_data));
        log_debug("new ota upload rsp data:%s", (char *)_g_pdev->dev_detail + 4);
    }

    int len = joylink_encypt_server_rsp(_g_pdev->send_buff, JL_MAX_PACKET_LEN, 
        PT_OTA_UPLOAD, (uint8_t*)_g_pdev->jlp.sessionKey, 
        _g_pdev->dev_detail, strlen(ota_upload_rsp_data) + 4);

    if(ota_upload_rsp_data != NULL){
        free(ota_upload_rsp_data);
        ota_upload_rsp_data = NULL;
    }

    return len;
}
