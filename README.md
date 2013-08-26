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
screenfetch-c has been successfully compiled and run on Cygwin and OS X, 
with most information being displayed correctly.
However, more testing needs to be done.
Note: Very little testing has been done on ascii_logos.h, and
screenfetch-c.c does not currently rely on it at all. If you wish to compile screenfetch-c, please do so _without_ ascii_logos.h.

The next big step will be the inclusion of ASCII logos.
