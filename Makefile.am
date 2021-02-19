AUTOMAKE_OPTIONS=foreign
bin_PROGRAMS=rtspd
rtspd_SOURCES=comm/version.c comm/parese_conf.c comm/md5c.c rtsp/rtsp_comm_fun.c rtsp/rtsp_options.c rtsp/rtsp_describe.c \
							rtsp/rtsp_setup.c rtsp/rtsp_play.c rtsp/rtsp_teardown.c rtsp/rtsp_status.c \
						  rtp/rtp_h264.c rtcp/rtcp.c  rtp/udp.c rtp/tcp.c net/socket.c rtspd.c
rtspd_CFLAGS = $(AM_CFLAGS)

lib_LTLIBRARIES=librtspd.la
librtspd_la_SOURCES=comm/version.c comm/parese_conf.c comm/md5c.c rtsp/rtsp_comm_fun.c rtsp/rtsp_options.c rtsp/rtsp_describe.c \
							rtsp/rtsp_setup.c rtsp/rtsp_play.c rtsp/rtsp_teardown.c rtsp/rtsp_status.c \
						  rtp/rtp_h264.c rtcp/rtcp.c  rtp/udp.c rtp/tcp.c net/socket.c rtspd.c
