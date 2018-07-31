#include "../comm/type.h"
#include "rtsp.h"


S32 get_rtsp_method(S32 cur_conn_num)
{
	CHAR method[32];

	if(!rtsp[cur_conn_num]->in_buffer)
		return -1;

	sscanf(rtsp[cur_conn_num]->in_buffer, "%3ls", method);
	
	if(strcmp(method, "OPTIONS") == 0)
	{
		return 1;
	}

	if(strcmp(method, "DESCRIBE") == 0)
	{

		return 2;
	}

	if(strcmp(method, "SETUP") == 0)
	{

		return 3;
	}
	
	if(strcmp(method, "PLAY") == 0)
	{
		return 4;
	}

	if(strcmp(method, "TEARDOWN") == 0)
	{
		return 5;
	}
	
	return 0;
	
}
