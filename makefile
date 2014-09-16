CC=gcc
CFLAGS=-O3 -std=c99 -Wall -Wunused-variable
CFLAGS_LINUX=-Wno-unused-result
LDFLAGS_OSX=-lpthread
LDFLAGS_BSD=-lpthread
LDFLAGS_LINUX=-lpthread -lX11
LDFLAGS_SOLARIS=-lpthread -lX11
INSTALL=/usr/bin/install -c

PREFIX=/usr/local
BIN=$(PREFIX)/bin
MAN=$(PREFIX)/share/man/man1

SOURCES=./src/*.c

all:
	@echo '========================================================='
	@echo 'You must run make with a target corresponding to your OS.'
	@echo 'Options: linux, solaris, bsd, osx, win.'
	@echo '========================================================='

linux:
	$(CC) $(CFLAGS) $(SOURCES) $(CFLAGS_LINUX) -o ./screenfetch-c $(LDFLAGS_LINUX)

solaris:
	$(CC) $(CFLAGS) $(SOURCES) -o ./screenfetch-c $(LDFLAGS_SOLARIS)

bsd:
	$(CC) $(CFLAGS) $(SOURCES) -o ./screenfetch-c $(LDFLAGS_BSD)

osx:
	$(CC) $(CFLAGS) $(SOURCES) -o ./screenfetch-c $(LDFLAGS_OSX)

win:
	$(CC) $(CFLAGS) $(SOURCES) -o ./screenfetch-c

install:
	$(INSTALL) screenfetch-c $(BIN)/screenfetch-c
	$(INSTALL) ./src/detectde $(BIN)/detectde
	$(INSTALL) ./src/detectgtk $(BIN)/detectgtk
	$(INSTALL) ./src/detectwm $(BIN)/detectwm
	$(INSTALL) ./src/detectwmtheme $(BIN)/detectwmtheme
	$(INSTALL) ./src/detectgpu $(BIN)/detectgpu
	mkdir -p $(MAN)
	$(INSTALL) ./manpage/screenfetch-c.1 $(MAN)/screenfetch-c.1

uninstall:
	rm -rf $(BIN)/screenfetch-c
	rm -rf $(BIN)/detectde
	rm -rf $(BIN)/detectgtk
	rm -rf $(BIN)/detectwm
	rm -rf $(BIN)/detectwmtheme
	rm -rf $(BIN)/detectgpu
	rm -rf $(MAN)/screenfetch-c.1

threadtest:
	$(CC) $(CFLAGS) ./src/tests/threadtest.c ./src/thread.c -o ./threadtest

clean:
	rm screenfetch-c

