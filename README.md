screenfetch-c
=============

### Current Version: 1.1 Beta (compiled, 99% complete)

screenfetch-c is an attempt to rewrite screenFetch, a popular shell 
script that displays system information and an ASCII logo, in the C 
language.
Like the original screenFetch, screenfetch-c is meant to be compatible with a number of systems, including *BSD, GNU/Linux, OS X, and Windows (via the Cygwin environment).

### Current Status:
screenfetch-c has been extensively tested on Windows 7 (in the Cygwin environment), OS X (10.6 and 10.8), and Linux (see list below).

There are only a few select sections that still require work. They are listed here:
- GPU detection is not implemented on Linux or BSD (appears as "Unknown").
- Distro detection is not 100% complete, although it's close. If screenfetch-c fails to detect a specific distro, try running it with the -D [distro] flag.
- Package detection requires more in-depth testing on OS X (testing for MacPorts/Fink).

### Installing screenfetch-c:

Installing screenfetch-c is very simple.
First, clone the repository:

```
$ git clone https://www.github.com/woodrufw/screenfetch-c.git
$ cd screenfetch-c
```
Then, follow the instructions for your OS:

#### Linux

```
make nix && sudo make install
```

#### Solaris (incl. derivatives)

```
make solaris && sudo make install
```

_Note:_ For Solaris, `make` *must* be GNU make. If it isn't, use `gmake`.

#### BSD

```
make bsd && sudo make install
```

#### OS X

```
make osx && sudo make install
```

#### Windows (Cygwin)

```
make win && sudo make install
```

### Removal:
Removing screenfetch-c is just as easy as installing it:

```
$ sudo make uninstall
```

### Current Known Compatibility:

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
- [ ] Solaris (including SunOS, OpenSolaris, and OpenIndiana)
- [x] Angstrom
