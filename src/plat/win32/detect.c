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