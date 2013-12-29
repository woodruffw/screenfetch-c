CC=gcc
CFLAGS=-O3 -lpthread -std=c99
INSTALL=/usr/bin/install -c

BINDIR=/usr/local/bin
MANDIR=/usr/local/share/man/man1


all:
	$(CC) $(CFLAGS) ./src/screenfetch-c.c ./src/thread.c -o ./screenfetch

install:
	$(INSTALL) screenfetch $(BINDIR)/screenfetch
	$(INSTALL) ./src/detectde $(BINDIR)/detectde
	$(INSTALL) ./src/detectgtk $(BINDIR)/detectgtk
	$(INSTALL) ./src/detectwm $(BINDIR)/detectwm
	$(INSTALL) ./src/detectwmtheme $(BINDIR)/detectwmtheme
	mkdir -p $(MANDIR)
	$(INSTALL) ./manpage/screenfetch.1 $(MANDIR)/screenfetch.1

uninstall:
	rm -rf $(BINDIR)/screenfetch
	rm -rf $(BINDIR)/detectde
	rm -rf $(BINDIR)/detectgtk
	rm -rf $(BINDIR)/detectwm
	rm -rf $(BINDIR)/detectwmtheme
	rm -rf $(MANDIR)/screenfetch.1

