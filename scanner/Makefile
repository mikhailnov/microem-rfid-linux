CXX ?= g++
CFLAGS ?= -O2 -g
PREFIX ?= /usr
BINDIR ?= $(PREFIX)/bin

all:
	g++ $(CFLAGS) -I. -lxdo main.cpp Transfer_unix.cpp Clscrfl.cpp -o microemscan

install:
	install -m0755 microemscan $(DESTDIR)/$(BINDIR)/microemscan

clean:
	rm -f microemscan
