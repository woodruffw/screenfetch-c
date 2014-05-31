screenfetch-c
=============

## Current Version: 1.2 release (compiled and 99.99% complete)

screenfetch-c is an attempt to rewrite screenFetch, a popular shell 
script that displays system information and an ASCII logo, in the C 
language.
Like the original screenFetch, screenfetch-c is meant to be compatible with a number of systems, including *BSD, GNU/Linux, OS X, and Windows (via the Cygwin environment).

## Current Status:
screenfetch-c has been extensively tested on Windows 7 (in the Cygwin environment), OS X (10.6 and 10.8), and Linux (see list below).

There are only a few select sections that still require work. They are listed here:
- GPU detection is currently experimental on Linux and BSD.
- Distro detection is not 100% complete, although it's close. If screenfetch-c fails to detect a specific distro, try running it with the -D [distro] flag.
- Package detection requires more in-depth testing on OS X (testing for MacPorts/Fink).

## Installing screenfetch-c:

### Dependencies:
screenfetch-c only relies on libraries that come on virtually every system.

* On Linux and Solaris, libX11 is required. In most package managers, this is listed as libX11-dev.

* On Windows, the WinAPI is required (duh).

Installing screenfetch-c is very simple.
First, clone the repository:

```shell
$ git clone https://www.github.com/woodruffw/screenfetch-c.git
$ cd screenfetch-c
```
Then, follow the instructions for your OS:

### Linux

```shell
$ make linux && sudo make install
```

### Solaris (incl. derivatives)

```shell
$ make solaris && sudo make install
```

_Note:_ For Solaris, `make` *must* be GNU make. If it isn't, use `gmake`.

### BSD

```shell
$ make bsd && sudo make install
```

### OS X

screenfetch-c is now available via Homebrew!

To install it, just run the following:

```shell
$ brew tap woodruffw/screenfetch-c
$ brew install screenfetch-c
```

Alternatively, you can use the plain makefile:

```shell
$ make osx && sudo make install
```

### Windows (Cygwin)

```shell
$ make win && sudo make install
```

## Removal:
Removing screenfetch-c is just as easy as installing it:

```shell
$ sudo make uninstall
```

If you installed it via Homebrew:

```shell
$ brew rm screenfetch-c
$ brew untap woodruffw/screenfetch-c # optional
```

## Current Known Compatibility:

_Note:_ These are only the ones that have actually been tested (so far). screenfetch-c may very well work on many of the untested distros, so feel free to try it.

- [x] OS X
- [x] Windows (requires Cygwin)
- [x] Arch Linux
- [x] Fedora
- [x] Linux Mint
- [x] LMDE
- [x] Ubuntu (tested with Lubuntu)
- [x] Fedora
- [x] Debian
- [x] CrunchBang
- [x] Gentoo
- [ ] Funtoo
- [x] FreeBSD
- [x] OpenBSD
- [ ] NetBSD
- [ ] DragonFlyBSD
- [ ] OpenSUSE
- [ ] Mandriva/Mandrake
- [ ] Slackware
- [ ] Red Hat (RHEL)
- [ ] Frugalware
- [x] Peppermint
- [ ] SolusOS
- [ ] Mageia
- [ ] ParabolaGNU/Linux-libre
- [ ] Viperr
- [x] LinuxDeepin
- [ ] Chakra
- [x] Fuduntu
- [x] Trisquel
- [ ] Manjaro
- [x] ElementaryOS (styled as 'elementary OS')
- [ ] Scientific Linux
- [x] Backtrack Linux
- [ ] Sabayon
- [x] Android (requires -D "Android" flag)
- [x] Linux (upon failure to detect a specific distro)
- [x] Solaris (including SunOS, OpenSolaris, and OpenIndiana)
- [x] Angstrom

## License
screenfetch-c is licensed under the MIT license.

For the exact terms, see the [license file](./LICENSE).
