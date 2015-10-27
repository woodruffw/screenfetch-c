screenfetch-c
=============

[![Build Status](https://drone.io/github.com/woodruffw/screenfetch-c/status.png)](https://drone.io/github.com/woodruffw/screenfetch-c/latest)

## Current Version: 1.4 (release)

screenfetch-c is an attempt to rewrite screenFetch, a popular shell 
script that displays system information and an ASCII logo, in the C 
language.
Like the original screenFetch, screenfetch-c is meant to be compatible with a number of systems, including *BSD, Solaris, GNU/Linux, OS X, and Windows (via the Cygwin environment).

## Installing screenfetch-c:

### Dependencies:
screenfetch-c only relies on libraries that come on virtually every system.

* On Linux, OpenGL is used to detect the GPU. In most package managers, this is listed as `libgl-dev`.

* On Linux and Solaris, libX11 is required. In most package managers, this is listed as `libX11-dev`.

Installing screenfetch-c is very simple:

```bash
$ git clone https://www.github.com/woodruffw/screenfetch-c.git
$ cd screenfetch-c
$ make
```

_Note:_ For Solaris, `make` *must* be GNU make. If it isn't, use `gmake`. Using regular (Sun) `make` will cause strange errors.

### OS X

screenfetch-c is also available via Homebrew!

To install it, just run the following:

```bash
$ brew install woodruffw/screenfetch-c/screenfetch-c
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

- Improve error checking and handling.
- Matching screencapture capabilities on Windows with OS X/Linux/BSD/Solaris
- Improve RAM/HDD detection on BSD
- Fix manual mode on Windows
- Improve features on Android
- Improve/replace configuration format and parser

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
- [x] Kali Linux
- [ ] Sabayon
- [x] Android (requires -D "Android" flag)
- [x] Linux (upon failure to detect a specific distro)
- [x] Solaris (including SunOS, OpenSolaris, and OpenIndiana)
- [x] Angstrom

## License
screenfetch-c is licensed under the MIT license.

For the exact terms, see the [license](./LICENSE).
