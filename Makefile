CC = gcc
CFLAGS = -O3 -std=c99 -Wall -Wformat -Wunused-variable -pedantic
CPPFLAGS = -D_POSIX_C_SOURCE=200809L
LDFLAGS =
INSTALL = install -c

PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/share/man/man1

ALL_SOURCES = $(shell find . -type f \( -name '*.c' -o -name '*.h' \))

SOURCES = $(wildcard ./src/*.c)
OBJS = $(SOURCES:.c=.o)

EXEEXT =
PROG = screenfetch-c$(EXEEXT)


SCRIPTS =
TESTS =

OLDTARGETS = linux win bsd osx sun

ifeq ($(COLORS),0)
	CPPFLAGS += -DNO_COLORS
endif
ifneq (,$(findstring mingw,$(shell $(CC) -dumpmachine)))
OS = Windows_NT
endif
ifeq ($(OS),Windows_NT)
	SOURCES += $(wildcard ./src/plat/win32/*.c)
	CPPFLAGS += -DWIN32_LEAN_AND_MEAN
	LDFLAGS += -lgdi32
	EXEEXT = .exe
else
	UNAME_S := $(shell uname -s)

	ifeq ($(UNAME_S),Linux)
		SOURCES += $(wildcard ./src/plat/linux/*.c)
		CFLAGS += -Wno-unused-result
		LDFLAGS += -lX11 -lGL
		SCRIPTS += ./src/scripts/detectgtk
		TESTS += x11test gltest
	endif

	ifeq ($(UNAME_S),Darwin)
		SOURCES += $(wildcard ./src/plat/darwin/*.c)
		LDFLAGS += -framework CoreFoundation -framework IOKit -framework CoreGraphics
		CPPFLAGS += -D_DARWIN_C_SOURCE -D_DARWIN_USE_64_BIT_INODE
	endif

	ifeq ($(UNAME_S),SunOS)
		SOURCES += $(wildcard ./src/plat/sun/*.c)
		LDFLAGS += -lX11
		SCRIPTS += ./src/scripts/detectwm ./src/scripts/detectwmtheme
		TESTS += x11test
	endif

	ifneq (,$(filter $(UNAME_S),FreeBSD NetBSD OpenBSD DragonFly))
		SOURCES += $(wildcard ./src/plat/bsd/*.c)
		LDFLAGS +=
		SCRIPTS += ./src/scripts/detectwm ./src/scripts/detectwmtheme \
		./src/scripts/detectgtk
	endif
endif

all: $(TESTS) $(OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OBJS) -o $(PROG) $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

install: all
	$(INSTALL) $(PROG) $(BINDIR)/$(PROG)
	if [ -n "$(SCRIPTS)" ] ; then \
		$(INSTALL) $(SCRIPTS) $(BINDIR) ; \
	fi
	mkdir -p $(MANDIR)
	$(INSTALL) ./man/man1/screenfetch-c.1 $(MANDIR)/screenfetch-c.1

uninstall:
	rm -rf $(BINDIR)/screenfetch-c
	rm -rf $(BINDIR)/detectde
	rm -rf $(BINDIR)/detectgtk
	rm -rf $(BINDIR)/detectwm
	rm -rf $(BINDIR)/detectwmtheme
	rm -rf $(BINDIR)/detectgpu
	rm -rf $(MANDIR)/screenfetch-c.1

x11test:
	@echo "Testing for X11..."
	$(CC) $(CFLAGS) ./src/tests/x11test.c -o ./x11test -lX11
	@echo "Looks good."

gltest:
	@echo "Testing for OpenGL..."
	$(CC) $(CFLAGS) ./src/tests/gltest.c -o ./gltest -lGL
	@echo "Looks good."

clean:
	rm -f ./src/*.o ./src/plat/*/*.o
	rm -f threadtest
	rm -f x11test
	rm -f gltest
	rm -f screenfetch-c screenfetch-c.exe

fmt:
	clang-format -i -style=file $(ALL_SOURCES)

$(OLDTARGETS): all

.PHONY: all install uninstall clean fmt $(OLDTARGETS)

