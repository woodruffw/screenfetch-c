screenfetch-c
=============

## Current Version: 1.2 release 

screenfetch-c is an attempt to rewrite screenFetch, a popular shell 
script that displays system information and an ASCII logo, in the C 
language.
Like the original screenFetch, screenfetch-c is meant to be compatible with a number of systems, including *BSD, GNU/Linux, OS X, and Windows (via the Cygwin environment).

## Installing screenfetch-c:

### Dependencies:
screenfetch-c only relies on libraries that come on virtually every system.

* On Linux, OpenGL is used to detect the GPU. In most package managers, this is listed as `libgl-dev`.

* On Linux and Solaris, libX11 is required. In most package managers, this is listed as `libX11-dev`.

* On Windows, the WinAPI is required (duh).

Installing screenfetch-c is very simple.
First, clone the repository:

```bash
$ git clone https://www.github.com/woodruffw/screenfetch-c.git
$ cd screenfetch-c
```
Then, follow the instructions for your OS:

### Linux

```bash
$ make linux && sudo make install
```

### Solaris (incl. derivatives)

```bash
$ make solaris && sudo make install
```

_Note:_ For Solaris, `make` *must* be GNU make. If it isn't, use `gmake`.

### BSD

```bash
$ make bsd && sudo make install
```

### OS X

screenfetch-c is now available via Homebrew!

To install it, just run the following:

```bash
$ brew install woodruffw/screenfetch-c/screenfetch-c
```

Alternatively, you can use the plain makefile:

```bash
$ make osx && sudo make install
```

### Windows (Cygwin)

```bash
$ make win && sudo make install
```

## Removal:
Removing screenfetch-c is just as easy as installing it:

```bash
$ sudo make uninstall
```

If you installed it via Homebrew:

```bash
$ brew rm screenfetch-c
$ brew untap woodruffw/screenfetch-c # optional
```

## Contributing

There are a number of things that still need to be tested or completed in screenfetch-c.

If you think that you can test/fix/improve one or more of them, feel free to fork and issue requests back to me.

In particular, the following things still need to be done:

- Comprehensive testing of GPU detection on Linux/BSD/Solaris
- Matching screencapture capabilities on Windows with OS X/Linux/BSD/Solaris
- Improve RAM/HDD detection on BSD
- Fix manual mode on Windows
- Improve features on Android

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
