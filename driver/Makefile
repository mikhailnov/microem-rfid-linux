KVER ?= $(shell uname -r)
DIR ?= $(PWD)

obj-m += microem.o

all:
	make -C "/lib/modules/$(KVER)/build" M="$(DIR)" modules

clean:
	make -C /lib/modules/$(KVER)/build M="$(DIR)" clean
