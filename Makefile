MODULE_NAME = hello_driver
KDIR = /lib/modules/5.10.92-v7l+/build
obj-m := $(MODULE_NAME).o
default:
	$(MAKE) -C $(KDIR) M=$$PWD modules
install:
	cp -f $(MODULE_NAME).ko /root/raspberry/nfs
clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean