/*	detect.c
 *	Author: William Woodruff
 *	-------------
 *
 *	The detection functions used by screenfetch-c on Windows (Cygwin) are implemented here.
 *	Like the rest of screenfetch-c, this file is licensed under the MIT license.
 */

/* standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

/* Windows-specific includes */
#include <Windows.h>
/* Cygwin wets itself without an explicit external linkage to popen */
extern FILE *popen(const char *command, const char *type);
extern int pclose(FILE *stream);

/* program includes */
#include "../../misc.h"
#include "../../disp.h"
#include "../../util.h"
#include "../../error_flag.h"

/*	detect_distro
	detects the computer's distribution (Windows version)
	argument char *str: the char array to be filled with the distro name
*/
void detect_distro(char *str)
{
	/* if distro_str was NOT set by the -D flag or manual mode */
	if (STREQ(str, "Unknown") || STREQ(str, "*"))
	{
#if defined(NTDDI_WIN7)
			safe_strncpy(str, "Microsoft Windows 7", MAX_STRLEN);
#elif defined(NTDDI_WIN8)
			safe_strncpy(str, "Microsoft Windows 8", MAX_STRLEN);
#elif defined(NTDDI_WINBLUE)
			safe_strncpy(str, "Microsoft Windows 8.1", MAX_STRLEN);
#elif defined(NTDDI_VISTA) || defined(NTDDI_VISTASP1)
			safe_strncpy(str, "Microsoft Windows Vista", MAX_STRLEN);
#elif defined(NTDDI_WINXP) || defined(NTDDI_WINXPSP1) || defined(NTDDI_WINXPSP2) || defined(NTDDI_WINXPSP3)
			safe_strncpy(str, "Microsoft Windows XP", MAX_STRLEN);
#elif defined(_WIN32_WINNT_WS03)
			safe_strncpy(str, "Microsoft Windows Server 2003", MAX_STRLEN);
#elif defined(_WIN32_WINNT_WS08)
			safe_strncpy(str, "Microsoft Windows Server 2008", MAX_STRLEN);
#else
			safe_strncpy(str, "Microsoft Windows", MAX_STRLEN);
#endif
	}

	return;
}

/*	detect_host
	detects the computer's hostname and active user and formats them
	argument char *str: the char array to be filled with the host info
*/
void detect_host(char *str)
{
	char *given_user = "Unknown";
	char given_host[MAX_STRLEN] = "Unknown";

	given_user = malloc(sizeof(char) * MAX_STRLEN);
	if (!given_user)
	{
		ERR_REPORT("Memory allocation failed in detect_host.");
		exit(1);
	}
	
	unsigned long len = MAX_STRLEN;
	GetUserName(given_user, &len);
	gethostname(given_host, MAX_STRLEN);

	snprintf(str, MAX_STRLEN, "%s@%s", given_user, given_host);

	free(given_user);

	return;
}

/*	detect_kernel
	detects the computer's kernel
	argument char *str: the char array to be filled with the kernel name
*/
void detect_kernel(char *str)
{
	OSVERSIONINFO kern_info;
	SYSTEM_INFO arch_info;
	char arch_str[MAX_STRLEN];

	ZeroMemory(&kern_info, sizeof(OSVERSIONINFO));
	kern_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&kern_info);

	GetNativeSystemInfo(&arch_info);

	if (arch_info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
	{
		safe_strncpy(arch_str, "AMD64", MAX_STRLEN);
	}
	else if (arch_info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM)
	{
		safe_strncpy(arch_str, "ARM", MAX_STRLEN);
	}
	else if (arch_info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
	{
		safe_strncpy(arch_str, "IA64", MAX_STRLEN);
	}
	else if (arch_info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
	{
		safe_strncpy(arch_str, "x86", MAX_STRLEN);
	}
	else
	{
		safe_strncpy(arch_str, "Unknown", MAX_STRLEN);
	}

	snprintf(str, MAX_STRLEN, "Windows NT %d.%d build %d (%s)",
			(int) kern_info.dwMajorVersion, (int) kern_info.dwMinorVersion,
			(int) kern_info.dwBuildNumber, arch_str);

	return;
}


/*	detect_uptime
	detects the computer's uptime
	argument char *str: the char array to be filled with the uptime
*/
void detect_uptime(char *str)
{
	long uptime = 0;
	int secs = 0;
	int mins = 0;
	int hrs = 0;
	int days = 0;

	uptime = GetTickCount(); /* known problem: will rollover after 49.7 days */
	uptime /= 1000;

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
void detect_pkgs(char *str, const char *distro_str)
{
	FILE *pkgs_file;
	int packages = 0;

	pkgs_file = popen("cygcheck -cd | wc -l", "r");
	fscanf(pkgs_file, "%d", &packages);
	packages -= 2;
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
	HKEY hkey;
	DWORD str_size = MAX_STRLEN;

	RegOpenKey(HKEY_LOCAL_MACHINE,
			"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", &hkey);
	RegQueryValueEx(hkey, "ProcessorNameString", 0, NULL, (BYTE *) str,
			&str_size);

	return;
}

/*	detect_gpu
	detects the computer's GPU brand/name-string
	argument char *str: the char array to be filled with the GPU name
*/
void detect_gpu(char *str)
{
	HKEY hkey;
	DWORD str_size = MAX_STRLEN;

	RegOpenKey(HKEY_LOCAL_MACHINE,
			"SYSTEM\\ControlSet001\\Control\\Class\\"
			"{4D36E968-E325-11CE-BFC1-08002BE10318}\\0000\\Settings", &hkey);
	RegQueryValueEx(hkey, "Device Description", 0, NULL, (BYTE *) str,
			&str_size);

	return;
}

/*	detect_disk
	detects the computer's total disk capacity and usage
	argument char *str: the char array to be filled with the disk data
*/
void detect_disk(char *str)
{
	FILE *disk_file;
	int disk_total = 0;
	int disk_used = 0;
	int disk_percentage = 0;

	/* GetDiskFreeSpaceEx? */

	disk_file = popen("df -H 2> /dev/null | grep -vE '^[A-Z]\\:\\/|File' | awk "
			"'{ print $2 }' | head -1 | tr -d '\\r\\n G'", "r");
	fscanf(disk_file, "%d", &disk_total);
	pclose(disk_file);

	disk_file = popen("df -H 2> /dev/null | grep -vE '^[A-Z]\\:\\/|File' | awk "
			"'{ print $3 }' | head -1 | tr -d '\\r\\n G'", "r");
	fscanf(disk_file, "%d", &disk_used);
	pclose(disk_file);

	if (disk_total > disk_used)
	{
		disk_percentage = (((float) disk_used / disk_total) * 100);

		snprintf(str, MAX_STRLEN, "%dG / %dG (%d%%)", disk_used, disk_total,
				disk_percentage);
	}
	else /* when disk_used is in a smaller unit */
	{
		disk_percentage = ((float) disk_used / (disk_total * 1024) * 100);

		snprintf(str, MAX_STRLEN, "%dM / %dG (%d%%)", disk_used, disk_total,
				disk_percentage);
	}

	return;
}

/*	detect_mem
	detects the computer's total and used RAM
	argument char *str: the char array to be filled with the memory data
*/
void detect_mem(char *str)
{
	long long total_mem = 0;
	long long used_mem = 0;
	MEMORYSTATUSEX mem_stat;

	mem_stat.dwLength = sizeof(mem_stat);
	GlobalMemoryStatusEx(&mem_stat);

	total_mem = (long long) mem_stat.ullTotalPhys / MB;
	used_mem = total_mem - ((long long) mem_stat.ullAvailPhys / MB);

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
void detect_shell(char *str)
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
		safe_strncpy(str, "POSIX sh", MAX_STRLEN);
	}
	else if (strstr(shell_name, "bash"))
	{
		shell_file = popen("bash --version | head -1", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);
		snprintf(str, MAX_STRLEN, "bash %.*s", 17, vers_str + 10);
		pclose(shell_file);
	}
	else if (strstr(shell_name, "zsh"))
	{
		shell_file = popen("zsh --version", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);	
		snprintf(str, MAX_STRLEN, "zsh %.*s", 5, vers_str + 4);
		pclose(shell_file);
	}
	else if (strstr(shell_name, "csh"))
	{
		shell_file = popen("csh --version | head -1", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);
		snprintf(str, MAX_STRLEN, "csh %.*s", 7, vers_str + 5);
		pclose(shell_file);
	}
	else if (strstr(shell_name, "fish"))
	{
		shell_file = popen("fish --version", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);
		snprintf(str, MAX_STRLEN, "fish %.*s", 13, vers_str + 6);
		pclose(shell_file);
	}
	else if (strstr(shell_name, "dash") || strstr(shell_name, "ash")
			|| strstr(shell_name, "ksh"))
	{
		/* i don't have a version detection system for these, yet */
		safe_strncpy(str, shell_name, MAX_STRLEN);
	}

	return;
}

/*	detect_res
	detects the combined resolution of all monitors attached to the computer
	argument char *str: the char array to be filled with the resolution
*/
void detect_res(char *str)
{
	int width = 0;
	int height = 0;

	width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	snprintf(str, MAX_STRLEN, "%dx%d", width, height);

	return;
}

/*	detect_de
	detects the desktop environment currently running on top of the OS.
	On Windows, this will always be either Luna, Aero, or Metro.
	argument char *str: the char array to be filled with the DE name
*/
void detect_de(char *str)
{
	FILE *de_file;
	int version;

	de_file = popen("wmic os get version | grep -o '^[0-9]'", "r");
	fscanf(de_file, "%d", &version);
	pclose(de_file);

	if (version == 6 || version == 7)
		safe_strncpy(str, "Aero", MAX_STRLEN);
	else if (version == 8)
		safe_strncpy(str, "Metro", MAX_STRLEN);
	else
		safe_strncpy(str, "Luna", MAX_STRLEN);

	return;
}

/*	detect_wm
	detects the window manager currently running on top of the OS.
	On Windows, this will always be DWM/Explorer.
	argument char *str: the char array to be filled with the WM name
*/
void detect_wm(char *str)
{
	safe_strncpy(str, "DWM/Explorer", MAX_STRLEN);

	return;
}

/*	detect_wm_theme
	On Windows, detects the current theme running on DWM.
	argument char *str: the char array to be filled with the WM Theme name
*/
void detect_wm_theme(char *str, const char *wm_str)
{
	FILE *wm_theme_file;

	/* nasty one-liner */
	wm_theme_file = popen("reg query 'HKCU\\Software\\Microsoft\\Windows"
			"\\CurrentVersion\\Themes' /v 'CurrentTheme' | "
			"grep -o '[A-Z]:\\\\.*' | awk -F\"\\\\\" '{print $NF}' | "
			"grep -o '[0-9A-z. ]*$' | grep -o '^[0-9A-z ]*' | tr -d '\\r\\n'",
			"r");
	fgets(str, MAX_STRLEN, wm_theme_file);
	pclose(wm_theme_file);

	return;
}

/*	detect_gtk
	On Windows, detects the font associated with Cygwin's terminal (mintty)
	argument char *str: the char array to be filled with the GTK info
*/
void detect_gtk(char *str)
{
	FILE *gtk_file;
	char font_str[MAX_STRLEN] = "Unknown";

	gtk_file = popen("grep '^Font=.*' < $HOME/.minttyrc | "
			"grep -o '[0-9A-z ]*$' | tr -d '\\r\\n'", "r");
	fgets(font_str, MAX_STRLEN, gtk_file);
	pclose(gtk_file);

	snprintf(str, MAX_STRLEN, "%s (Font)", font_str);

	return;
}
