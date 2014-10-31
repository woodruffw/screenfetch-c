/* standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

/* BSD-specific includes */
#if defined(__FreeBSD__)
	#include <sys/utsname.h>
	#include <time.h>
#elif defined(__NetBSD__)
	#include <sys/utsname.h>
#elif defined(__OpenBSD__)
	#define OS OPENBSD
	#include <sys/utsname.h>
	#include <time.h>
#elif defined(__DragonFly__)
	#include <sys/utsname.h>
	#include <time.h>
#endif

/* program includes */
#include "../../misc.h"
#include "../../disp.h"
#include "../../util.h"

/*	detect_distro
	detects the computer's distribution (really only relevant on Linux)
	argument char *str: the char array to be filled with the distro name
*/
void detect_distro(char *str, bool error)
{
	distro_file = popen("uname -sr | tr -d '\\n'", "r");
	fgets(str, MAX_STRLEN, distro_file);
	pclose(distro_file);

	return;
}