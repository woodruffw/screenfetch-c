screenfetch-c
=============

### Current Version: 1.1 Beta (compiled, 99% complete)

screenfetch-c is an attempt to rewrite screenFetch, a popular shell 
script that displays system information and an ASCII logo, in the C 
language.
Like the original screenFetch, screenfetch-c is meant to be compatible with a number of systems, including *BSD, GNU/Linux, OS X, and Windows (via the Cygwin environment).

### Current Status:
screenfetch-c has been extensively tested on Windows 7 (in the Cygwin environment), OS X (10.6 and 10.8), and Linux (see list below).
It has not yet been tested on any BSD distro. Therefore, I currently advise against using screenfetch-c on *BSD, even if it does compile successfully. However, it is up to you.

There are only a few select sections that still require work. They are listed here:
- DE/WM/WM Theme/GTK detection is not complete on *BSD or Linux. As a result, they will appear as 'Unknown' in output (unless specified in manual mode).
- Distro detection is not 100% complete, although it's close. If screenfetch-c fails to detect a specific distro, try running it with the -D [distro] flag.
- Package detection requires more in-depth testing.

### Installing screenfetch-c:

#### Using the provided script (install.sh):
screenfetch-c can be installed very easily via the install.sh script provided in the /scripts directory.

Simply download the script and run it:
```
sudo ./install.sh
```

It will fetch all source code (and manpages) required, compile screenfetch, install the manpage, and leave a tidy binary in $HOME. You can then move that binary whereever you please.

_Note_: install.sh requires sudo to install the manpage! That is, unless you're on Cygwin. In that case, you're fine.

#### Manual compilation:
Alternatively, you could always compile screenfetch-c yourself. It's very easy to do, as 
screenfetch-c has no dependencies outside of the standard libraries of each OS.
screenfetch-c is designed to be C99 compliant, although compiling with C90 should work provided
you have the snprintf() function.

```
gcc -std=c99 screenfetch-c.c -o screenfetch
```

Just be sure that screenfetch-c.h is in the same directory, as it contains all of 
screenfetch-c.c's prototypes and macros. If you want to be able to detect DE/WM/WM Theme/GTK information without manual mode, you will also need the scripts (chmoded to executable, of course). They _must_ be stored in the same directory as the screenfetch executable, or it will not be able to utilize them.
You can also choose the install screenfetch-c's manpage, if you wish.

##### Removal:
Removing screenfetch-c is just as easy as installing it.
Simply delete the binary, ~/.screenfetchc (if you used manual mode), and the manpage, which is stored in /usr/share/man/man1/screenfetch.1.gz

If you compiled screenfetch-c manually, the final step may not be required.

### Current Known Compatibility:

_Note:_ These are only the ones that have actually been tested (so far). screenfetch-c may very well work on many of the untested distros, so feel free to try it.

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
- [x] Linux (upon failure to detect a specific distro)
