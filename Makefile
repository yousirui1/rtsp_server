#CROSS_COMPILE = 

#CROSS_COMPILE = arm-goke-linux-uclibcgnueabi-
CROSS_COMPILE = 

#CFLAGES = -I./include/ -I../base/include/ -lpthread
CFLAGES =   -g  -lpthread 


.PHONY : clean  # avoid actually named clean

#obj=param.cgi
#objects= main.o inirw.o cgic.o 


OBJS = rtspd.o ./net/socket.o ./rtcp/rtcp.o ./rtp/rtp_h264.o ./rtp/udp.o \
					./rtsp/rtsp_comm_fun.o ./rtsp/rtsp_options.o ./rtsp/rtsp_play.o \
					./rtsp/rtsp_setup.o ./rtsp/rtsp_status.o ./rtsp/rtsp_teardown.o \
					./rtsp/rtsp_describe.o ./rtcp/random32.o ./comm/version.o ./comm/parese_conf.o\
					./comm/md5c.o

TARGET = edio

AS = $(CROSS_COMPILE)as
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar
STRIP = $(CROSS_COMPILE)strip

dir_nfs=/usr/gk

VERSION=1.0
OBJCOPY = $(CROSS_COMPILE)objcopy
CONFIG_COMPILER=gnu



$(TARGET):$(OBJS)                     
	$(CC) -o  $@  $^ $(CFLAGES)       

$(OBJS):%.o:%.c         
	$(CC) -c $<  $(CFLAGES) -o $@      

clean:                    
	-$(RM) $(TARGET) $(OBJS)        

