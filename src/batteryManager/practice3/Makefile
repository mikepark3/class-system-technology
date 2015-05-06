obj-m := batteryModule.o

KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

.PHONY: all
all: dev \
	app

.PHONY: dev 
dev:
	make -C $(KERNELDIR) M=$(PWD) modules

.PHONY: app 
app:
	gcc -o batteryStatus batteryStatus.c

.PHONY: clean 
clean:
	make -C $(KERNELDIR) M=$(PWD) clean
	rm -rf batteryStatus
