#include "../rtsp/rtsp.h"
#include "../rtcp/rtcp.h"
#include "../comm/type.h"

/**********************************************************
 *
 *
 *
 *
 *
 * *********************************************************/

S32 create_vrtp_socket(const CHAR* host, S32 port, S32 type, S32 cur_conn_num)
{
	/* Create a socket for the client */
	S32 len, reuse = 1;
	struct sockaddr_in rtp_address, rtp_bind;
	S32 result;

	rtsp[cur_conn_num]->fd.video_rtp_fd = socket(AF_INET, type, 0);
	
	/* set address reuse */
	(VOID) setsockopt(rtsp[cur_conn_num]->fd.video_rtp_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	/*bind local port */
	rtp_bind.sin_family = AF_INET;
	rtp_bind.sin_addr.s_addr = htonl(INADDR_ANY);
	rtp_bind.sin_port = htons(rtsp[cur_conn_num]->cmd_port.rtp_ser_port);
	if((bind(rtsp[cur_conn_num]->fd.video_rtp_fd, (struct sockaddr *) &rtp_bind, sizeof(rtp_bind))) < 0)
	{
		printf("bind rtsp server port error ");
		return -1;
	}
	
	/* Name the socket, as agreed with the server */
	rtp_address.sin_family = AF_INET;
	rtp_address.sin_addr.s_addr = inet_addr(host);
	rtp_address.sin_port = htons(port);
	len  = sizeof(rtp_address);
	
	/* Now connect our socket to the server's socket */
	result = connect(rtsp[cur_conn_num]->fd.video_rtp_fd, (struct sockaddr *)&rtp_address, len);
	
	if(result == -1)
	{
		printf("connect vrtp socket error \n ");
		return -1;
	}
	
	printf("bind conn video rtp sucessful \n");
	return 0;
}

S32 create_vrtcp_socket(const CHAR *host, S32 port, S32 type, S32 cur_conn_num)
{
	S32 len, reuse = 1;
	struct sockaddr_in rtcp_address, rtcp_bind;
	S32 result;

	rtsp[cur_conn_num]->fd.video_rtcp_fd = socket(AF_INET, type, 0);
	
	/* set address reuse */
	(VOID) setsockopt(rtsp[cur_conn_num]->fd.video_rtcp_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
	
	/* bind local port */
	rtcp_bind.sin_family = AF_INET;
	rtcp_bind.sin_addr.s_addr = htonl(INADDR_ANY);
	rtcp_bind.sin_port = htons(rtsp[cur_conn_num]->cmd_port.rtcp_ser_port);
	
	if((bind(rtsp[cur_conn_num]->fd.video_rtcp_fd, (struct sockaddr*)&rtcp_bind, sizeof(rtcp_bind))) == -1)
	{
		printf("bind rtcp server port error");
		return -1;
	}

	/* Name the socket, as agreed with the server */
	rtcp_address.sin_family = AF_INET;
	rtcp_address.sin_addr.s_addr = inet_addr(host);
	rtcp_address.sin_port = htons(port);
	len = sizeof(rtcp_address);
	
	/* Now connect out socket to the server's socket */
	result = connect(rtsp[cur_conn_num]->fd.video_rtcp_fd, (struct sockaddr *)&rtcp_address, len);
	
	if(result == -1)
	{
		printf("connect vrtcp socket error \n");
		return -1;
	}

	printf("bind conn video rtcp sucessful\n");
	return 0;

}

VOID *vd_rtp_func(VOID *arg)
{
	S32 conn_cur;
	struct timeval tv;
	
	pthread_detach(pthread_self());
	conn_cur = (S32)arg;
	
	rtp_send_packet(conn_cur);
	
	pthread_exit(NULL);
}

VOID *vd_rtcp_func(VOID *arg)
{
	S32 len, cur_conn;
	struct timeval tv;
	struct rtcp_pkt pkt;
	
	//线程结束后，自动释放
	pthread_detach(pthread_self());

	cur_conn = (S32)arg;
	len = rtcp_send_packet(&pkt);
	
	while(rtsp[cur_conn]->is_runing)
	{
		if(write(rtsp[cur_conn]->fd.video_rtcp_fd, &pkt, len) <0)
		{
			printf("send rtcp packet error \n");
			rtsp[cur_conn]->rtspd_status = 0x22;
		}
		sleep(3);
	}

	close(rtsp[cur_conn]->fd.video_rtcp_fd);
	pthread_exit(NULL);
}