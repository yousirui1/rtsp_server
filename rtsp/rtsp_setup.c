#include "rtsp.h"
#include "../comm/global.h"
//#include "../comm/md5.h"
#include "../comm/type.h"

#define RTP_DEFAULT_PORT 5005
S32 start_port = RTP_DEFAULT_PORT;


S32 get_server_port(S32 cur_conn_num)
{


}

S32 get_sessionid(S32 cur_conn_num)
{
	#ifdef WIN32
	
	#else
			
	#endif 
}


S32 send_setup_reply(S32 status, S32 cur_conn_num)
{

}

S32 rtsp_setup(S32 cur_conn_num)
{

}
