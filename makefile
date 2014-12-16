CC = gcc
CFLAGS = -O3 -std=c99 -Wall -Wformat -Wunused-variable -pedantic
CPPFLAGS = -D_POSIX_C_SOURCE=200112L
LDFLAGS =
INSTALL = install -c

PREFIX = /usr/local
BIN = $(PREFIX)/bin
MAN = $(PREFIX)/share/man/man1

SOURCES = $(wildcard ./src/*.c)
OBJS = $(SOURCES:.c=.o)

SCRIPTS =
TESTS =

OLDTARGETS = linux win bsd osx sun

ifeq ($(OS),Windows_NT)
	SOURCES += $(wildcard ./src/plat/win32/*.c)
	CPPFLAGS += -DWIN32_LEAN_AND_MEAN
else
	UNAME_S := $(shell uname -s)

	ifeq ($(UNAME_S),Linux)
		SOURCES += $(wildcard ./src/plat/linux/*.c)
		CFLAGS += -Wno-unused-result
		LDFLAGS += -lX11 -lGL
		SCRIPTS += ./src/scripts/detectwmtheme ./src/scripts/detectgtk
		TESTS += x11test gltest
	endif

	ifeq ($(UNAME_S),Darwin)
		SOURCES += $(wildcard ./src/plat/darwin/*.c)
		LDFLAGS += -framework CoreServices
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
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OBJS) -o ./screenfetch-c $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

install: all
	$(INSTALL) ./screenfetch-c $(BIN)/screenfetch-c
	if [ -n "$(SCRIPTS)" ] ; then \
		$(INSTALL) $(SCRIPTS) $(BIN) ; \
	fi
	mkdir -p $(MAN)
	$(INSTALL) ./man/screenfetch-c.1 $(MAN)/screenfetch-c.1

uninstall:
	rm -rf $(BIN)/screenfetch-c
	rm -rf $(BIN)/detectde
	rm -rf $(BIN)/detectgtk
	rm -rf $(BIN)/detectwm
	rm -rf $(BIN)/detectwmtheme
	rm -rf $(BIN)/detectgpu
	rm -rf $(MAN)/screenfetch-c.1

x11test:
	@echo "Testing for X11..."
	$(CC) $(CFLAGS) ./tests/x11test.c -o ./x11test -lX11
	@echo "Looks good."

gltest:
	@echo "Testing for OpenGL..."
	$(CC) $(CFLAGS) ./tests/gltest.c -o ./gltest -lGL
	@echo "Looks good."

clean:
	rm -f ./src/*.o ./src/plat/*/*.o
	rm -f threadtest
	rm -f x11test
	rm -f gltest
	rm -f screenfetch-c

$(OLDTARGETS): all

.PHONY: all install uninstall clean $(OLDTARGETS)

