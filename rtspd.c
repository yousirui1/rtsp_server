#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include "./rtsp/rtsp.h"
#include "rtspd_api.h"
#include "rtp/udp.h"
#include "comm/type.h"
#include "./net/socket.h"



pthread_cond_t rtspd_cond;
sem_t rtspd_semop;
sem_t rtspd_lock[MAX_CONN];
sem_t rtspd_accept_lock;
pthread_mutex_t rtspd_mutex;

//extern S32 create_vrtcp_socket(const CHAR *host, S32 port,S32 type,S32 cur_conn_num);

VOID sig_exit(VOID)
{
	return;
}

/*****************************************************
 *rtsp protocol processing entry function
 *
 *ser_ip: rtsp server socket listen ip
 *ser_port : rtsp server socket listen port defult 554
 *
 * **************************************************/

S32 proc_rtspd(CHAR *rtsp_ip, S32 rtsp_port)
{
	CHAR port[128] = "";
		
	if(parse_sys_conf("./rtspd.conf") < 0)
	{
		printf("please check rtspd.conf \n");
		return -1;
	}
	
	strcpy(rtsp[0]->host_name, rtsp_ip);
	rtsp[0]->rtsp_deport = rtsp_port;
	sprintf(port, "%d", rtsp[0]->rtsp_deport);
	if(create_sercmd_socket(rtsp[0]->host_name, port, SOCK_STREAM)<0)
		return -1;	
	
	return 0;
}


S32 getrtspd_version(CHAR *version)
{
	if(!version)
		return -1;

	if(convert_iver_2str(version) < 0)
		return -1;
	return 0;	
}

/**************************************************
 * 函数名称: rtspd_init
 * 函数功能: 流媒体模块初始化
 * 输入参数: NULL
 * 输出参数: NULL
 * 返回值:  = 0 成功 <0 失败
 * 其他说明: 分配内存
 * ************************************************/
S32 rtspd_init()
{
	S32 res;
	if(init_memory() < 0)
	{
		printf("init rtspd memory error\n");
		return -1;	
	}
		
	//初始化一个定位在 sem 的匿名信号量
	//	进程内线程共享  初始值
	sem_init(&rtspd_semop, 0 ,1);
	sem_init(&rtspd_accept_lock, 0 , 0);	

	res = pthread_mutex_init(&rtspd_mutex, NULL); //互斥锁初始化
	if(res != 0)
	{
		perror("Mutex initialization failed");
		return -1;
	}

	//初始化条件变量
	(void) pthread_cond_init(&rtspd_cond, NULL);
}


/*****************************************************
 * 函数名称: rtspd_free
 * 函数功能: 流媒体模块释放
 * 输入参数: NULL
 * 输出参数: NULL
 * 返回值:  = 0 成功 <0 失败
 * 其他说明: 释放内存
 * ************************************************/
S32 rtspd_free()
{
	rtsp_free();
	free_memory();
	pthread_mutex_destroy(&rtspd_mutex);
	pthread_cond_destroy(&rtspd_cond);
	sem_destroy(&rtspd_semop);
	sem_destroy(&rtspd_accept_lock);
	return 0;
}


/*****************************************************
 * 函数名称: rtspd_staus
 * 函数功能: 获取流媒体运行状态
 * 输入参数: free_chn: 空闲通道号
 * 输出参数: NULL
 * 返回值:  = 0 成功 <0 失败
 * 其他说明:
 * *************************************************/
S32 rtspd_status(S32 free_chn)
{
	return rtsp[free_chn]->rtspd_status;
}


/*****************************************************
 * 函数名称: rtspd_freechn
 * 函数功能: 获取流媒体空闲通道号
 * 输入参数: NULL
 * 输出参数: NULL
 * 返回值:  >= 0 成功 <0 失败
 * 其他说明:
 * *************************************************/
S32 rtspd_freechn()
{
	S32 i, j =0;
	//sem_wait(&rtspd_accept_lock);
	
	pthread_mutex_lock(&rtspd_mutex);
	pthread_cond_wait(&rtspd_cond, &rtspd_mutex); //无条件等待
	pthread_mutex_unlock(&rtspd_mutex);
	
	for(i = 0; i<MAX_CONN; i++)
	{
		if(rtsp[i]->conn_status == 0)
			return i;
	}	
	return -1;
}


/*****************************************************
 * 函数名称: rtsp_init
 * 函数功能: rtsp初始化
 * 输入参数: 
 * 			rtsp_ip 监听IP地址
 * 			rtsp_port 监听端口
 * 输出参数: NULL
 * 返回值:  >= 0 成功 <0 失败
 * 其他说明:
 * *************************************************/
S32 rtsp_init(CHAR *rtsp_ip, S32 rtsp_port)
{
	if(proc_rtspd(rtsp_ip, rtsp_port)< 0 )
	{
		printf("rtsp_init fail\n");
		return -1;
	}
	printf("rtsp_init successful\n");
	return 0;

}

/*****************************************************
 * 函数名称: rtsp_free
 * 函数功能: rtsp初始化
 * 输入参数: NULL
 * 输出参数: NULL
 * 返回值:  >= 0 成功 <0 失败
 * 其他说明: 建立监听rtsp server 端口
 * *************************************************/
S32 rtsp_free()
{
#if 0
	rtsp[0] ->fd.rtspfd;
#endif
	return 0;
}


/*****************************************************
 * 函数名称: set_framerate
 * 函数功能: 设置流媒体发送帧率
 * 输入参数: 
 * 			 f_rate: 帧率
 * 			 free_chn 空闲通道号
 * 输出参数: NULL
 * 返回值:  >= 0 成功 <0 失败
 * 其他说明: 
 * *************************************************/
S32 set_framerate(S32 f_rate, S32 free_chn)
{

	printf("set_framerate\n");
	switch(f_rate)
	{
		case 25:
		{
			rtsp[free_chn]->cmd_port.frame_rate_step = 3600;
		}
		break;
		
		case 30:
		{
			rtsp[free_chn]->cmd_port.frame_rate_step = 3000;
		}
		break;
		
		defult:
		{
			printf("Default frame rate 25 fpbs\n");
			rtsp[free_chn]->cmd_port.frame_rate_step = 3600;
		}
		break;

	}
}


/*****************************************************
 * 函数名称: rtsp_proc
 * 函数功能: rtsp协议命令行解析处理
 * 输入参数: 
 * 			free_chn 空闲通道号
 * 输出参数: NULL
 * 返回值:  >= 0 成功 <0 失败
 * 其他说明:
 * *************************************************/
S32 rtsp_proc(S32 free_chn)
{
	if(pthread_create(&rtsp[free_chn]->pth.rtsp_vthread, NULL, vd_rtsp_proc, (VOID *)free_chn) < 0)
	{
		printf("pthread_create vd_rtsp_proc thread error \n");
		return -1;
	}
	
	return 0;
}

/*****************************************************
 * 函数名称: rtp_init
 * 函数功能: rtp协议初始化
 * 输入参数: 
 * 			free_chn 空闲通道号
 * 输出参数: NULL
 * 返回值:  >= 0 成功 <0 失败
 * 其他说明:
 * *************************************************/
S32 rtp_init(S32 free_chn)
{
	printf("rtp_init \ n");
	S32 ret;
	sem_wait(&rtspd_lock[free_chn]);
	ret = rtspd_status(free_chn);
	
	if(ret != 8)
	{
		printf("");
		return -1;
	}

	if(create_vrtp_socket(rtsp[free_chn]->cli_rtsp.cli_host,
			rtsp[free_chn]->cmd_port.rtp_cli_port,
			SOCK_DGRAM,
			free_chn))
	{
		printf("Create vrtp socket error!\n");
		rtsp[free_chn]->rtspd_status = 0x13;
		return -1;
	}

	rtsp[free_chn]->rtspd_status = 0x14;
	
	return 0;
}


/*****************************************************
 * 函数名称: send_file
 * 函数功能: 通过文件发送数据的函数接口
 * 输入参数: 
 * 			free_chn 空闲通道号
 * 输出参数: NULL
 * 返回值:  >= 0 成功 <0 失败
 * 其他说明:
 * *************************************************/
S32 send_file(S32 free_chn)
{
	//创建一个新线程
	if(pthread_create(&rtsp[free_chn]->pth.rtp_vthread, NULL, vd_rtp_func, (VOID *)free_chn) < 0)
	{
		printf("pthread_create rtcp error:\n");
		return -1;
	}
	return 0;
}


/*****************************************************
 * 函数名称: rtp_free
 * 函数功能: rtp协议注销
 * 输入参数: 
 * 			free_chn 空闲通道号
 * 输出参数: NULL
 * 返回值:  >= 0 成功 <0 失败
 * 其他说明:
 * *************************************************/
S32 rtp_free(S32 free_chn)
{
	close(rtsp[free_chn]->fd.video_rtp_fd);
	return 0;
}

/*****************************************************
 * 函数名称: rtcp_init
 * 函数功能: rtcp协议初始化
 * 输入参数: 
 * 			free_chn 空闲通道号
 * 输出参数: NULL
 * 返回值:  >= 0 成功 <0 失败
 * 其他说明:
 * *************************************************/
S32 rtcp_init(S32 free_chn)
{
	printf("rtcp_init");
	if(create_vrtcp_socket(rtsp[free_chn]->cli_rtsp.cli_host,
			rtsp[free_chn]->cmd_port.rtcp_cli_port,
			SOCK_DGRAM,
			free_chn))
	{
		printf("Create vrtcp socket error!\n");
		rtsp[free_chn]->rtspd_status = 0x17;
		return -1;
	}
	rtsp[free_chn]->rtspd_status = 0x18;
	if(pthread_create(&rtsp[free_chn]->pth.rtcp_vthread, NULL, vd_rtcp_func, (VOID*)free_chn)< 0)
	{
		printf("pthread_create rtcp error:\n");
		return -1;
	}	
	return 0;
}

/*****************************************************
 * 函数名称: rtspd_vtype
 * 函数功能: rtsp数据流访问方式
 * 输入参数: 
 * 			free_chn 空闲通道号
 * 输出参数: NULL
 * 返回值:  
 * 			0 H264文件访问
 * 			1 PS文件访问
 * 			2 实时流方式
 *
 * 			<0 失败
 * 其他说明:
 * *************************************************/
S32 rtspd_vtype(S32 free_chn)
{
	return rtsp[free_chn]->vist_type;
}

/*****************************************************
 * 函数名称: rtcp_free
 * 函数功能: rtcp协议注销
 * 输入参数: 
 * 			free_chn 空闲通道号
 * 输出参数: NULL
 * 返回值:  
 * 			>=0 rtp注销成功
 * 			<0 失败
 * 其他说明:
 * *************************************************/
S32 rtcp_free(S32 free_chn)
{
	close(rtsp[free_chn]->fd.video_rtcp_fd);
	rtsp[free_chn]->is_runing = 0;
}


/*****************************************************
 * 函数名称: rtp_svpactet
 * 函数功能: 发送视频数据函数接口
 * 输入参数: 
 * 			buff: 		一帧视频数据buf
 * 			framesize:  一帧数据长度
 * 			free_chn:	空闲通道号
 * 输出参数: NULL
 * 返回值:  
 * 			>=0 发送数据成功
 * 			<0  发送数据失败
 * 其他说明:
 * *************************************************/
S32 rtp_svpactet(U8 *buff, S32 framesize, S32 free_chn)
{
	if(!buff)
		return -1;
	
	if(build_rtp_nalu(buff, framesize, free_chn)<0)
	{
		printf("send video packet error \n");
		return -1;
	}
	
	return 0;
}

/*****************************************************
 * 函数名称: rtsp_freeall
 * 函数功能: 释放rtp rtcp数据通道
 * 输入参数: NULL
 * 输出参数: NULL
 * 返回值:  
 * 			>=0 发送数据成功
 * 			<0  发送数据失败
 * 其他说明:
 * *************************************************/
S32 rtsp_freeall()
{
	S32 free_chn, i;
	
	for(i = 0; i<MAX_CONN; i++)
	{
		if(rtsp[free_chn]->is_runing)
		{
			rtp_free(free_chn);
			rtcp_free(free_chn);
		}
	}	
	return 0;
}
/*****************************************************
 * 函数名称: rtspd_chn_quit
 * 函数功能: rtspd 数据通道断开状态判断
 * 输入参数: 
 * 			freechn 空闲通道号
 * 输出参数: NULL
 * 返回值:  
 * 			=0  数据通道已断开
 * 			=1  数据通道正在使用
 * 其他说明:
 * *************************************************/
S32 rtspd_chn_quit(S32 free_chn)
{

	return rtsp[free_chn]->is_runing;
}


S32 main()
{
	S32 free_chn, vist_type;
	CHAR version[32];
	
	signal(SIGINT, (VOID*)sig_exit);
	
	getrtspd_version(version);
	printf("rtspd version is %s\n", version);
		
	rtspd_init();
	rtsp_init("192.168.254.222", 5554);
	free_chn = rtspd_freechn();
	set_framerate(25, free_chn);

	rtsp_proc(free_chn);

	rtp_init(free_chn);
	rtcp_init(free_chn);


	vist_type = rtspd_vtype(free_chn);
	
	if(!vist_type)
	{
		send_file(free_chn);
	}

	pause();
	printf("rtspd start release resource\n");
	rtp_free(free_chn);
	rtcp_free(free_chn);
	rtsp_free();
	rtspd_free();

	return 0;
}
