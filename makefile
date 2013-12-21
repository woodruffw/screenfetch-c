CC=gcc
CFLAGS=-03
INSTALL=/usr/bin/install -c

bindir=/usr/local/bin
mandir=/usr/local/share/man/man1

all:
	$(CC) ./src/screenfetch-c.c ./src/thread.c -o ./screenfetch -std=c99 -lpthread

install:
	$(INSTALL) screenfetch $(bindir)/screenfetch
	$(INSTALL) ./src/detectde $(bindir)/detectde
	$(INSTALL) ./src/detectgtk $(bindir)/detectgtk
	$(INSTALL) ./src/detectwm $(bindir)/detectwm
	$(INSTALL) ./src/detectwmtheme $(bindir)/detectwmtheme
	mkdir -p $(mandir)
	$(INSTALL) ./manpage/screenfetch.1 $(mandir)/screenfetch.1

uninstall:
	rm -rf $(bindir)/screenfetch
	rm -rf $(bindir)/detectde
	rm -rf $(bindir)/detectgtk
	rm -rf $(bindir)/detectwm
	rm -rf $(bindir)/detectwmtheme
	rm -rf $(mandir)/screenfetch.1

