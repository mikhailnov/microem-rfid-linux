WITH_XDO ?= 0
XDO_VERSION ?= 3
CXX ?= g++
CFLAGS ?= -O2 -g
PREFIX ?= /usr
BINDIR ?= $(PREFIX)/bin

ifeq ($(WITH_XDO),1)
CFLAGS += -lxdo -DUSE_XDO$(XDO_VERSION)
endif

all:
	g++ $(CFLAGS) -I. main.cpp Transfer_unix.cpp Clscrfl.cpp -o microemscan

install:
	install -D -m0755 microemscan $(DESTDIR)/$(BINDIR)/microemscan

clean:
	rm -f microemscan
