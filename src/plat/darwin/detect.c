/*	detect.c
**	Author: William Woodruff
**	-------------
**
**	The detection functions used by screenfetch-c on OS X (Darwin) are implemented here.
**	Like the rest of screenfetch-c, this file is licensed under the MIT license.
*/

/* standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

/* OS X-specific includes */
#include <sys/types.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/utsname.h>
#include <time.h>
#include <glob.h>
#include <Availability.h>
#include <mach/mach_time.h>
#if __MAC_OS_X_VERSION_MIN_REQUIRED <= 1070
	#include <CoreServices/CoreServices.h> /* for Gestalt */
#endif

/* program includes */
#include "../../arrays.h"
#include "../../misc.h"
#include "../../disp.h"
#include "../../util.h"
#include "../../error_flag.h"

/*	detect_distro
	detects the computer's distribution (OS X release)
*/
void detect_distro(void)
{
#if __MAC_OS_X_VERSION_MIN_REQUIRED <= 1070
	int ver_maj, ver_min, ver_bug;
#else
	FILE *distro_file;
#endif

	/*
		Use this:
		https://www.opensource.apple.com/source/DarwinTools/DarwinTools-1/sw_vers.c
	*/
#if __MAC_OS_X_VERSION_MIN_REQUIRED <= 1070
	Gestalt(gestaltSystemVersionMajor, (SInt32 *) &ver_maj);
	Gestalt(gestaltSystemVersionMinor, (SInt32 *) &ver_min);
	Gestalt(gestaltSystemVersionBugFix, (SInt32 *) &ver_bug);

	snprintf(distro_str, MAX_STRLEN, "Max OS X %d.%d.%d", ver_maj, ver_min, ver_bug);
#else
	distro_file = popen("sw_vers -productVersion | tr -d '\\n'", "r");
	fgets(distro_name_distro_str, MAX_STRLEN, distro_file);
	pclose(distro_file);

	snprintf(distro_str, MAX_STRLEN, "Mac OS X %s", distro_name_str);
#endif

	safe_strncpy(host_color, TLBL, MAX_STRLEN);

	return;
}

/*	detect_host
	detects the computer's hostname and active user and formats them
*/
void detect_host(void)
{
	char *given_user = "Unknown";
	char given_host[MAX_STRLEN] = "Unknown";
	struct utsname host_info;

	given_user = getlogin();
	uname(&host_info);
	safe_strncpy(given_host, host_info.nodename, MAX_STRLEN);

	snprintf(host_str, MAX_STRLEN, "%s@%s", given_user, given_host);

	return;
}

/*	detect_kernel
	detects the computer's kernel
*/
void detect_kernel(void)
{
	struct utsname kern_info;

	uname(&kern_info);
	snprintf(kernel_str, MAX_STRLEN, "%s %s %s", kern_info.sysname,
			kern_info.release, kern_info.machine);

	return;
}

/*	detect_uptime
	detects the computer's uptime
*/
void detect_uptime(void)
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

	uptime = (long long)((mach_absolute_time() * timebase_info.numer) /
				(1000* 1000 * timebase_info.denom));
	uptime /= 1000;

	split_uptime(uptime, &secs, &mins, &hrs, &days);

	if (days > 0)
		snprintf(uptime_str, MAX_STRLEN, "%dd %dh %dm %ds", days, hrs, mins, secs);
	else
		snprintf(uptime_str, MAX_STRLEN, "%dh %dm %ds", hrs, mins, secs);

	return;
}

/*	detect_pkgs
	detects the number of packages installed on the computer
*/
void detect_pkgs(void)
{
	int packages = 0;
	glob_t gl;

	if (glob("/usr/local/Cellar/*", GLOB_NOSORT, NULL, &gl) == 0)
	{
		packages = gl.gl_pathc;
	}
	else if (error)
	{
		ERR_REPORT("Failure while globbing packages.");
	}

	globfree(&gl);

	snprintf(pkgs_str, MAX_STRLEN, "%d", packages);

	return;
}

/*	detect_cpu
	detects the computer's CPU brand/name-string
*/
void detect_cpu(void)
{
	FILE *cpu_file;

	/*
		something like:
		int len = MAX_STRLEN;
		sysctlbyname("machdep.cpu.brand_string", str, &len, NULL, 0);
	*/
	cpu_file = popen("sysctl -n machdep.cpu.brand_string | "
				"sed 's/(\\([Tt][Mm]\\))//g;s/(\\([Rr]\\))//g;s/^//g' | "
				"tr -d '\\n' | tr -s ' '", "r");
	fgets(cpu_str, MAX_STRLEN, cpu_file);
	pclose(cpu_file);

	return;
}

/*	detect_gpu
	detects the computer's GPU brand/name-string
*/
void detect_gpu(void)
{
	FILE *gpu_file;

	gpu_file = popen("system_profiler SPDisplaysDataType | "
				"awk -F': ' '/^\\ *Chipset Model:/ {print $2}' | "
				"tr -d '\\n'", "r");
	fgets(gpu_str, MAX_STRLEN, gpu_file);
	pclose(gpu_file);

	return;
}

/*	detect_disk
	detects the computer's total disk capacity and usage
*/
void detect_disk(void)
{
	struct statfs disk_info;
	long disk_total = 0, disk_used = 0, disk_percentage = 0;

	if (!(statfs(getenv("HOME"), &disk_info)))
	{
		disk_total = ((disk_info.f_blocks * disk_info.f_bsize) / GB);
		disk_used = (((disk_info.f_blocks - disk_info.f_bfree)
					* disk_info.f_bsize) / GB);
		disk_percentage = (((float) disk_used / disk_total) * 100);
		snprintf(disk_str, MAX_STRLEN, "%ldG / %ldG (%ld%%)", disk_used, disk_total,
				disk_percentage);
	}
	else if (error)
	{
		ERR_REPORT("Could not stat $HOME for filesystem statistics.");
	}

	return;
}

/*	detect_mem
	detects the computer's total and used RAM
*/
void detect_mem(void)
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

	snprintf(mem_str, MAX_STRLEN, "%lld%s / %lld%s", used_mem, "MB", total_mem, "MB");

	return;
}

/*	detect_shell
	detects the shell currently running on the computer
	--
	CAVEAT: shell version detection relies on the standard versioning format for 
	each shell. If any shell's older (or newer versions) suddenly begin to use a
	new	scheme, the version may be displayed incorrectly.
	--
*/
void detect_shell(void)
{
	FILE *shell_file;
	char *shell_name;
	char vers_str[MAX_STRLEN];

	shell_name = getenv("SHELL");

	if (shell_name == NULL)
	{
		if (error)
			ERR_REPORT("Could not detect a shell.");

		return;
	}

	if (STREQ(shell_name, "/bin/sh"))
	{
		safe_strncpy(shell_str, "POSIX sh", MAX_STRLEN);
	}
	else if (strstr(shell_name, "bash"))
	{
		shell_file = popen("bash --version | head -1", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);
		snprintf(shell_str, MAX_STRLEN, "bash %.*s", 17, vers_str + 10);
		pclose(shell_file);
	}
	else if (strstr(shell_name, "zsh"))
	{
		shell_file = popen("zsh --version", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);	
		snprintf(shell_str, MAX_STRLEN, "zsh %.*s", 5, vers_str + 4);
		pclose(shell_file);
	}
	else if (strstr(shell_name, "csh"))
	{
		shell_file = popen("csh --version | head -1", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);
		snprintf(shell_str, MAX_STRLEN, "csh %.*s", 7, vers_str + 5);
		pclose(shell_file);
	}
	else if (strstr(shell_name, "fish"))
	{
		shell_file = popen("fish --version", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);
		snprintf(shell_str, MAX_STRLEN, "fish %.*s", 13, vers_str + 6);
		pclose(shell_file);
	}
	else if (strstr(shell_name, "dash") || strstr(shell_name, "ash")
			|| strstr(shell_name, "ksh"))
	{
		/* i don't have a version detection system for these, yet */
		safe_strncpy(shell_str, shell_name, MAX_STRLEN);
	}

	return;
}

/*	detect_res
	detects the combined resolution of all monitors attached to the computer
*/
void detect_res(void)
{
	FILE *res_file;

	res_file = popen("system_profiler SPDisplaysDataType | "
				"awk '/Resolution:/ {print $2\"x\"$4}' | tr -d '\\n'", "r");
	fgets(res_str, MAX_STRLEN, res_file);
	pclose(res_file);

	return;
}

/*	detect_de
	detects the desktop environment currently running on top of the OS.
	On OS X, this will always be Aqua.
*/
void detect_de(void)
{
	safe_strncpy(de_str, "Aqua", MAX_STRLEN);

	return;
}

/*	detect_wm
	detects the window manager currently running on top of the OS.
	On OS X, this will always be the Quartz Compositor.
	argument char *str: the char array to be filled with the WM name
*/
void detect_wm(char *str)
{
	safe_strncpy(str, "Quartz Compositor", MAX_STRLEN);

	return;
}

/*	detect_wm_theme
	detects the theme associated with the WM detected in detect_wm().
	On OS X, this will always be Aqua.
	argument char *str: the char array to be filled with the WM Theme name
*/
void detect_wm_theme(char *str, const char *wm_str)
{
	safe_strncpy(str, "Aqua", MAX_STRLEN);

	return;
}

/*	detect_gtk
	OS X doesn't use GTK, so this function fills str with "Not Applicable"
	argument char *str: the char array to be filled with any GTK info
*/
void detect_gtk(char *str)
{
	safe_strncpy(str, "Not Applicable", MAX_STRLEN);

	return;
}
