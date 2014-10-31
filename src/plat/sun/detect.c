/* standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

/* Solaris-specific includes */
#include <utmpx.h>
#include <time.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/utsname.h>
#include <X11/Xlib.h>

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
	FILE *distro_file;

	distro_file = popen("uname -sr | tr -d '\\n'", "r");
	fgets(str, MAX_STRLEN, distro_file);
	pclose(distro_file);

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

/*	detect_uptime
	detects the computer's uptime
	argument char *str: the char array to be filled with the uptime in format '$d $h $m $s' where $ is a number
*/
void detect_uptime(char *str)
{
	long uptime = 0, currtime = 0, boottime = 0;

	int secs = 0;
	int mins = 0;
	int hrs = 0;
	int days = 0;

	currtime = time(NULL);
	struct utmpx *ent;

	while ((ent = getutxent()))
	{
		if (STRCMP("system boot", ent->ut_line))
		{
			boottime = ent->ut_tv.tv_sec;
		}
	}

	uptime = currtime - boottime;

	split_uptime(uptime, &secs, &mins, &hrs, &days);

	if (days > 0)
		snprintf(str, MAX_STRLEN, "%dd %dh %dm %ds", days, hrs, mins, secs);
	else
		snprintf(str, MAX_STRLEN, "%dh %dm %ds", hrs, mins, secs);

	return;
}

/*	detect_cpu
	detects the computer's CPU brand/name-string
	argument char *str: the char array to be filled with the CPU name
*/
void detect_cpu(char *str)
{
	FILE *cpu_file;

	cpu_file = popen("psrinfo -pv | tail -1 | tr -d '\\t\\n'", "r");
	fgets(str, MAX_STRLEN, cpu_file);
	pclose(cpu_file);

	return;
}