/* standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

/* linux-specific includes */
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <glob.h>

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
		FILE *distro_file;

		char distro_name_str[MAX_STRLEN];

		if (FILE_EXISTS("/system/bin/getprop"))
		{
			safe_strncpy(str, "Android", MAX_STRLEN);
		}
		else
		{
			bool detected = false;

			/* Note: this is a very bad solution, as /etc/issue contains junk on some distros */
			distro_file = fopen("/etc/issue", "r");

			if (distro_file != NULL)
			{
				/* get the first 4 chars, that's all we need */
				fscanf(distro_file, "%4s", distro_name_str);
				fclose(distro_file);

				if (STRCMP(distro_name_str, "Back"))
				{
					safe_strncpy(str, "Backtrack Linux", MAX_STRLEN);
					detected = true;
				}
				else if (STRCMP(distro_name_str, "Crun"))
				{
					safe_strncpy(str, "CrunchBang", MAX_STRLEN);
					detected = true;
				}
				else if (STRCMP(distro_name_str, "LMDE"))
				{
					safe_strncpy(str, "LMDE", MAX_STRLEN);
					detected = true;
				}
				else if (STRCMP(distro_name_str, "Debi") || STRCMP(distro_name_str, "Rasp"))
				{
					safe_strncpy(str, "Debian", MAX_STRLEN);
					detected = true;
				}
			}

			if (!detected)
			{
				if (FILE_EXISTS("/etc/fedora-release"))
				{
					safe_strncpy(str, "Fedora", MAX_STRLEN);
				}
				else if (FILE_EXISTS("/etc/SuSE-release"))
				{
					safe_strncpy(str, "OpenSUSE", MAX_STRLEN);
				}
				else if (FILE_EXISTS("/etc/arch-release"))
				{
					safe_strncpy(str, "Arch Linux", MAX_STRLEN);
				}
				else if (FILE_EXISTS("/etc/gentoo-release"))
				{
					safe_strncpy(str, "Gentoo", MAX_STRLEN);
				}
				else if (FILE_EXISTS("/etc/angstrom-version"))
				{
					safe_strncpy(str, "Angstrom", MAX_STRLEN);
				}
				else if (FILE_EXISTS("/etc/manjaro-release"))
				{
					safe_strncpy(str, "Manjaro", MAX_STRLEN);
				}
				else if (FILE_EXISTS("/etc/lsb-release"))
				{
					distro_file = fopen("/etc/lsb-release", "r");
					fgets(distro_name_str, MAX_STRLEN, distro_file);
					distro_name_str[strlen(distro_name_str) - 1] = '\0';
					fclose(distro_file);

					snprintf(str, MAX_STRLEN, "%s", distro_name_str + 11);
				}
				else
				{
					safe_strncpy(str, "Linux", MAX_STRLEN);

					if (error)
					{
						ERROR_OUT("Error: ", "Failed to detect specific Linux distro.");
					}
				}
			}
		}

		// else if (ISBSD() || OS == SOLARIS)
		// {
		// 	distro_file = popen("uname -sr | tr -d '\\n'", "r");
		// 	fgets(str, MAX_STRLEN, distro_file);
		// 	pclose(distro_file);
		// }
	}

	return;
}