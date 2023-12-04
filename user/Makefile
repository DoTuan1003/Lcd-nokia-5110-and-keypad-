
INCLUDE_DIR=../lib
CFLAGS=-I$(INCLUDE_DIR) -Wall -lm -pthread
LDFLAGS=-L$(INCLUDE_DIR) -llcd_lib

SOURCES = $(shell echo *.c)

TARGET=mysnake
CC=/home/tuan/work/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc

all:
	$(CC) $(SOURCES) -o $(TARGET) $(CFLAGS) $(LDFLAGS)

clean:
	rm $(TARGET) 

