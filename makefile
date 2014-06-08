CC=gcc
CFLAGS=-O3 -std=c99 -Wall
LDFLAGS_OSX=-lpthread
LDFLAGS_BSD=-lpthread
LDFLAGS_LINUX=-lpthread -lX11
LDFLAGS_SOLARIS=-lpthread -lX11
INSTALL=/usr/bin/install -c

BINDIR=/usr/local/bin
MANDIR=/usr/local/share/man/man1


all:
	@echo '========================================================='
	@echo 'You must run make with a target corresponding to your OS.'
	@echo 'Options: linux, solaris, bsd, osx, win.'
	@echo '========================================================='

linux:
	$(CC) $(CFLAGS) ./src/screenfetch-c.c ./src/thread.c -o ./screenfetch-c $(LDFLAGS_LINUX)

solaris:
	$(CC) $(CFLAGS) ./src/screenfetch-c.c ./src/thread.c -o ./screenfetch-c $(LDFLAGS_SOLARIS)

bsd:
	$(CC) $(CFLAGS) ./src/screenfetch-c.c ./src/thread.c -o ./screenfetch-c $(LDFLAGS_BSD)

osx:
	$(CC) $(CFLAGS) ./src/screenfetch-c.c ./src/thread.c -o ./screenfetch-c $(LDFLAGS_OSX)

win:
	$(CC) $(CFLAGS) ./src/screenfetch-c.c ./src/thread.c -o ./screenfetch-c

install:
	$(INSTALL) screenfetch-c $(BINDIR)/screenfetch-c
	$(INSTALL) ./src/detectde $(BINDIR)/detectde
	$(INSTALL) ./src/detectgtk $(BINDIR)/detectgtk
	$(INSTALL) ./src/detectwm $(BINDIR)/detectwm
	$(INSTALL) ./src/detectwmtheme $(BINDIR)/detectwmtheme
	$(INSTALL) ./src/detectgpu $(BINDIR)/detectgpu
	mkdir -p $(MANDIR)
	$(INSTALL) ./manpage/screenfetch-c.1 $(MANDIR)/screenfetch-c.1

uninstall:
	rm -rf $(BINDIR)/screenfetch-c
	rm -rf $(BINDIR)/detectde
	rm -rf $(BINDIR)/detectgtk
	rm -rf $(BINDIR)/detectwm
	rm -rf $(BINDIR)/detectwmtheme
	rm -rf $(BINDIR)/detectgpu
	rm -rf $(MANDIR)/screenfetch-c.1

threadtest:
	$(CC) $(CFLAGS) ./src/threadtest.c ./src/thread.c -o ./threadtest

clean:
	rm screenfetch-c

