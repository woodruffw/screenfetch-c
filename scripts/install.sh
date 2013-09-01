#!/bin/bash
#  build-install-screenfetch.sh
#  Author: William Woodruff
#  ---
#  Function:
#  fetches the most recent stable version of screenfetch-c, builds it,
#  and moves it to /usr/bin/screenfetch.
#  Also fetches the most recent manpage and installs it.
#  NOTE: Requires a superuser's password.
#  ---
#  Dependencies:
#  Requires wget to fetch the files. 
#  Requires gcc to compile screenfetch-c.

printf "Warning: This script will install screenfetch-c as \'screenfetch\'.\n"
printf "Are you sure you want to continue? (y/n) "
read ans

if [ "$ans" = "y" ] ; then
	printf "Fetching files..."
	wget https://raw.github.com/woodrufw/screenfetch-c/master/screenfetch-c.c --no-check-certificate -O ~/screenfetch-c.c 2> /dev/null
	wget https://raw.github.com/woodrufw/screenfetch-c/master/screenfetch-c.h --no-check-certificate -O ~/screenfetch-c.h 2> /dev/null
	wget https://raw.github.com/woodrufw/screenfetch-c/master/manpage/screenfetch.man --no-check-certificate -O ~/screenfetch.man 2> /dev/null
	printf "done\n"

	printf "Compiling screenfetch-c..."
	gcc ~/screenfetch-c.c -o ~/screenfetch -std=c99
	printf "done\n"

	printf "Moving files into place..."
	mv ~/screenfetch /usr/bin/
	mv ~/screenfetch.man /usr/share/man/man1/screenfetch.1
	gzip /usr/share/man/man1/screenfetch.1
	printf "done\n"

	printf "Cleaning up..."
	rm ~/screenfetch-c.c ~/screenfetch-c.h
	printf "done\n"

	printf "Installation complete.\n"
	printf "In order to invoke screenfetch, make sure that /usr/bin is in your PATH\n"
	exit 1
else
	printf "Installation terminated.\n"
	exit 1
fi