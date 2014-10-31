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

/*	detect_uptime
	detects the computer's uptime
	argument char *str: the char array to be filled with the uptime in format '$d $h $m $s' where $ is a number
*/
void detect_uptime(char *str)
{
	long long uptime = 0;

	int secs = 0;
	int mins = 0;
	int hrs = 0;
	int days = 0;

	/* three cheers for undocumented functions and structs */
	static mach_timebase_info_data_t timebase_info;

	if (timebase_info.denom == 0)
	{
		(void) mach_timebase_info(&timebase_info);
	}

	uptime = (long long)((mach_absolute_time() * timebase_info.numer) / (1000* 1000 * timebase_info.denom));
	uptime /= 1000;

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

	/*
		something like:
		int len = MAX_STRLEN;
		sysctlbyname("machdep.cpu.brand_string", str, &len, NULL, 0);
	*/
	cpu_file = popen("sysctl -n machdep.cpu.brand_string | sed 's/(\\([Tt][Mm]\\))//g;s/(\\([Rr]\\))//g;s/^//g' | tr -d '\\n' | tr -s ' '", "r");
	fgets(str, MAX_STRLEN, cpu_file);
	pclose(cpu_file);

	return;
}

/*	detect_gpu
	detects the computer's GPU brand/name-string
	argument char *str: the char array to be filled with the GPU name
*/
void detect_gpu(char *str, bool error)
{
	FILE *gpu_file;

	gpu_file = popen("system_profiler SPDisplaysDataType | awk -F': ' '/^\\ *Chipset Model:/ {print $2}' | tr -d '\\n'", "r");
	fgets(str, MAX_STRLEN, gpu_file);
	pclose(gpu_file);

	return;
}

/*	detect_disk
	detects the computer's total disk capacity and usage
	argument char *str: the char array to be filled with the disk data in format '$G / $G ($G%)', where $ is a number
*/
void detect_disk(char *str)
{
	FILE *disk_file;

	int disk_total = 0;
	int disk_used = 0;
	int disk_percentage = 0;

	disk_file = popen("df -H 2> /dev/null | grep -vE '^[A-Z]\\:\\/|File' | awk '{ print $2 }' | head -1 | tr -d '\\r\\n G'", "r");
	fscanf(disk_file, "%d", &disk_total);
	pclose(disk_file);

	disk_file = popen("df -H 2> /dev/null | grep -vE '^[A-Z]\\:\\/|File' | awk '{ print $3 }' | head -1 | tr -d '\\r\\n G'", "r");
	fscanf(disk_file, "%d", &disk_used);
	pclose(disk_file);

	if (disk_total > disk_used)
	{
		disk_percentage = (((float) disk_used / disk_total) * 100);

		snprintf(str, MAX_STRLEN, "%dG / %dG (%d%%)", disk_used, disk_total, disk_percentage);
	}
	else /* when disk_used is in a smaller unit */
	{
		disk_percentage = ((float) disk_used / (disk_total * 1024) * 100);

		snprintf(str, MAX_STRLEN, "%dM / %dG (%d%%)", disk_used, disk_total, disk_percentage);
	}

	return;
}

/*	detect_mem
	detects the computer's total and used RAM
	argument char *str: the char array to be filled with the memory data in format '$MB / $MB', where $ is a number
*/
void detect_mem(char *str)
{
	FILE *mem_file;

	long long total_mem = 0;
	long long free_mem = 0;
	long long used_mem = 0;

	mem_file = popen("sysctl -n hw.memsize", "r");
	fscanf(mem_file, "%lld", &total_mem);
	pclose(mem_file);

	mem_file = popen("vm_stat | head -2 | tail -1 | tr -d 'Pages free: .'", "r");
	fscanf(mem_file, "%lld", &free_mem);
	pclose(mem_file);

	total_mem /= (long) MB;

	free_mem *= 4096; /* 4KiB is OS X's page size */
	free_mem /= (long) MB;

	used_mem = total_mem - free_mem;

	snprintf(str, MAX_STRLEN, "%lld%s / %lld%s", used_mem, "MB", total_mem, "MB");

	return;
}

/*	detect_shell
	detects the shell currently running on the computer
	argument char *str: the char array to be filled with the shell name and version
	--
	CAVEAT: shell version detection relies on the standard versioning format for 
	each shell. If any shell's older (or newer versions) suddenly begin to use a new
	scheme, the version may be displayed incorrectly.
	--
*/
void detect_shell(char *str, bool error)
{
	FILE *shell_file;

	char *shell_name;
	char vers_str[MAX_STRLEN];

	shell_name = getenv("SHELL");

	if (shell_name == NULL)
	{
		if (error)
			ERROR_OUT("Error: ", "Problem detecting shell.");

		return;
	}

	if (strstr(shell_name, "bash"))
	{
		shell_file = popen("bash --version | head -1", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);
		/* evil pointer arithmetic */
		snprintf(str, MAX_STRLEN, "bash %.*s", 17, vers_str + 10);
		pclose(shell_file);
	}

	else if (strstr(shell_name, "zsh"))
	{
		shell_file = popen("zsh --version", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);	
		/* evil pointer arithmetic */
		snprintf(str, MAX_STRLEN, "zsh %.*s", 5, vers_str + 4);
		pclose(shell_file);
	}

	else if (strstr(shell_name, "csh"))
	{
		shell_file = popen("csh --version | head -1", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);
		/* evil pointer arithmetic */
		snprintf(str, MAX_STRLEN, "csh %.*s", 7, vers_str + 5);
		pclose(shell_file);
	}

	else if (strstr(shell_name, "fish"))
	{
		shell_file = popen("fish --version", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);
		/* evil pointer arithmetic */
		snprintf(str, MAX_STRLEN, "fish %.*s", 13, vers_str + 6);
		pclose(shell_file);
	}

	else if (strstr(shell_name, "dash") || strstr(shell_name, "ash") || strstr(shell_name, "ksh"))
	{
		/* i don't have a version detection system for these, yet */
		safe_strncpy(str, shell_name, MAX_STRLEN);
	}

	return;
}

/*	detect_res
	detects the combined resolution of all monitors attached to the computer
	argument char *str: the char array to be filled with the resolution in format '$x$', where $ is a number
*/
void detect_res(char *str, bool error)
{
	FILE *res_file;

	res_file = popen("system_profiler SPDisplaysDataType | awk '/Resolution:/ {print $2\"x\"$4}' | tr -d '\\n'", "r");
	fgets(str, MAX_STRLEN, res_file);
	pclose(res_file);

	return;
}

/*	detect_de
	detects the desktop environment currently running on top of the OS
	argument char *str: the char array to be filled with the DE name
	--
	CAVEAT: On *BSDs and Linux distros, this function relies on the presence of 
	'detectde', a shell script. If it isn't present in the working directory, the DE will be set as 'Unknown'
	--
*/
void detect_de(char *str)
{
	safe_strncpy(str, "Aqua", MAX_STRLEN);

	return;
}

/*	detect_wm
	detects the window manager currently running on top of the OS
	argument char *str: the char array to be filled with the WM name
	--
	CAVEAT: On *BSDs and Linux distros, this function relies on the presence of 
	'detectwm', a shell script. If it isn't present in the working directory, the WM will be set as 'Unknown'
	--
*/
void detect_wm(char *str)
{
	safe_strncpy(str, "Quartz Compositor", MAX_STRLEN);

	return;
}
