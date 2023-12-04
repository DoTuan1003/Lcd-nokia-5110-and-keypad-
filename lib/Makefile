.PHONY : clean

SOURCES = $(shell echo *.c)
HEADERS = $(shell echo *.h)
OBJECTS = $(SOURCES:.c=.o)
TARGET = liblcd_lib.a

AR = ar
CC=/home/tuan/work/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(AR) rcs $(TARGET) $(OBJECTS)

$(OBJECTS): $(SOURCES) $(HEADERS)
	$(CC) -c $(SOURCES) -o $(OBJECTS)

clean: 
	rm -f $(OBJECTS) $(TARGET)
