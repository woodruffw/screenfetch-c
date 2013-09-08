screenfetch-c
=============

### Current Version: 0.5 Alpha (compiled, not complete)

screenfetch-c is an attempt to rewrite screenFetch, a popular shell 
script that displays system information and an ASCII logo, in the C 
language.
Like the original screenFetch, screenfetch-c will be capable of 
compiling and running on Windows (via Cygwin), all Linux distros, the 
four major *BSD distros, and OS X.

### Current Status:
Right now, the majority of detection functions are complete.
However, some of the larger ones (detect_distro(), detect_wm(), 
detect_pkgs()) are lacking Linux/BSD sections due to a lack of a testing 
environment.
screenfetch-c has been successfully compiled and run on Cygwin, OS X, and a select few Linux distros (see below), 
with most information being displayed correctly.
However, more testing needs to be done.
Current phase: completion of BSD/Linux sections, bug squashing, general testing.

### Installing screenfetch-c:

#### Using the provided script (install.sh):
screenfetch-c can be installed very easily via the install script provided in the /scripts directory.

Simply download the script and run it:
```
sudo ./install.sh
```

It will fetch all source code (and manpages) required, build the program, move it to /usr/bin/screenfetch, and clean up after itself.
If you ever need to remove screenfetch-c for any reason, remove.sh is also provided.
_Note:_ Make sure to run both with sudo, or the installation will be incomplete!

#### Manual compilation:
Alternatively, you could always compile screenfetch-c yourself. It's very easy to do, as 
screenfetch-c has no dependencies outside of the standard/POSIX libraries.

```
gcc screenfetch-c.c -o screenfetch
```

Just be sure that screenfetch-c.h is in the same directory, as it contains all of 
screenfetch-c.c's prototypes and macros.
You can also choose the install screenfetch-c's manpage, if you wish.

### Current Compatibility:
- [x] OS X
- [x] Windows (requires Cygwin)
- [x] Fedora
- [x] Linux Mint
- [x] LMDE
- [x] Ubuntu (tested with Lubuntu)
- [x] Fedora
- [x] Debian
- [x] CrunchBang
- [ ] Gentoo
- [ ] Funtoo
- [ ] FreeBSD
- [ ] OpenBSD
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
- [ ] Linux (upon failure to detect a specific distro)
