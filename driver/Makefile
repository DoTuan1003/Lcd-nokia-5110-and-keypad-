obj-m := lcd_driver.o
lcd_driver-objs := lcd_ctrl.o lcd_5110.o

obj-m += button_ctr.o

PWD := $(shell pwd)
CROSS=/home/tuan/work/bbb/kernelbuildscripts/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-
KER_DIR=/home/tuan/work/bbb/kernelbuildscripts/KERNEL

all:
	make ARCH=arm CROSS_COMPILE=$(CROSS) -C $(KER_DIR) M=$(PWD) modules

clean:
	make -C $(KER_DIR) M=$(PWD) clean
