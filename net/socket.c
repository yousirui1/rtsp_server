

#include "../rtsp/rtsp.h"
#include "../comm/type.h"


/**********************************************************
 * 函数名称: init_memory
 * 函数功能: 初始化内存
 * 输入参数 NULL
 * 输出参数 NULL
 * 返回值  
 * 			0 成功
 * 			-1 失败
 * 其他说明
 **********************************************************/
S32 init_memory()
{
	S32 i;
	
	for(i = 0; i < MAX_CONN; i++)
	{
		rtsp[i] = calloc(1, sizeof(struct rtsp_buffer));
		
		if(!rtsp[i])
		{
			return -1;
		}
		
		sem_init(&rtspd_lock[i], 0, 0);
	}

	return 0;
}



/**********************************************************
 * 函数名称: free_memory
 * 函数功能: 是否内存
 * 输入参数 NULL
 * 输出参数 NULL
 * 返回值  
 * 			0 成功
 * 			-1 失败
 * 其他说明
 **********************************************************/
S32 free_memory()
{
	S32 i;
	
	for(i = 0; i<MAX_CONN; i++)
	{
		if(rtsp[i] != NULL)
		{
			free(rtsp[i]);
			rtsp[i] = NULL;	
		}
		
		sem_destroy(&rtspd_lock[i]);
	}
	
	return 0;
}


S32 set_free_conn_status(S32 cur_conn, S32 cur_status)
{
	S32 i, j = 0;
	
	for(i = 0; i<MAX_CONN; i++)
	{
		if(i == cur_conn)
		{
			rtsp[i]->conn_status = cur_status;
		}
	}
	
	return 0;

}

VOID *vd_rtsp_procin(VOID *arg)
{
	pthread_detach(pthread_self());
	
	while(1)
	{
		printf("server waiting \n");
		tcp_accept();
	}
	pthread_exit(NULL);

}

/**********************************************************
 * 函数名称:  creat_sercmd_socket
 * 函数功能:  创建rtsp socket
 * 输入参数  
 * 				host
 * 				port   
 * 				type   SOCK_STREAM 或者 SOCK_DGRAM
 * 输出参数
 * 返回值
 * 其他说明
 **********************************************************/
S32 create_sercmd_socket(const CHAR *host, const CHAR *port, S32 type)
{
	/* create a socket for the server*/
	S32 server_len;
	S32 reuse = 1;
	struct sockaddr_in server_address;

	if(!host)
	{
		return -1;
	}
	
	rtsp[0]->fd.rtspfd = socket(AF_INET, type, 0);
	if(rtsp[0]->fd.rtspfd < 0)
	{
		printf("socket() error in create_sercmd_socet. \n");	
	}

	/* set address reuse */
	(void) setsockopt(rtsp[0]->fd.rtspfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr(host);
	server_address.sin_port = htons(atoi(port));
	server_len = sizeof(server_address);
	bind(rtsp[0]->fd.rtspfd, (struct sockaddr*)&server_address, server_len);
	listen(rtsp[0]->fd.rtspfd, 5);
	
	printf("rtsp server listen successful\n");
	
	/* rtsp cmd thread */
	if(pthread_create(&rtsp[0]->pth.rtsp_vthread1, NULL, vd_rtsp_procin, NULL) < 0)
	{
		printf("pthread_create vd_rtsp_procin thread error \n");
	}
	
	return 0 ;	
}

/**********************************************************
 * 函数名称:  rtsp_cmd_match
 * 函数功能:  
 * 输入参数  
 * 				method: rtsp command
 * 				cur_conn_num: current connection status
 * 输出参数
 * 返回值
 * 其他说明
 **********************************************************/
S32 rtsp_cmd_match(S32 method, S32 cur_conn_num)
{
	switch(method)
	{
		case 1:
		{
			if(rtsp_options(cur_conn_num) <= 0)
			{

			}
			else
			{

			}

		}
			break;
		case 2:
		{
#if 0
			if(rtsp_describe(cur_conn_num) <= 0)
			{

			}
			else
			{

			}
#endif
		}
			break;
		case 3:
		{
			if(rtsp_setup(cur_conn_num) <= 0)
			{

			}
			else
			{

			}
		}
			break;
		case 4:
		{
			if(rtsp_play(cur_conn_num) <= 0)
			{

			}
			else
			{

			}
		}
			break;
		
		case 5:
		{
			if(rtsp_terardown(cur_conn_num) <= 0)
			{
			
			}
			else
			{

			}
		}
			break;

		default:
			printf("");
			break;
		
			
	}
}



/**********************************************************
 * 函数名称:  vd_rtsp_proc
 * 函数功能:  thread for rtsp client command
 * 输入参数  
 * 				arg
 * 输出参数
 * 返回值
 * 其他说明
 **********************************************************/
void *vd_rtsp_proc(void *arg)
{
	S32 method, free_conn = 0;
	
	pthread_detach(pthread_self());
	
	free_conn = (S32) arg;
		
	while(1)
	{
		if(tcp_read(rtsp[free_conn]->cli_rtsp.cli_fd,
			rtsp[free_conn]->in_buffer, sizeof(rtsp[free_conn]->in_buffer)) < 0)
		{
			break;
		}
		method = get_rtsp_method(free_conn);
		if(rtsp_cmd_match(method, free_conn) <0)
		{
			sem_wait(&rtspd_semop);
			set_free_conn_status(free_conn, 0);
			sem_post(&rtspd_semop);
				break;		
		}
		else
		{
			sem_wait(&rtspd_semop);
			set_free_conn_status(free_conn, 1);
			sem_post(&rtspd_semop);
		}
		
		
	}

QUIT:
	pthread_exit(NULL);
}

S32 get_free_conn_status()
{
	S32 i, j = 0;
	
	for(i = 0; i < MAX_CONN; i++)
	{
		if(rtsp[i]->conn_status == 0)
			return i;
	}

	return -1;
}


S32 tcp_accept()
{
	S32 client_len, free_conn = 0;
	struct sockaddr_in client_address;
	
	free_conn = get_free_conn_status();
	if(free_conn == -1)
	{
		return -1;
	}

	rtsp[free_conn]->cli_rtsp.conn_num = free_conn;
	
	client_len = sizeof(client_address);
	
	printf("ip %s\n", inet_ntoa(client_address.sin_addr));
	printf("tcp_accept 1\n");
	rtsp[free_conn]->cli_rtsp.cli_fd = accept(rtsp[0]->fd.rtspfd, (struct sockaddr*)&client_address, &client_len);
	
	printf("tcp_accept 2\n");
	sem_wait(&rtspd_semop);
	strcpy(rtsp[free_conn]->cli_rtsp.cli_host, inet_ntoa(client_address.sin_addr));
	sem_post(&rtspd_semop);
	
	pthread_mutex_lock(&rtspd_mutex);
	pthread_cond_signal(&rtspd_cond);
	pthread_mutex_unlock(&rtspd_mutex);
	
	return 0;
	
}


/**********************************************************
 * 函数名称:  tcp_read
 * 函数功能:  创建rtsp socket
 * 输入参数  
 * 				fd
 * 				buf   
 * 				length  
 * 输出参数
 * 返回值
 * 其他说明
 **********************************************************/
S32 tcp_read(S32 fd, void  *buf, S32 length)
{
	S32 bytes_read;
	S32 bytes_left;	
	CHAR *ptr = buf;
	bytes_left = length;
		
	while(1)
	{
		bytes_read = read(fd, ptr, bytes_left);	
		if(bytes_read < 0)
		{
			if(EINTR == errno)
				continue;
			else
				return -1;
		}
		else
		 	break;
	}	
}


/**********************************************************
 * 函数名称:  tcp_write
 * 函数功能:  创建rtsp socket
 * 输入参数  
 * 				fd
 * 				buf   
 * 				length  
 * 输出参数
 * 返回值
 * 其他说明
 **********************************************************/
S32 tcp_write(S32 fd, void *buf, S32 length)
{
	S32 bytes_write;
	S32 bytes_left = length;
	CHAR *ptr = buf;
	
	while(bytes_left >0)
	{
		bytes_write = write(fd, ptr, bytes_left);
		if(bytes_write < 0)
		{
			if(EINTR == errno)
				continue;
			else 
				return -1;
		}

		bytes_left -= bytes_write;
		ptr += bytes_write;
	}

	return (length - bytes_left);
}
