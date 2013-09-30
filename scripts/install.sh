#!/bin/bash
#  install.sh
#  Author: William Woodruff
#  ---
#  Function:
#  fetches the most recent stable version of screenfetch-c, builds it,
#  and moves it to /usr/bin/screenfetch.
#  Also fetches the most recent manpage and installs it.
#  NOTE: Requires sudo
#  ---
#  Dependencies:
#  Requires wget to fetch the files. 
#  Requires gcc to compile screenfetch-c.

printf "Warning: This script will install screenfetch-c as \'screenfetch\'.\n"
printf "Are you ABSOLUTELY sure you want to continue? (y/n) "
read ans

if [ "$ans" = "y" ] ; then
	printf "Fetching files..."
	wget https://raw.github.com/woodrufw/screenfetch-c/master/src/screenfetch-c.c --no-check-certificate -O ~/screenfetch-c.c 2> /dev/null
	wget https://raw.github.com/woodrufw/screenfetch-c/master/src/screenfetch-c.h --no-check-certificate -O ~/screenfetch-c.h 2> /dev/null
	wget https://raw.github.com/woodrufw/screenfetch-c/master/src/detectde --no-check-certificate -O ~/detectde 2> /dev/null
	wget https://raw.github.com/woodrufw/screenfetch-c/master/src/detectwm --no-check-certificate -O ~/detectwm 2> /dev/null
	wget https://raw.github.com/woodrufw/screenfetch-c/master/src/detectwmtheme --no-check-certificate -O ~/detectwmtheme 2> /dev/null
	wget https://raw.github.com/woodrufw/screenfetch-c/master/src/detectgtk --no-check-certificate -O ~/detectgtk 2> /dev/null
	wget https://raw.github.com/woodrufw/screenfetch-c/master/manpage/screenfetch.man --no-check-certificate -O ~/screenfetch.man 2> /dev/null
	printf "done\n"

	printf "Compiling screenfetch-c..."
	gcc ~/screenfetch-c.c -o ~/screenfetch -std=c99
	chmod +x ~/detectde
	chmod +x ~/detectwm
	chmod +x ~/detectwmtheme
	chmod +x ~/detectgtk
	printf "done\n"

	printf "Moving manpage into place..."
	mv ~/screenfetch.man /usr/share/man/man1/screenfetch.1
	gzip /usr/share/man/man1/screenfetch.1
	printf "done\n"

	printf "Cleaning up..."
	rm -f ~/screenfetch-c.*
	rm -f ~/screenfetch.man
	printf "done\n"

	printf "Installation complete.\n"
	exit 1
else
	printf "Installation terminated.\n"
	exit 1
fi