#!/bin/bash
#  remove.sh
#  Author: William Woodruff
#  ---
#  Function: 
#  Removes any and all traces of screenfetch-c, provided it 
#  was installed via 'install.sh' 
#  If screenfetch-c was installed manually or install.sh
#  was modified, this script may not work partially or at all.

printf "Warning: This script will remove screenfetch-c from your computer!\n"
printf "Are you ABSOLUTELY sure you want to continue? (y/n) "
read ans

if [ "$ans" = "y" ] ; then
	printf "Removing files..."
	rm -f /usr/bin/screenfetch
	rm -f /usr/share/man/man1/screenfetch.1.gz
	printf "done\n"
	printf "Uninstallation complete.\n"
else
	printf "Uninstallation canceled. \n"
	exit 1
fi

