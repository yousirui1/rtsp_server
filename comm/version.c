

#include "version.h"


S32 convert_iver_2str(CHAR *sver)
{
	if(!sver)
		return -1;
	
	sprintf(sver, "%d.%d.%d.%d",
			(RTSPD_VERSION >> 24)&0xff,
			(RTSPD_VERSION >> 16)&0xff,
			(RTSPD_VERSION >> 8)&0xff,
			(RTSPD_VERSION >> 0)&0xff);
	return 0;
}
