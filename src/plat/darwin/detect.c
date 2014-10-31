/* standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

/* OS X-specific includes */
#include <sys/utsname.h>
#include <time.h>
#include <glob.h>
#include <Availability.h>
#include <mach/mach_time.h>
#if __MAC_OS_X_VERSION_MIN_REQUIRED <= 1070
	#include <CoreServices/CoreServices.h> /* for Gestalt */
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
	/*
		Use this:
		https://www.opensource.apple.com/source/DarwinTools/DarwinTools-1/sw_vers.c
	*/
	#if __MAC_OS_X_VERSION_MIN_REQUIRED <= 1070
		int ver_maj, ver_min, ver_bug;
		Gestalt(gestaltSystemVersionMajor, (SInt32 *) &ver_maj);
		Gestalt(gestaltSystemVersionMinor, (SInt32 *) &ver_min);
		Gestalt(gestaltSystemVersionBugFix, (SInt32 *) &ver_bug);

		snprintf(str, MAX_STRLEN, "Max OS X %d.%d.%d", ver_maj, ver_min, ver_bug);
	#else
		FILE *distro_file;

		distro_file = popen("sw_vers -productVersion | tr -d '\\n'", "r");
		fgets(distro_name_str, MAX_STRLEN, distro_file);
		pclose(distro_file);

		snprintf(str, MAX_STRLEN, "Mac OS X %s", distro_name_str);
	#endif

	return;
}

/*	detect_arch
	detects the computer's architecture
	argument char *str: the char array to be filled with the architecture
*/
void detect_arch(char *str)
{
	struct utsname arch_info;
	uname(&arch_info);
	safe_strncpy(str, arch_info.machine, MAX_STRLEN);

	return;
}

/*	detect_host
	detects the computer's hostname and active user and formats them
	argument char *str: the char array to be filled with the user and hostname in format 'user@host'
*/
void detect_host(char *str)
{
	char *given_user = "Unknown";
	char given_host[MAX_STRLEN] = "Unknown";

	given_user = getlogin(); /* getlogin is apparently buggy on linux, so this might be changed */

	struct utsname host_info;
	uname(&host_info);
	safe_strncpy(given_host, host_info.nodename, MAX_STRLEN);

	snprintf(str, MAX_STRLEN, "%s@%s", given_user, given_host);

	return;
}

/*	detect_kernel
	detects the computer's kernel
	argument char *str: the char array to be filled with the kernel name
*/
void detect_kernel(char *str)
{
	struct utsname kern_info;
	uname(&kern_info);
	snprintf(str, MAX_STRLEN, "%s %s", kern_info.sysname, kern_info.release);

	return;
}