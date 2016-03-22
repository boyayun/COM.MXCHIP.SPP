

#include "geniot.h"
#include "MICO.h"

#ifdef WIN32
#include<string.h>
#endif
#define server_log(M, ...) custom_log("guniot", M, ##__VA_ARGS__)


const char * base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


unsigned short iot_htons(unsigned short n)
{
#if LITTLEENDING
  return ((n & 0xff) << 8) | ((n & 0xff00) >> 8);
#else
	return n;
#endif
}


unsigned short iot_ntohs(unsigned short n)
{
#if LITTLEENDING
  return iot_htons(n);
#else
	return n;
#endif
}


unsigned int iot_htonl(unsigned int n)
{
#if LITTLEENDING
  return ((n & 0xff) << 24) |
    ((n & 0xff00) << 8) |
    ((n & 0xff0000UL) >> 8) |
    ((n & 0xff000000UL) >> 24);
#else
	return n;
#endif
}


unsigned int iot_ntohl(unsigned int n)
{
#if LITTLEENDING
  return iot_htonl(n);
#else
	return n;
#endif
}

char * iotBase64encode( const char * bindataArray, int binlength, char * base64)
{
    int i, j;
    unsigned char current;

	const unsigned char * bindata = (const unsigned char *)bindataArray;

    for ( i = 0, j = 0 ; i < binlength ; i += 3 )
    {
        current = (bindata[i] >> 2) ;
        current &= (unsigned char)0x3F;
        base64[j++] = base64char[(int)current];

        current = ( (unsigned char)(bindata[i] << 4 ) ) & ( (unsigned char)0x30 ) ;
        if ( i + 1 >= binlength )
        {
            base64[j++] = base64char[(int)current];
            base64[j++] = '=';
            base64[j++] = '=';
            break;
        }
        current |= ( (unsigned char)(bindata[i+1] >> 4) ) & ( (unsigned char) 0x0F );
        base64[j++] = base64char[(int)current];

        current = ( (unsigned char)(bindata[i+1] << 2) ) & ( (unsigned char)0x3C ) ;
        if ( i + 2 >= binlength )
        {
            base64[j++] = base64char[(int)current];
            base64[j++] = '=';
            break;
        }
        current |= ( (unsigned char)(bindata[i+2] >> 6) ) & ( (unsigned char) 0x03 );
        base64[j++] = base64char[(int)current];

        current = ( (unsigned char)bindata[i+2] ) & ( (unsigned char)0x3F ) ;
        base64[j++] = base64char[(int)current];
    }
    base64[j] = '\0';
    return base64;
}

int iotBase64decode_str( const char * base64, unsigned char * bindata )
{
    int i, j;
    unsigned char k;
    unsigned char temp[4];
    for ( i = 0, j = 0; base64[i] != '\0' ; i += 4 )
    {
        //memset( temp, 0xFF, sizeof(temp) );
		temp[0] = 0xFF;
		temp[1] = 0xFF;
		temp[2] = 0xFF;
		temp[3] = 0xFF;

        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i] )
                temp[0]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+1] )
                temp[1]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+2] )
                temp[2]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+3] )
                temp[3]= k;
        }

        bindata[j++] = ((unsigned char)(((unsigned char)(temp[0] << 2))&0xFC)) |
                ((unsigned char)((unsigned char)(temp[1]>>4)&0x03));
        if ( base64[i+2] == '=' )
            break;

        bindata[j++] = ((unsigned char)(((unsigned char)(temp[1] << 4))&0xF0)) |
                ((unsigned char)((unsigned char)(temp[2]>>2)&0x0F));
        if ( base64[i+3] == '=' )
            break;

        bindata[j++] = ((unsigned char)(((unsigned char)(temp[2] << 6))&0xF0)) |
                ((unsigned char)(temp[3]&0x3F));
    }
    return j;
}
int iotBase64decode( const char * base64, int base64len,  char * bindataArray)
{
    int i, j;
    unsigned char k;
    unsigned char temp[4];

	unsigned char * bindata = (unsigned char *)bindataArray;

    for ( i = 0, j = 0; i < base64len ; i += 4 )
    {
        //memset( temp, 0xFF, sizeof(temp) );
		temp[0] = 0xFF;
		temp[1] = 0xFF;
		temp[2] = 0xFF;
		temp[3] = 0xFF;

        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i] )
                temp[0]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+1] )
                temp[1]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+2] )
                temp[2]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+3] )
                temp[3]= k;
        }

        bindata[j++] = ((unsigned char)(((unsigned char)(temp[0] << 2))&0xFC)) |
                ((unsigned char)((unsigned char)(temp[1]>>4)&0x03));
        if ( base64[i+2] == '=' )
            break;

        bindata[j++] = ((unsigned char)(((unsigned char)(temp[1] << 4))&0xF0)) |
                ((unsigned char)((unsigned char)(temp[2]>>2)&0x0F));
        if ( base64[i+3] == '=' )
            break;

        bindata[j++] = ((unsigned char)(((unsigned char)(temp[2] << 6))&0xF0)) |
                ((unsigned char)(temp[3]&0x3F));
    }
    return j;
}



void iotLog(const char *s)
{
	if (IOTLOG)
	{
		IOTPRINTF(s);
	}
}



int iotGetVersion()
{
	return 11;
}



unsigned short iotCrc(int length, unsigned char *address)
{   
	  int i,j;
	  unsigned short crc;
	  crc = 0xffff;
	  for(i=0;i<length;i++)
	  {
	    crc^= *address++;
	    for(j=0;j<8;j++)
	    {
	       if(crc&0x01)
	       {
	         crc=(crc>>1)^0x8408;
	       }
	       else
		  {
		    crc >>=0x01;
		  } 
	     }   	    
	   }
	   return ~crc;
}



void iotSetHead(char *buf, int len, unsigned short ctrl, unsigned short informationType, unsigned int dnetid, unsigned int snetid)
{
	IotHead *h = iotGetHead((const char *)buf,  len);
	if (h == NULL)
		return;

	h->ctrl = iot_htons(0x0100);
	h->informationType = iot_htons(informationType);
	h->dnetid = iot_htonl(dnetid);
	h->snetid = iot_htonl(snetid);
}

void iotSetHeadCmd(char *buf, int len, unsigned int sn, unsigned short	cmd, unsigned short datalen)
{
	IotHead *h = iotGetHead((const char *)buf,  len);
	if (h == NULL)
		return;

	h->sn = iot_htonl(sn);
	h->cmd = iot_htons(cmd);
	h->len = iot_htons(datalen);
}


IotHead *iotGetHead(const char *buf, int len)
{
	IotHead *h = NULL;
	int hs = sizeof(IotHead);
	if (len < hs)
		return 0;

	if (buf == NULL)
		return 0;

	h = (IotHead *)buf;
	return h;
}

unsigned short iotGetHeadCmd(const char *buf, int len)
{
	unsigned short cmd = 0;
	IotHead *h = iotGetHead(buf, len);
	if (h == NULL)
		return 0;

	cmd = iot_ntohs(h->cmd);
	return cmd;
}


int iotSetHeart2(IotHeart2 *d, int swh, int swl)
{
	int h = sizeof(IotHeart2);
	if (d == NULL)
		return 0;

	d->reserve = 0;
	d->swh = iot_htonl(swh);
	d->swl = iot_htonl(swl);
	return h;
}


int iotSetAddDevice(IotAddDevice *d, char *deviceid, unsigned int deviceNetid, unsigned int gatewayNetID, int gatewayVer)
{
	int as = sizeof(IotAddDevice);
	if (d == NULL)
		return 0;

	IOTSTRCPY(d->deviceid, deviceid);
	d->deviceNetid = iot_htonl(deviceNetid);
	//d->gatewayNetID = iot_htonl(gatewayNetID); 
	d->gatewayVer = iot_htonl(gatewayVer); 
	return as;
}

IotAddDevice *iotGetAddDevice(const char *buf, int len)
{
	int is = sizeof(IotAddDevice);
	if (len < is)
		return NULL;

	return (IotAddDevice *)buf;
}



int iotAddDeviceAck(IotAddDeviceAck *d, char *deviceid, unsigned int deviceNetid, unsigned int gatewayNetID)
{
	if (d == NULL)
		return 0;

	IOTSTRCPY(d->deviceid, deviceid);

	d->deviceNetid = iot_htonl(deviceNetid);
	//d->gatewayNetID = iot_htonl(gatewayNetID);
	return sizeof(IotAddDeviceAck);
}


void iotSetOtaTypeVer(char *buf, int len, unsigned int codeType, unsigned int codeVer)
{
	IotFota *f = NULL;
	int needsize = 0;
	int hs = sizeof(IotHead);
	int fs = sizeof(IotFota);
	needsize = fs + hs;

	if (len < needsize)
		return;

	f = (IotFota *)(buf + hs);
	if (f == NULL)
		return;

	f->codeType = iot_htonl(codeType);
	f->codeVer = iot_htonl(codeVer);
}

int iotSetOta(char *buf, int len, int codenumber, int codeTotal,  char *data, int datalen)
{
	IotFota *f = NULL;
	int needsize = 0;
	int fotasize = 0;
	int hs = sizeof(IotHead);
	int fs = sizeof(IotFota);
	needsize = fs + hs + datalen;

	if (len < needsize)
		return 0;

	f = (IotFota *)(buf + hs);
	if (f == NULL)
		return 0;

	f->codeNumber = iot_htonl(codenumber);
	f->codeTotal = iot_htonl(codeTotal);

	fotasize = fs + datalen;
	IOTMEMCPY(buf + hs + fs, data, datalen);
	return fotasize;
}


int iotSetOtaAck(char *buf, int len, int codenumber, int codeType, int codeVer)
{
	IotFotaAck *f = NULL;
	int needsize = 0;
	int fotasize = 0;
	int hs = sizeof(IotHead);
	int fs = sizeof(IotFotaAck);
	needsize = fs + hs;

	if (len < needsize)
		return 0;

	f = (IotFotaAck *)(buf + hs);
	if (f == NULL)
		return 0;

	f->codeNumber = iot_htonl(codenumber);
	f->codeType = iot_htonl(codeType);
	f->codeVer = iot_htonl(codeVer);
	return fs;
}


char *iotGetOtaData(char *buf, int len, int *datalen, IotFota *fota)
{
	IotFota *f = NULL;
	int needsize = 0;
	int packlen = 0;
	int hs = sizeof(IotHead);
	int fs = sizeof(IotFota);
	
	IotHead *h = iotGetHead(buf, len);
	if (h == NULL)
		return NULL;

	//wcz del packlen = iot_ntohs(h->len);
	packlen = (h->len);
	if (packlen <= 0 || packlen > 1024)
		return NULL;


	needsize = hs + packlen;
	if (len != needsize)
		return NULL;

	f = (IotFota *)(buf + hs);
	if (f == NULL)
		return NULL;
	server_log("fota==%x,f==%x",fota,f);

		*fota = *f;
	//memcpy((char *)fota,(char *)f,sizeof(IotFota));
	*datalen = packlen - fs;

	return buf + hs + fs;
}




int iotDataToBase64(const char * bindata, int binlength, char * base64, int base64len)
{
	int len = 0;
	if (base64len < 4 * binlength / 3 + 12)
		return 0;

	iotBase64encode(bindata, binlength, base64 + 1);
	base64[0] = '^';
	len = IOTSTRLEN(base64);
	base64[len] = '&';
	base64[len + 1] = 0;

	return len + 1;
}

int iotBase64ToData(const char * base64,  char * bindata, int binlength)
{
	int len = IOTSTRLEN(base64);
	if (binlength < len)
		return 0;

	if (base64[0] == '^' && base64[len - 1] == '&')
	{
		len = len - 2;
		len = iotBase64decode(base64 + 1, len, bindata);
		return len;
	}
	else
	{
		return 0;
	}
}

//辅助的JSON函数
//json处理函数, json 的key和value不能超过JSONLENGTH个字符
//value长度不能超过JSONLENGTH
void  iotTrim(char *value)
{
	char buf[JSONLENGTH];
	int len = IOTSTRLEN(value);
	int i = 0;
	int s = 0;

	for (i = 0; i < len; i++)
	{
		if (value[i] != ' ' && value[i] != '\"')
			break;
	}

	IOTSTRCPY(buf, value + i);
	len = IOTSTRLEN(buf);

	for (i = len - 1; i >= 0; i--)
	{
		if (buf[i] != ' ' && buf[i] != '\"')
			break;

		buf[i] = 0;
	}

	IOTSTRCPY(value, buf);
}



int iotGetJson(const char *json, char *key, char *value)
{
	int j = 0;
	int i = 0;
	char keysp[128];
	int len = IOTSTRLEN(json);
	int keylen = IOTSTRLEN(key);
	
	if (keylen > 32)
	{
		value[0] = 0;
		return 0;
	}

	IOTPSRINTF(keysp, "\"%s\":", key);
	keylen = IOTSTRLEN(keysp);
	
	IOTSTRCPY(value, "");

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
			IOTMEMCPY(value,  json + index, s - index);
			value[s - index] = 0;
			iotTrim(value);
			return IOTSTRLEN(value);
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










