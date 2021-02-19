CC = gcc
INCLUDE =	-L./ -lrtspd -lpthread
FLAGS = -Wall -g -O3 -o 
TARGET = sample_rtspd
OBJECT = sample_rtspd.c
#LIBS = 

$(TARGET): $(OBJECT)
	cp ../.libs/librtspd.so.0 /usr/lib/
	$(CC) $(INCLUDE) $(FLAGS) $(TARGET) sample_rtspd.c 
	
install:
	@mkdir -p ./bin
	@rm -f ./bin/*
	@cp $(TARGET) ./bin

.PHONY clean:
clean:
	rm -f *.o $(TARGET) 
