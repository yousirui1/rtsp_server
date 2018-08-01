

#include "../rtsp/rtsp.h"


/*******************************************************
 *remove enter a new linue character
 *
 *
 *
 * *****************************************************/

VOID rtrun_cate(CHAR *pString, CHAR *szFill)
{
	S32 i, len;
	len = strlen(pString);
	
	for(i = len ; i > 0; i--)
	{
		//搜索字符在字符串第一次出现的位置
		if(!strchr(szFill, pString[i-1]))
			break;
	}

	pString[i] = 0;

}

S32 get_um_state(CHAR *buffer)
{
	CHAR *temp_buff;
	
	if(!buffer)
		return -1;
	
	if(strstr(buffer, "rtspd_is_unicast"))
	{
		temp_buff = strstr(buffer, "=") +1;
		rtsp[0]->rtsp_um_stat = atoi(temp_buff);
	}
		
	return 0;
}

S32 get_ser_ip(CHAR *buffer)
{
	CHAR *temp_buff;
	
	if(!buffer)
		return -1;
	
	if(strstr(buffer, "rtspd_serip"))
	{
		temp_buff = strstr(buffer, "=") +1;
		strncpy(rtsp[0]->host_name, temp_buff, strlen(temp_buff));
	}
	
	return 0;
}

S32 get_ser_port(CHAR *buffer)
{
	CHAR *temp_buff;
	
	if(!buffer)
		return -1;
	if(strstr(buffer, "rtspd_port"))
	{
		temp_buff = strstr(buffer, "=") + 1;
		rtsp[0]->rtsp_deport = atoi(temp_buff);
	}
	return 0;
}

S32 get_vist_file(CHAR *buffer)
{
	CHAR *temp_buff;
		
	if(!buffer)
		return -1;

	if(strstr(buffer, "rtspd_vist_file"))
	{
		temp_buff = strstr(buffer, "=") + 1;
		strncpy(rtsp[0]->file_name, temp_buff, strlen(temp_buff));
	}
	return 0;
}

S32 get_sys_value(CHAR *buffer)
{
	if(!buffer)
		return -1;
	
	if(get_um_state(buffer) < 0)
		return -1;
	
	if(get_ser_ip(buffer) < 0)
		return -1;
	
	if(get_ser_port(buffer) < 0)
		return -1;
	
	if(get_vist_file(buffer) < 0)
		return -1;

	return 0;
}

S32 parse_sys_conf(CHAR *szFileName)
{
	FILE *fp;
	S32 i = 0;
	CHAR szBuf[128];

	if(!szFileName)
		return -1;
	
	if((fp = fopen(szFileName, "rb")) == NULL)
	{
		printf("rtspd>>cmd config File Not Find! \n");
		return -1;
	}
	while(!feof(fp))
	{
		memset(szBuf, 0, 128);
		fgets(szBuf, 128 , fp);
		rtrun_cate(szBuf, "\n");
		rtrun_cate(szBuf, "\r");
		if(get_sys_value(szBuf) < 0)
		{
			printf("rtspd>>get_sys_value error\n");
		}
		i++;

	}
}

