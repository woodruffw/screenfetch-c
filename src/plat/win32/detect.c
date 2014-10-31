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

/*	detect_distro
	detects the computer's distribution (really only relevant on Linux)
	argument char *str: the char array to be filled with the distro name
*/
void detect_distro(char *str, bool error)
{
	if (STRCMP(str, "Unknown") || STRCMP(str, "*")) /* if distro_str was NOT set by the -D flag or manual mode */
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

/*	detect_arch
	detects the computer's architecture
	argument char *str: the char array to be filled with the architecture
*/
void detect_arch(char *str)
{
	SYSTEM_INFO arch_info;
	GetNativeSystemInfo(&arch_info);

	if (arch_info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
	{
		safe_strncpy(str, "AMD64", MAX_STRLEN);
	}
	else if (arch_info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM)
	{
		safe_strncpy(str, "ARM", MAX_STRLEN);
	}
	else if (arch_info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
	{
		safe_strncpy(str, "IA64", MAX_STRLEN);
	}
	else if (arch_info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
	{
		safe_strncpy(str, "x86", MAX_STRLEN);
	}
	else
	{
		safe_strncpy(str, "Unknown", MAX_STRLEN);
	}

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

	given_user = malloc(sizeof(char) * MAX_STRLEN);
	if (given_user == NULL)
	{
		ERROR_OUT("Error: ", "Failed to allocate sufficient memory in detect_host.");
		exit(1);
	}
	/* why does the winapi require a pointer to a long? */
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
	ZeroMemory(&kern_info, sizeof(OSVERSIONINFO));
	kern_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&kern_info);
	snprintf(str, MAX_STRLEN, "Windows NT %d.%d build %d", (int) kern_info.dwMajorVersion, (int) kern_info.dwMinorVersion, (int) kern_info.dwBuildNumber);

	return;
}


/*	detect_uptime
	detects the computer's uptime
	argument char *str: the char array to be filled with the uptime in format '$d $h $m $s' where $ is a number
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

/*	detect_cpu
	detects the computer's CPU brand/name-string
	argument char *str: the char array to be filled with the CPU name
*/
void detect_cpu(char *str)
{
	HKEY hkey;
	DWORD str_size = MAX_STRLEN;
	RegOpenKey(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", &hkey);
	RegQueryValueEx(hkey, "ProcessorNameString", 0, NULL, (BYTE *) str, &str_size);

	return;
}

/*	detect_gpu
	detects the computer's GPU brand/name-string
	argument char *str: the char array to be filled with the GPU name
*/
void detect_gpu(char *str, bool error)
{
	HKEY hkey;
	DWORD str_size = MAX_STRLEN;
	RegOpenKey(HKEY_LOCAL_MACHINE, "SYSTEM\\ControlSet001\\Control\\Class\\{4D36E968-E325-11CE-BFC1-08002BE10318}\\0000\\Settings", &hkey);
	RegQueryValueEx(hkey, "Device Description", 0, NULL, (BYTE *) str, &str_size);

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

	/* Cygwin -- GetDiskFreeSpaceEx? */

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