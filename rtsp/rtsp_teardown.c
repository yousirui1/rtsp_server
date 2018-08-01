
#include "rtsp.h"


S32 send_terardown_reply(S32 status, S32 cur_conn_num)
{


}

S32 rtsp_terardown(S32 cur_conn_num)
{
	CHAR *p = NULL;
	CHAR trash[255];
		
	if(!rtsp[cur_conn_num]->in_buffer)
		return -1;
		
	//parse the input message
	
	if(check_rtsp_url(cur_conn_num) < 0)
		return -1;

	if(get_rtsp_cseg(cur_conn_num) < 0)
		return -1;
		
	if(check_rtsp_filename(cur_conn_num) < 0)
		return -1;
		
	//if we get a session hdr, then we have an aggregate control
	if((p = strstr(rtsp[cur_conn_num]->in_buffer, HDR_SESSION)) != NULL)
	{
		if(sscanf(p, "%254s %d", trash, &rtsp[cur_conn_num]->session_id) != 2)
		{
			//Session Not Found
			send_reply(454, cur_conn_num);
			return -1;
		}
	}	
	else
	{
		// bad request
		send_reply(400, cur_conn_num);
		return -1;
	}
	
	if(send_terardown_reply(200, cur_conn_num) != 1)
		return 1;
	
	return 0;
}
