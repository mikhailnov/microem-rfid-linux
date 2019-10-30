KVER ?= $(shell uname -r)
DIR ?= $(PWD)
MODDESTDIR ?= /lib/modules/$(KVER)/kernel/drivers/usb/misc/

obj-m += microem.o

all:
	make -C "/lib/modules/$(KVER)/build" M="$(DIR)" modules CONFIG_DEBUG_INFO=y

install:
	make -C "/lib/modules/$(KVER)/build" M="$(DIR)" modules_install INSTALL_MOD_PATH="$(MODDESTDIR)"
	install -m0644 microem.ko "$(MODDESTDIR)/microem.ko"

clean:
	make -C "/lib/modules/$(KVER)/build" M="$(DIR)" clean
	rm -fv "$(DIR)/microem.ko"

modprobe:
	depmod -a
	modprobe microem
