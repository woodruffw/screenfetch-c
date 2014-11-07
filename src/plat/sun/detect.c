/*	detect.c
	Author: William Woodruff
	-------------

	The detection functions used by screenfetch-c on Solaris are implemented here.
	Like the rest of screenfetch-c, this file is licensed under the MIT license.
	You should have received a copy of it with this code.
*/

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
#include <sys/statvfs.h>
#include <X11/Xlib.h>

/* program includes */
#include "../../misc.h"
#include "../../disp.h"
#include "../../util.h"

/*	detect_distro
	detects the computer's distribution
	argument char *str: the char array to be filled with the distro name
	argument bool error: true for verbose errors, false for silent
*/
void detect_distro(char *str, bool error)
{
	struct utsname distro_info;
	uname(&distro_info);
	snprintf(str, MAX_STRLEN, "%s", distro_info.sysname);

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
	snprintf(str, MAX_STRLEN, "%s", kern_info.release);

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

/*	detect_pkgs
	detects the number of packages installed on the computer
	argument char *str: the char array to be filled with the number of packages
*/
void detect_pkgs(char *str, const char *distro_str, bool error)
{
	FILE *pkgs_file;
	int packages = 0;

	pkgs_file = popen("pkg list | wc -l", "r");
	fscanf(pkgs_file, "%d", &packages);
	pclose(pkgs_file);

	snprintf(str, MAX_STRLEN, "%d", packages);

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

/*	detect_gpu
	detects the computer's GPU brand/name-string
	argument char *str: the char array to be filled with the GPU name
	argument bool error: true for verbose errors, false for silent
*/
void detect_gpu(char *str, bool error)
{
	FILE *gpu_file;

	gpu_file = popen("detectgpu 2>/dev/null", "r");
	fgets(str, MAX_STRLEN, gpu_file);
	pclose(gpu_file);

	return;
}

/*	detect_disk
	detects the computer's total disk capacity and usage
	argument char *str: the char array to be filled with the disk data in format '$G / $G ($G%)', where $ is a number
*/
void detect_disk(char *str, bool error)
{
	struct statvfs disk_info;
	unsigned long disk_total = 0, disk_used = 0, disk_percentage = 0;

	if (!(statvfs("/home", &disk_info)))
	{
		disk_total = ((disk_info.f_blocks * disk_info.f_bsize) / GB);
		disk_used = (((disk_info.f_blocks - disk_info.f_bfree) * disk_info.f_bsize) / GB);
		disk_percentage = (((float) disk_used / disk_total) * 100);
		snprintf(str, MAX_STRLEN, "%ldG / %ldG (%ld%%)", disk_used, disk_total, disk_percentage);
	}
	else if (error)
	{
		ERROR_OUT("Error: ", "Could not stat /home for filesystem statistics.");
	}

	return;
}

/*	detect_mem
	detects the computer's total and used RAM
	argument char *str: the char array to be filled with the memory data in format '$MB / $MB', where $ is a number
*/
void detect_mem(char *str)
{
	long long total_mem = 0;

	total_mem = sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGE_SIZE);
	total_mem /= MB;

	/* sar -r 1 to get free pages? */

	snprintf(str, MAX_STRLEN, "%lld%s", total_mem, "MB");

	return;
}

/*	detect_shell
	detects the shell currently running on the computer
	argument char *str: the char array to be filled with the shell name and version
	argument bool error: true for verbose errors, false for silent
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
	argument bool error: true for verbose errors, false for silent
*/
void detect_res(char *str, bool error)
{
	int width = 0;
	int height = 0;

	Display *disp;

	if ((disp = XOpenDisplay(NULL)))
	{
		Screen *screen = XDefaultScreenOfDisplay(disp);
		width = WidthOfScreen(screen);
		height = HeightOfScreen(screen);
		snprintf(str, MAX_STRLEN, "%dx%d", width, height);
	}
	else
	{
		safe_strncpy(str, "No X Server", MAX_STRLEN);

		if (error)
			ERROR_OUT("Error: ", "Problem detecting X display resolution.");
	}

	XCloseDisplay(disp);

	return;
}

/*	detect_de
	detects the desktop environment currently running on top of the OS
	argument char *str: the char array to be filled with the DE name
	--
	CAVEAT: This function relies on the presence of 'detectde', a shell script. 
	If it isn't present somewhere in the PATH, the WM Theme will be set as 'Unknown'
	--
*/
void detect_de(char *str)
{
	safe_strncpy(str, "Not implemented", MAX_STRLEN);

	return;
}

/*	detect_wm
	detects the window manager currently running on top of the OS
	argument char *str: the char array to be filled with the WM name
	--
	CAVEAT: This function relies on the presence of 'detectwm', a shell script. 
	If it isn't present somewhere in the PATH, the WM Theme will be set as 'Unknown'
	--
*/
void detect_wm(char *str, bool error)
{
	FILE *wm_file;

	wm_file = popen("detectwm 2> /dev/null", "r");
	fgets(str, MAX_STRLEN, wm_file);
	pclose(wm_file);

	return;
}

/*	detect_wm_theme
	detects the theme associated with the WM detected in detect_wm()
	argument char *str: the char array to be filled with the WM Theme name
	--
	CAVEAT: This function relies on the presence of 'detectwmtheme', a shell script. 
	If it isn't present somewhere in the PATH, the WM Theme will be set as 'Unknown'
	--
*/
void detect_wm_theme(char *str)
{
	FILE *wm_theme_file;

	wm_theme_file = popen("detectwmtheme 2> /dev/null", "r");
	fgets(str, MAX_STRLEN, wm_theme_file);
	pclose(wm_theme_file);

	return;
}

/*	detect_gtk
	detects the theme, icon(s), and font(s) associated with a GTK DE (if present)
	argument char *str: the char array to be filled with the GTK info
	--
	CAVEAT: This function relies on the presence of 'detectgtk', a shell script. 
	If it isn't present somewhere in the PATH, the WM Theme will be set as 'Unknown'
	--
*/
void detect_gtk(char *str)
{
	safe_strncpy(str, "Not implemented", MAX_STRLEN);

	return;
}
