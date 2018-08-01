
#include "rtsp.h"
#include "../comm/type.h"


S32 set_option_reply(S32 status, S32 cur_conn_num)
{
    if(!rtsp[cur_conn_num]->out_buffer)
        return -1; 
    sprintf(rtsp[cur_conn_num]->out_buffer, "%s %d %s"RTSP_EL"CSeq: %d"RTSP_EL, RTSP_VER, status, (CHAR *)get_stat(status), rtsp[cur_conn_num]->rtsp_cseq);
    strcat(rtsp[cur_conn_num]->out_buffer, "Public: OPTIONS,DESCRIBE,SETUP,PLAY,PAUSE,TEARDOWN"RTSP_EL);
    strcat(rtsp[cur_conn_num]->out_buffer, RTSP_EL);
    if(write(rtsp[cur_conn_num]->cli_rtsp.cli_fd, rtsp[cur_conn_num]->out_buffer, strlen(rtsp[cur_conn_num]->out_buffer))<0){
        printf("set_options_reply error\n");
        return -1; 
    }   

    return 0;
}


S32 rtsp_options(S32 cur_conn_num)
{
	if(!rtsp[cur_conn_num]->in_buffer)
	{
		return -1;
	}		
	
	if(get_rtsp_cseg(cur_conn_num) != -1)
	{
		set_option_reply(200, cur_conn_num);
		return 1;
	}
	return 0;
}
