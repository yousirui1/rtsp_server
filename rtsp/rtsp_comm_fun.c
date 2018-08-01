#include "rtsp.h"
#include "../comm/type.h"

struct rtsp_buffer *rtsp[MAX_CONN];


S32 get_rtsp_cseg(S32 cur_conn_num)
{
	CHAR *p;
	CHAR trash[255] = {0};
	
	/* check CSeq */
	if((p = strstr(rtsp[cur_conn_num]->in_buffer, "CSeq")) == NULL)
	{
	 	send_reply(400, cur_conn_num);
	}
	else
	{
		if(sscanf(p, "%254s %d", trash, &(rtsp[cur_conn_num]->rtsp_cseq)) != 2)
		{
			//not find CSeq value send 400 error
			send_reply(400, cur_conn_num);
			return -1;	
		}
	}
	
	return 0;

}


S32 check_rtsp_url(S32 cur_conn_num)
{
	U16 port;
	CHAR url[128] = {0};
	CHAR object[128], server[128];
	
	printf("check_rtsp_url %s", url);
	
	if(!sscanf(rtsp[cur_conn_num]->in_buffer, " %*s %254s ", url))
	{
		send_reply(400, cur_conn_num);
		return -1;
	}

	//Validate the URL
	if(!parse_url(url, server, &port, object))
	{
		send_reply(400, cur_conn_num);
		return -1;
	}
	strcpy(rtsp[0]->host_name, server);
	
	//get media file name
	if(strstr(object, "trackID"))
	{
		strcpy(object, rtsp[0]->file_name);
	}
	else
	{
		if(strcmp(object, "") == 0)
		{
			strcpy(object, rtsp[0]->file_name);
			
		}
		strcpy(rtsp[0]->file_name, object);
	}	

	return 0;

}


S32 check_rtsp_filename(S32 cur_conn_num)
{
	CHAR *p;
	S32 valid_url;
	
	printf("file_name %s \n", rtsp[0]->file_name);
	if(strstr(rtsp[0]->file_name, "../"))
	{
		//disallow relative paths outside of current directory
		printf("check file erro 1\n");
		send_reply(403, cur_conn_num);
		return -1;
	}

	if(strstr(rtsp[0]->file_name, "./"))
	{
		//Disallow
		printf("check file erro 2\n");
		send_reply(403, cur_conn_num);
		return -1;
	}
	
	p = strrchr(rtsp[0]->file_name, '.');
	valid_url = 0;
	if(p == NULL)
	{
		//Unsupported media type
		send_reply(415, cur_conn_num);
		printf("check file erro 3\n");
		return -1;

	}
	else
	{
		valid_url = is_supported_mediatype(p, cur_conn_num);
	}
	if(!valid_url)
	{
		send_reply(415, cur_conn_num);
		printf("check file erro 4\n");
		return -1;
	}
	return 0;	
}
