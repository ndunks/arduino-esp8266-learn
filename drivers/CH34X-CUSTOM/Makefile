ifeq ($(KERNELRELEASE), )
MODULEDIR := /lib/modules/$(shell uname -r)/extra/
KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD :=$(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR)  M=$(PWD)

clean:
	rm -rf .tmp_versions Module.symvers *.mod.c *.o *.ko .*.cmd Module.markers modules.order

install:
	./install.sh $(MODULEDIR)

uninstall:
	rm -f $(MODULEDIR)/ch34x-custom.ko /etc/udev/rules.d/99-ch34x-custom.rules /etc/modprobe.d/blacklist-ch341.conf

load:
	modprobe usbserial
	insmod $(MODULEDIR)/ch34x-custom.ko

unload:
	rmmod ch34x-custom

else
	obj-m := ch34x-custom.o
endif
