/*	disp.c
	Author: William Woodruff
	-------------

	Functions used by screenfetch-c for displaying version
	and help output to the user.
	Like the rest of screenfetch-c, this file is licensed under the MIT license.
	You should have received a copy of it with this code.
*/

#include <stdio.h>
#include "version.h"

/*  display_version
    called if the -v flag is tripped, outputs the current version of screenfetch-c
*/
void display_version(void)
{
	printf("%s\n", "screenfetch-c - Version " SCREENFETCH_C_VERSION_MAJOR "." SCREENFETCH_C_VERSION_MINOR ", revision " SCREENFETCH_C_VERSION_RELEASE ".");
	return;
}

/*  display_help
    called if the -h flag is tripped, tells the user where to find the manpage
*/
void display_help(void)
{
	printf("%s\n", "screenfetch-c");
	printf("%s\n", "A rewrite of screenFetch, the popular shell script, in C.");
	printf("%s\n", "Operating Systems currently supported:");
	printf("%s\n", "Windows (via Cygwin), Linux, *BSD, OS X, and Solaris.");
	printf("%s\n", "Using screenfetch-c on an OS not listed above may not work entirely or at all (and is disabled by default).");
	printf("%s\n", "Please access 'man screenfetch' for in-depth information on compatibility and usage.");
	return;
}