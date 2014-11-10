CC = gcc
CFLAGS = -O3 -std=c99 -Wall -Wformat -Wunused-variable -pedantic -D_POSIX_C_SOURCE=200112L
LDFLAGS =
INSTALL = /usr/bin/install -c

PREFIX = /usr/local
BIN = $(PREFIX)/bin
MAN = $(PREFIX)/share/man/man1

SOURCES = $(wildcard ./src/*.c)
OBJS = $(SOURCES:.c=.o)

TESTS =

ifeq ($(OS),Windows_NT)
	SOURCES += $(wildcard ./src/plat/win32/*.c)
	CFLAGS += -DWIN32_LEAN_AND_MEAN
else
	UNAME_S := $(shell uname -s)

	ifeq ($(UNAME_S),Linux)
		SOURCES += $(wildcard ./src/plat/linux/*.c)
		CFLAGS += -Wno-unused-result
		LDFLAGS += -lpthread -lX11 -lGL
		TESTS += x11test gltest
	endif

	ifeq ($(UNAME_S),Darwin)
		SOURCES += $(wildcard ./src/plat/darwin/*.c)
		LDFLAGS += -lpthread -framework CoreServices
	endif

	ifeq ($(UNAME_S),SunOS)
		SOURCES += $(wildcard ./src/plat/sun/*.c)
		LDFLAGS += -lpthread -lX11
		TESTS += x11test
	endif

	ifneq (,$(filter $(UNAME_S),FreeBSD NetBSD OpenBSD DragonFly))
		SOURCES += $(wildcard ./src/plat/bsd/*.c)
		LDFLAGS += -lpthread
	endif
endif

all: $(TESTS) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o ./screenfetch-c $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

install:
	$(INSTALL) screenfetch-c $(BIN)/screenfetch-c
	$(INSTALL) ./src/scripts/detectde $(BIN)/detectde
	$(INSTALL) ./src/scripts/detectgtk $(BIN)/detectgtk
	$(INSTALL) ./src/scripts/detectwm $(BIN)/detectwm
	$(INSTALL) ./src/scripts/detectwmtheme $(BIN)/detectwmtheme
	$(INSTALL) ./src/scripts/detectgpu $(BIN)/detectgpu
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
	$(CC) $(CFLAGS) ./src/tests/x11test.c -o ./x11test -lX11
	rm -f ./x11test

gltest:
	@echo "Testing for OpenGL..."
	$(CC) $(CFLAGS) ./src/tests/gltest.c -o ./gltest -lGL
	rm -f ./gltest

clean:
	rm -f ./src/*.o ./src/plat/*/*.o
	rm -f threadtest
	rm -f x11test
	rm -f gltest
	rm -f screenfetch-c

