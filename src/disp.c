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
    called if the -v flag is tripped
*/
void display_version(void)
{
	printf("%s\n", "screenfetch-c - Version " SCREENFETCH_C_VERSION_MAJOR "." SCREENFETCH_C_VERSION_MINOR ", revision " SCREENFETCH_C_VERSION_RELEASE ".");
	return;
}

/*  display_help
    called if the -h flag is tripped
*/
void display_help(void)
{
	display_version();
	printf("\n%s\n", "Options:");
	printf(" %s\n", "-m, --manual\t\t\t Enter manual mode.");
	printf(" %s\n", "-v, --verbose\t\t\t Enable verbosity during output.");
	printf(" %s\n", "-s, --screenshot\t\t Take a screenshot.");
	printf(" %s\n", "-n, --no-logo\t\t\t Print output without a logo.");
	printf(" %s\n", "-D, --distro [DISTRO]\t\t Print output with DISTRO's logo.");
	printf(" %s\n", "-E, --suppress-errors\t\t Suppress error output.");
	printf(" %s\n", "-V, --version\t\t\t Output screenfetch-c's version and exit.");
	printf(" %s\n", "-h, --help\t\t\t Output this help information.");
	printf(" %s\n", "-L, --logo-only [DISTRO]\t Output only the logo associated with DISTRO.");
	printf("\n%s\n", "For more information, consult screenfetch-c's man page.");
	return;
}
