/* 	screenfetch-c.c
	Author: William Woodruff
	Source Version: 1.O BETA
	-------------

	A rewrite of screenFetch.sh 3.0.5 in C.
	This is primarily an experiment borne out of an awareness of the slow execution time on the 
	screenfetch-dev.sh script. 
	Hopefully this port will execute faster, although it's more for self education than anything else.
	
	------

	NOTES:
	I used many of Brett Bohnenkamper's awk/sed/grep/etc oneliners in my popen() calls, 
	although some were modified to change/improve the output.

	The shell scripts detectwm.sh, detectwmtheme.sh, and detectde.sh are all partially or completely based upon
	screenfetch-dev. They are called within the program, as detecting the WM/WM theme/DE within C would be a pain in the ass.

	Credit goes to shrx and Hu6 for many of the oneliners used in screenfetch-c's OS X popen() calls.
	The ASCII artwork used in screenfetch-c also comes directly from screenFetch, albiet with changes in color format.

	PLANNED IMPROVEMENTS:
	Add libcpuid to decrease reliance on shell utilities.
	Streamline code, make C89 (ANSI) compatible.

	TODO:
	Figure out DE/WM/WM theme/GTK detection
	Fully implement detect_distro() on Linux.
	Fix issues with RAM usage detection on OS X (values slightly inaccurate)

	------

	**From the original author:**

	------

	Copyright (c) 2010-2012 Brett Bohnenkamper < kittykatt AT archlinux DOT us >

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software
	and associated documentation files (the "Software"), to deal in the Software without restriction,
	including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
	and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
	subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
	WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
	COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
	ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

	Yes, I do realize some of this is horribly ugly coding. Any ideas/suggestions would be
	appreciated by emailing me or by stopping by http://github.com/KittyKatt/screenFetch . You
	could also drop in on my IRC network, SilverIRC, at irc://kittykatt.silverirc.com:6667/screenFetch
	to put forth suggestions/ideas. Thank you.

	------

	I hereby regrant this version of screenFetch under the same MIT license.
	If you have any questions, please contact me at woodrufw@bxscience.edu or on github (http://www.github.com/woodrufw/screenfetch-c)
*/

#define _XOPEN_SOURCE 700 /* ensures that popen and pclose are available in C99 on Linux */

#include <stdio.h> /* for a medley of I/O operations, including popen/pclose */
#include <stdlib.h> /* for getenv, etc */
#include <stdbool.h> /* for the bool type */
#include <string.h> /* for strcmp, strncpy, etc. */
#include <unistd.h> /* for sleep, getopt */
#include "screenfetch-c.h" /* contains function prototypes, macros, ascii logos */

/* string definitions */
static char distro_str[MAX_STRLEN];
static char arch_str[MAX_STRLEN];
static char host_str[MAX_STRLEN];
static char kernel_str[MAX_STRLEN];
static char uptime_str[MAX_STRLEN];
static char pkgs_str[MAX_STRLEN];
static char cpu_str[MAX_STRLEN];
static char gpu_str[MAX_STRLEN];
static char disk_str[MAX_STRLEN];
static char mem_str[MAX_STRLEN];
static char shell_str[MAX_STRLEN];
static char res_str[MAX_STRLEN];
static char de_str[MAX_STRLEN];
static char wm_str[MAX_STRLEN];
static char wm_theme_str[MAX_STRLEN];
static char gtk_str[MAX_STRLEN];

/* output string definitions */
static char* detected_arr[16];
static char* detected_arr_names[16] = {"", "OS: ", "Kernel: ", "Arch: ", "CPU: ", "GPU: ", "Shell: ", "Packages: ", "Disk: ", "Memory: ", "Uptime: ", "Resolution: ", "DE: ", "WM: ", "WM Theme: ", "GTK: "};

/* other definitions */
bool logo = true;
bool debug = false;
bool error = true;
bool verbose = false;
bool screenshot = false;
bool ascii = false;

int main(int argc, char** argv)
{
	/* first off, don't allow unknown OSes to run this program */
	if (OS == UNKNOWN)
	{
		ERROR_OUT("Error: ", "This program isn't designed for your OS.");
		ERROR_OUT("Even if it did compile successfully, it will not execute correctly.", "");

		return EXIT_FAILURE;
	}

	/* copy 'Unknown' to each string and append a null character */
	safe_strncpy(distro_str, "Unknown", MAX_STRLEN);
	safe_strncpy(arch_str, "Unknown", MAX_STRLEN);
	safe_strncpy(host_str, "Unknown", MAX_STRLEN);
	safe_strncpy(kernel_str, "Unknown", MAX_STRLEN);
	safe_strncpy(uptime_str, "Unknown", MAX_STRLEN);
	safe_strncpy(pkgs_str, "Unknown", MAX_STRLEN);
	safe_strncpy(cpu_str, "Unknown", MAX_STRLEN);
	safe_strncpy(gpu_str, "Unknown", MAX_STRLEN);
	safe_strncpy(disk_str, "Unknown", MAX_STRLEN);
	safe_strncpy(mem_str, "Unknown", MAX_STRLEN);
	safe_strncpy(shell_str, "Unknown", MAX_STRLEN);
	safe_strncpy(res_str, "Unknown", MAX_STRLEN);
	safe_strncpy(de_str, "Unknown", MAX_STRLEN);
	safe_strncpy(wm_str, "Unknown", MAX_STRLEN);
	safe_strncpy(wm_theme_str, "Unknown", MAX_STRLEN);
	safe_strncpy(gtk_str, "Unknown", MAX_STRLEN);

	char c; 

	while ((c = getopt(argc, argv, "dvnNsS:D:A:EVh")) != -1)
	{
		switch (c)
		{
			case 'd':
				SET_DEBUG(true);
				break;
			case 'v':
				SET_VERBOSE(true);
				break;
			case 'n':
				SET_LOGO(false);
				break;
			case 'N':
				/* something like SET_COLOR(false); */
				break;
			case 's':
				SET_SCREENSHOT(true);
				break;
			case 'S':
				/*SET_SCREENSHOT(true);*/
				break;
			case 'D':
				SET_DISTRO(optarg);
				break;
			case 'A':
				/* something like SET_DISTRO_ART(optarg); */
				break;
			case 'E':
				SET_ERROR(false);
				break;
			case 'V':
				display_version();
				return EXIT_SUCCESS;
			case 'h':
				display_help();
				return EXIT_SUCCESS;
			case '?':
				if (optopt == 'S' || optopt == 'D' || optopt == 'A')
					ERROR_OUT("Error: ", "One or more tripped flag(s) requires an argument.");
				else
					ERROR_OUT("Error: ", "Unknown option or option character.");
				return EXIT_FAILURE;
		}
	}

	/* each string is filled by its respective function */
	detect_distro(distro_str);
	detect_arch(arch_str);
	detect_host(host_str);
	detect_kernel(kernel_str);
	detect_uptime(uptime_str);
	detect_pkgs(pkgs_str);
	detect_cpu(cpu_str);
	detect_gpu(gpu_str);
	detect_disk(disk_str);
	detect_mem(mem_str);
	detect_shell(shell_str);
	detect_res(res_str);
	detect_de(de_str);
	detect_wm(wm_str);
	detect_wm_theme(wm_theme_str);
	detect_gtk(gtk_str);

	/* debug section - only executed if -d flag is tripped */
	if (debug)
	{
		if (STRCMP(distro_str, "Unknown"))
			DEBUG_OUT("Distro detection failure: ", distro_str);

		if (STRCMP(arch_str, "Unknown"))
			DEBUG_OUT("Architecture detection failure: ", arch_str);

		if (STRCMP(host_str, "Unknown"))
			DEBUG_OUT("Host detection failure: ", host_str);

		if (STRCMP(kernel_str, "Unknown"))
			DEBUG_OUT("Kernel detection failure: ", kernel_str);

		if (STRCMP(uptime_str, "Unknown"))
			DEBUG_OUT("Uptime detection failure: ", uptime_str);

		if (STRCMP(pkgs_str, "Unknown"))
			DEBUG_OUT("Package detection failure: ", pkgs_str);

		if (STRCMP(cpu_str, "Unknown"))
			DEBUG_OUT("CPU detection failure: ", cpu_str);

		if (STRCMP(gpu_str, "Unknown"))
			DEBUG_OUT("GPU detection failure: ", gpu_str);

		if (STRCMP(disk_str, "Unknown"))
			DEBUG_OUT("Disk detection failure: ", disk_str);

		if (STRCMP(mem_str, "Unknown"))
			DEBUG_OUT("Memory detection failure: ", mem_str);

		if (STRCMP(shell_str, "Unknown"))
			DEBUG_OUT("Shell detection failure: ", shell_str);

		if (STRCMP(res_str, "Unknown"))
			DEBUG_OUT("Resolution detection failure: ", res_str);

		if (STRCMP(de_str, "Unknown"))
			DEBUG_OUT("DE detection failure: ", de_str);

		if (STRCMP(wm_str, "Unknown"))
			DEBUG_OUT("WM detection failure: ", wm_str);

		if (STRCMP(wm_theme_str, "Unknown"))
			DEBUG_OUT("WM Theme detection failure: ", wm_theme_str);

		if (STRCMP(gtk_str, "Unknown"))
			DEBUG_OUT("GTK detection failure: ", gtk_str);

		printf("%s\n", "Enter any character to end the program.");
		getchar();

		return EXIT_SUCCESS;
	}

	/* detected_arr is filled with the gathered from the detection functions */
	fill_detected_arr(detected_arr, distro_str, arch_str, host_str, kernel_str, uptime_str, pkgs_str, cpu_str, gpu_str, disk_str, mem_str, shell_str, res_str, de_str, wm_str, wm_theme_str, gtk_str);

	if (logo)
		main_ascii_output(detected_arr, detected_arr_names);
	else
		main_text_output(detected_arr, detected_arr_names);

	if (screenshot)
		take_screenshot();

	return EXIT_SUCCESS;
}

/*  **  BEGIN DETECTION FUNCTIONS  ** */

/*  detect_distro
    detects the computer's distribution (really only relevant on Linux)
    argument char* str: the char array to be filled with the distro name
*/
void detect_distro(char* str)
{
	if (STRCMP(str, "Unknown")) //if distro_str was NOT set by the -D flag
	{
		FILE* distro_file;

		char distro_name_str[MAX_STRLEN];

		if (OS == CYGWIN)
		{
			/* alternative solution (3.5s execution time! uh oh!):
			   systeminfo | grep 'OS Name:' | cut -d ':' -f 2 | sed 's/^ * //g
			*/
			distro_file = popen("wmic os get name | head -2 | tail -1", "r");
			fgets(distro_name_str, MAX_STRLEN, distro_file);
			pclose(distro_file);

			/* currently only works on W7, working on a solution */
			snprintf(str, MAX_STRLEN, "%.*s", 19, distro_name_str);
		}

		else if (OS == OSX)
		{
			distro_file = popen("sw_vers | grep ProductVersion | tr -d 'ProductVersion: \\t\\n'", "r");
			fgets(distro_name_str, MAX_STRLEN, distro_file);
			pclose(distro_file);

			snprintf(str, MAX_STRLEN, "%s%s", "Mac OS X ", distro_name_str);
		}

		else if (OS == LINUX)
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

				else if (STRCMP(distro_name_str, "Debi"))
				{
					safe_strncpy(str, "Debian", MAX_STRLEN);
					detected = true;
				}
			}

			if (!detected)
			{
				distro_file = fopen("/etc/lsb-release", "r");

				if (distro_file != NULL)
				{
					fclose(distro_file);

					distro_file = popen("cat /etc/lsb-release | head -1 | tr -d \"\\\"\\n\"", "r");
					fgets(distro_name_str, MAX_STRLEN, distro_file);
					pclose(distro_file);

					snprintf(str, MAX_STRLEN, "%s", distro_name_str + 11);
				}

				else /* begin the tedious task of checking each /etc/<distro>-release */
				{
					distro_file = fopen("/etc/fedora-release", "r");

					if (distro_file != NULL)
					{
						fclose(distro_file);
						safe_strncpy(str, "Fedora", MAX_STRLEN);
					}
			
					else
					{
						distro_file = fopen("/etc/SuSE-release", "r");

						if (distro_file != NULL)
						{
							fclose(distro_file);
							safe_strncpy(str, "OpenSUSE", MAX_STRLEN);
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
			}
		}

		else if (ISBSD())
		{
			distro_file = popen("uname -sr", "r");
			fgets(str, sizeof(str), distro_file);
			pclose(distro_file);
		}
	}

	if (verbose)
		VERBOSE_OUT("Found distro as ", str);

	return;
}

/*  detect_arch
    detects the computer's architecture
    argument char* str: the char array to be filled with the architecture
*/
void detect_arch(char* str)
{
	FILE* arch_file;

	if (OS == CYGWIN)
	{
		arch_file = popen("wmic os get OSArchitecture | head -2 | tail -1 | tr -d '\\r\\n '", "r");
		fgets(str, MAX_STRLEN, arch_file);
		pclose(arch_file);
	}

	else if (OS == OSX || OS == LINUX || ISBSD())
	{
		arch_file = popen("uname -m | tr -d '\\n'", "r");
		fgets(str, MAX_STRLEN, arch_file);
		pclose(arch_file);
	}

	if (verbose)
		VERBOSE_OUT("Found system arch as ", str);

	return;
}

/*  detect_host
    detects the computer's hostname and active user and formats them
    argument char* str: the char array to be filled with the user and hostname in format 'user@host'
    --
    CAVEAT: acceses an environment variable
    --
*/
void detect_host(char* str)
{
	char* given_user; /* has to be a pointer for getenv(), god knows why */
	char given_host[MAX_STRLEN];

	given_user = getenv("USER");

	FILE* host_file = popen("hostname | tr -d '\\r\\n '", "r");
	fgets(given_host, MAX_STRLEN, host_file);
	pclose(host_file);

	/* format str */
	snprintf(str, MAX_STRLEN, "%s@%s", given_user, given_host);

	if (verbose)
		VERBOSE_OUT("Found host as ", str);

	return;
}

/*  detect_kernel
    detects the computer's kernel
    argument char* str: the char array to be filled with the kernel name
*/
void detect_kernel(char* str)
{
	FILE* kernel_file = popen("uname -sr | tr -d '\\r\\n'", "r");
	fgets(str, MAX_STRLEN, kernel_file);
	pclose(kernel_file);

	if (verbose)
		VERBOSE_OUT("Found kenel as ", str);

	return;
}

/*  detect_uptime
    detects the computer's uptime
    argument char* str: the char array to be filled with the uptime in format '$d $h $m $s' where $ is a number
*/
void detect_uptime(char* str)
{
	FILE* uptime_file;

	long uptime;
	long now, boottime; /* may or may not be used depending on OS */
	int secs;
	int mins;
	int hrs;
	int days;

	if (OS == CYGWIN || OS == NETBSD)
	{
		uptime_file = popen("cat /proc/uptime | cut -d ' ' -f 1", "r");
		fscanf(uptime_file, "%ld", &uptime);
		pclose(uptime_file);
	}

	else if (OS == OSX || OS == FREEBSD || OS == DFBSD)
	{
		uptime_file = popen("sysctl -n kern.boottime | cut -d \"=\" -f 2 | cut -d \",\" -f 1", "r");
		fscanf(uptime_file, "%ld", &boottime); /* get boottime in secs */
		pclose(uptime_file);

		uptime_file = popen("date +%s", "r");
		fscanf(uptime_file, "%ld", &now); /* get current time in secs */
		pclose(uptime_file);

		uptime = now - boottime;
	}

	else if (OS == LINUX)
	{
		#ifdef __linux__
			struct sysinfo si_upt;
			sysinfo(&si_upt);

			uptime = si_upt.uptime;
		#endif
	}

	else if (OS == OPENBSD)
	{
		uptime_file = popen("sysctl -n kern.boottime", "r");
		fscanf(uptime_file, "%ld", &boottime); /* get boottime in secs */
		pclose(uptime_file);

		uptime_file = popen("date +%s", "r");
		fscanf(uptime_file, "%ld", &now); /* get current time in secs */
		pclose(uptime_file);

		uptime = now - boottime;
	}

	split_uptime(uptime, &secs, &mins, &hrs, &days);

	if (days > 0)
		snprintf(str, MAX_STRLEN, "%dd %dh %dm %ds", days, hrs, mins, secs);
	else
		snprintf(str, MAX_STRLEN, "%dh %dm %ds", hrs, mins, secs);

	if (verbose)
		VERBOSE_OUT("Found uptime as ", str);

	return;
}

/*  detect_pkgs
    detects the number of packages installed on the computer
    argument char* str: the char array to be filled with the number of packages
*/
void detect_pkgs(char* str)
{
	FILE* pkgs_file;

	int packages = 0;

	if (OS == CYGWIN)
	{
		pkgs_file = popen("cygcheck -cd | wc -l", "r");
		fscanf(pkgs_file, "%d", &packages);
		packages -= 2;
		pclose(pkgs_file);

		snprintf(str, MAX_STRLEN, "%d", packages);
	}

	else if (OS == OSX)
	{
		pkgs_file = popen("ls /usr/local/bin 2> /dev/null | wc -w", "r");
		fscanf(pkgs_file, "%d", &packages);
		pclose(pkgs_file);

		pkgs_file = fopen("/usr/local/bin/brew", "r"); /* test for existence of homebrew */

		if (pkgs_file != NULL)
		{
			fclose(pkgs_file);

			int brew_pkgs = 0;
			pkgs_file = popen("brew list -1 | wc -l", "r");
			fscanf(pkgs_file, "%d", &brew_pkgs);
			pclose(pkgs_file);

			packages += brew_pkgs; 
		}

		/* test for existence of macports, fink, etc here */
	}

	else if (OS == LINUX)
	{
		if (STRCMP(distro_str, "Arch Linux") || STRCMP(distro_str, "ParabolaGNU/Linux-libre") || STRCMP(distro_str, "Chakra") || STRCMP(distro_str, "Manjaro"))
		{
			pkgs_file = popen("pacman -Qq | wc -l", "r");
			fscanf(pkgs_file, "%d", &packages);
			pclose(pkgs_file);
		}

		else if (STRCMP(distro_str, "Frugalware"))
		{
			pkgs_file = popen("pacman-g2 -Q | wc -l", "r");
			fscanf(pkgs_file, "%d", &packages);
			pclose(pkgs_file);
		}

		else if (STRCMP(distro_str, "Ubuntu") || STRCMP(distro_str, "LinuxMint") || STRCMP(distro_str, "SolusOS") || STRCMP(distro_str, "Debian") || STRCMP(distro_str, "LMDE") || STRCMP(distro_str, "CrunchBang") || STRCMP(distro_str, "Peppermint") || STRCMP(distro_str, "LinuxDeepin") || STRCMP(distro_str, "Trisquel") || STRCMP(distro_str, "elementary OS") || STRCMP(distro_str, "Backtrack Linux"))
		{
			pkgs_file = popen("dpkg --get-selections | wc -l", "r");
			fscanf(pkgs_file, "%d", &packages);
			pclose(pkgs_file);
		}

		else if (STRCMP(distro_str, "Slackware"))
		{
			pkgs_file = popen("ls -l /var/log/packages | wc -l", "r");
			fscanf(pkgs_file, "%d", &packages);
			pclose(pkgs_file);
		}

		else if (STRCMP(distro_str, "Gentoo") || STRCMP(distro_str, "Sabayon") || STRCMP(distro_str, "Funtoo"))
		{
			pkgs_file = popen("ls -d /var/db/pkg/*/* | wc -l", "r");
			fscanf(pkgs_file, "%d", &packages);
			pclose(pkgs_file);
		}

		else if (STRCMP(distro_str, "Fuduntu") || STRCMP(distro_str, "Fedora") || STRCMP(distro_str, "OpenSUSE") || STRCMP(distro_str, "Red Hat Linux") || STRCMP(distro_str, "Mandriva") || STRCMP(distro_str, "Mandrake") || STRCMP(distro_str, "Mageia") || STRCMP(distro_str, "Viperr"))
		{
			pkgs_file = popen("rpm -qa | wc -l", "r");
			fscanf(pkgs_file, "%d", &packages);
			pclose(pkgs_file);
		}

		/* if linux disto detection failed */
		else if (STRCMP(distro_str, "Linux"))
		{
			safe_strncpy(str, "Not Found", MAX_STRLEN);
			if (error)
				ERROR_OUT("Error: ", "Packages cannot be detected on an unknown Linux distro.");
		}
	}

	else if (OS == FREEBSD || OS == OPENBSD)
	{
		pkgs_file = popen("pkg_info | wc -l | awk '{sub(\" \", \"\");print $1}'", "r");

		pclose(pkgs_file);
	}

	else if (OS == NETBSD || OS == DFBSD)
	{
		safe_strncpy(str, "Not Found", MAX_STRLEN);
		ERROR_OUT("Error: ", "Could not find packages on current OS.");
	}

	if (verbose)
		VERBOSE_OUT("Found package count as ", str);

	snprintf(str, MAX_STRLEN, "%d", packages);

	return;
}

/*  detect_cpu
    detects the computer's CPU brand/name-string
    argument char* str: the char array to be filled with the CPU name
*/
void detect_cpu(char* str)
{
	FILE* cpu_file;

	if (OS == CYGWIN)
	{
		cpu_file = popen("wmic cpu get name | tail -2 | tr -d '\\r\\n'", "r");
		fgets(str, MAX_STRLEN, cpu_file);
		pclose(cpu_file);
	}

	else if (OS == OSX)
	{
		cpu_file = popen("sysctl -n machdep.cpu.brand_string | sed 's/(\\([Tt][Mm]\\))//g;s/(\\([Rr]\\))//g;s/^//g' | tr -d '\\n'", "r");
		fgets(str, MAX_STRLEN, cpu_file);
		pclose(cpu_file);
	}

	else if (OS == LINUX || OS == NETBSD)
	{
		cpu_file = popen("awk 'BEGIN{FS=\":\"} /model name/ { print $2; exit }' /proc/cpuinfo | sed -e 's/ @/\\n/' -e 's/^ *//g' -e 's/ *$//g' | head -1 | tr -d '\\n'", "r");
		fgets(str, MAX_STRLEN, cpu_file);
		pclose(cpu_file);
	}

	else if (OS == DFBSD || OS == FREEBSD || OS == OPENBSD)
	{
		cpu_file = popen("sysctl -n hw.model", "r");
		fgets(str, MAX_STRLEN, cpu_file);
		pclose(cpu_file);
	}

	if (verbose)
		VERBOSE_OUT("Found CPU as ", str);

	return;
}

/*  detect_gpu
    detects the computer's GPU brand/name-string
    argument char* str: the char array to be filled with the GPU name
*/
void detect_gpu(char* str)
{
	FILE* gpu_file;

	if (OS == CYGWIN)
	{
		gpu_file = popen("wmic path Win32_VideoController get caption | tail -2 | tr -d '\\r\\n'", "r");
		fgets(str, MAX_STRLEN, gpu_file);
		pclose(gpu_file);
	}

	else if (OS == OSX)
	{
		gpu_file = popen("system_profiler SPDisplaysDataType | awk -F': ' '/^\\ *Chipset Model:/ {print $2}' | tr -d '\\n'", "r");
		fgets(str, MAX_STRLEN, gpu_file);
		pclose(gpu_file);
	}

	else if (OS == LINUX || ISBSD())
	{
		/* is probably going to use lspci or glxinfo  */
	}

	if (verbose)
		VERBOSE_OUT("Found GPU as ", str);

	return;
}

/*  detect_disk
    detects the computer's total HDD/SSD capacity and usage
    argument char* str: the char array to be filled with the disk data in format '$G / $G ($G%)', where $ is a number
*/
void detect_disk(char* str)
{
	FILE* disk_file;

	int disk_total;
	int disk_used;
	int disk_percentage;

	if (OS == CYGWIN || OS == LINUX || OS == OSX)
	{
		disk_file = popen("df -H | grep -vE '^[A-Z]\\:\\/|File' | awk '{ print $2 }' | head -1 | tr -d '\\r\\n G'", "r");
		fscanf(disk_file, "%d", &disk_total);
		pclose(disk_file);

		if (OS != CYGWIN)
		{
			disk_file = popen("df -H | grep -vE '^[A-Z]\\:\\/|File' | awk '{ print $3 }' | head -1 | tr -d '\\r\\n G'", "r");
			fscanf(disk_file, "%d", &disk_used);
			pclose(disk_file);
		}
		else
		{
			disk_file = popen("df -H | grep -vE '^[A-Z]\\:\\/|File' | awk '{ print $4 }' | head -1 | tr -d '\\r\\n G'", "r");
			fscanf(disk_file, "%d", &disk_used);
			pclose(disk_file);
		}
	}

	else if (ISBSD())
	{
		disk_file = popen("df -h | grep -vE '^[A-Z]\\:\\/|File' | awk '{ print $2 }' | head -1 | tr -d '\\r\\n G'", "r");
		fscanf(disk_file, "%d", &disk_total);
		pclose(disk_file);

		disk_file = popen("df -h | grep -vE '^[A-Z]\\:\\/|File' | awk '{ print $3 }' | head -1 | tr -d '\\r\\n G'", "r");
		fscanf(disk_file, "%d", &disk_used);
		pclose(disk_file);
	}

	/* ugly casting */
	disk_percentage = (int) (((float) disk_used / disk_total) * 100);

	snprintf(str, MAX_STRLEN, "%dG / %dG (%d%%)", disk_used, disk_total, disk_percentage);

	if (verbose)
		VERBOSE_OUT("Found disk usage as ", str);

	return;
}

/*  detect_mem
    detects the computer's total and used RAM
    argument char* str: the char array to be filled with the memory data in format '$MB / $MB', where $ is a number
*/
void detect_mem(char* str)
{
	FILE* mem_file;

	long kb = 1024;
	long mb = kb * kb;
	long total_mem; /* each of the following MAY contain bytes/kbytes/mbytes/pages */
	long free_mem; 
	long used_mem;

	if (OS == CYGWIN || OS == NETBSD)
	{
		mem_file = popen("awk '/MemTotal/ { print $2 }' /proc/meminfo", "r");
		fscanf(mem_file, "%ld", &total_mem);
		pclose(mem_file);

		mem_file = popen("awk '/MemFree/ { print $2 }' /proc/meminfo", "r");
		fscanf(mem_file, "%ld", &free_mem);
		pclose(mem_file);

		total_mem /= (long) kb;
		free_mem /= (long) kb;
		used_mem = total_mem - free_mem;
	}

	else if (OS == OSX)
	{
		mem_file = popen("sysctl -n hw.memsize", "r");
		fscanf(mem_file, "%ld", &total_mem);
		pclose(mem_file);

		mem_file = popen("vm_stat | head -2 | tail -1 | tr -d \"Pages free: .\"", "r");
		fscanf(mem_file, "%ld", &free_mem);
		pclose(mem_file);

		total_mem /= (long) mb;

		free_mem *= 4096; /* 4KiB is OS X's page size */
		free_mem /= (long) mb;

		used_mem = total_mem - free_mem;
	}

	else if (OS == LINUX)
	{
		#ifdef __linux__
			struct sysinfo si_mem;
			sysinfo(&si_mem);

			total_mem = (long) si_mem.totalram / mb;
			free_mem = (long) si_mem.freeram / mb;
			used_mem = (long) total_mem - free_mem;
		#endif
	}

	else if (OS == FREEBSD)
	{
		/* it's unknown if FreeBSD's /proc/meminfo is in the same format as Cygwin's */

		mem_file = popen("awk '/MemTotal/ { print $2 }' /proc/meminfo", "r");
		fscanf(mem_file, "%ld", &total_mem);
		pclose(mem_file);

		mem_file = popen("awk '/MemFree/ { print $2 }' /proc/meminfo", "r");
		fscanf(mem_file, "%ld", &free_mem);
		pclose(mem_file);

		total_mem /= (long) kb;
		free_mem /= (long) kb;
		used_mem = total_mem - free_mem;
	}

	else if (OS == OPENBSD)
	{
		mem_file = popen("top -1 1 | awk '/Real:/ {k=split($3,a,\"/\");print a[k] }' | tr -d 'M'", "r");
		fscanf(mem_file, "%ld", &total_mem);
		pclose(mem_file);

		mem_file = popen("top -1 1 | awk '/Real:/ {print $3}' | sed 's/M.*//'", "r");
		fscanf(mem_file, "%ld", &used_mem);
		pclose(mem_file);
	}

	else if (OS == DFBSD)
	{
		/* i currently don't know of any way to detected used memory in DFBSD */

		mem_file = popen("sysctl -n hw.physmem", "r");
		fscanf(mem_file, "%ld", &total_mem);
		pclose(mem_file);

		total_mem /= (long) mb;
	}

	if (OS != DFBSD)
		snprintf(str, MAX_STRLEN, "%ld%s / %ld%s", used_mem, "MB", total_mem, "MB");
	else
		snprintf(str, MAX_STRLEN, "%ld%s", total_mem, "MB");

	if (verbose)
		VERBOSE_OUT("Found memory usage as ", str);

	return;
}

/*  detect_shell
    detects the shell currently running on the computer
    argument char* str: the char array to be filled with the shell name and version
    --
    CAVEAT: shell version detection relies on the standard versioning format for 
    each shell. If any shell's older (or newer versions) suddenly begin to use a new
    scheme, the version may be displayed incorrectly.
    --
*/
void detect_shell(char* str)
{
	FILE* shell_file;

	char shell_name[MAX_STRLEN];
	char vers_str[MAX_STRLEN];

	shell_file = popen("echo $SHELL | awk -F \"/\" '{print $NF}' | tr -d '\\r\\n'", "r");
	fgets(shell_name, MAX_STRLEN, shell_file);
	pclose(shell_file);

	if (STRCMP(shell_name, "bash"))
	{
		shell_file = popen("bash --version | head -1", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);
		/* evil pointer arithmetic */
		snprintf(str, MAX_STRLEN, "%s %.*s", shell_name, 17, vers_str + 10);
		pclose(shell_file);
	}

	else if (STRCMP(shell_name, "zsh"))
	{
		shell_file = popen("zsh --version", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);	
		/* evil pointer arithmetic */
		snprintf(str, MAX_STRLEN, "%s %.*s", shell_name, 5, vers_str + 4);
		pclose(shell_file);
	}

	else if (STRCMP(shell_name, "csh"))
	{
		shell_file = popen("csh --version | head -1", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);
		/* evil pointer arithmetic */
		snprintf(str, MAX_STRLEN, "%s %.*s", shell_name, 7, vers_str + 5);
		pclose(shell_file);
	}
	
	else if (STRCMP(shell_name, "fish"))
	{
		shell_file = popen("fish --version", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);
		/* evil pointer arithmetic */
		snprintf(str, MAX_STRLEN, "%s %.*s", shell_name, 13, vers_str + 6);
		pclose(shell_file);
	}

	else if (STRCMP(shell_name, "dash") || STRCMP(shell_name, "ash") || STRCMP(shell_name, "ksh"))
	{
		/* i don't have a version detection system for these, yet */
		safe_strncpy(str, shell_name, MAX_STRLEN);
	}

	if (verbose)
		VERBOSE_OUT("Found shell as ", str);

	return;
}

/*  detect_res
    detects the combined resoloution of all monitors attached to the computer
    argument char* str: the char array to be filled with the resolution in format '$x$', where $ is a number
*/
void detect_res(char* str)
{
	FILE* res_file;

	char width_str[MAX_STRLEN];
	char height_str[MAX_STRLEN];

	if (OS == CYGWIN)
	{
		res_file = popen("wmic desktopmonitor get screenwidth | tail -2 | tr -d '\\r\\n '", "r");
		fgets(width_str, MAX_STRLEN, res_file);
		pclose(res_file);

		res_file = popen("wmic desktopmonitor get screenheight | tail -2 | tr -d '\\r\\n '", "r");
		fgets(height_str, MAX_STRLEN, res_file);
		pclose(res_file);

		snprintf(str, MAX_STRLEN, "%sx%s", width_str, height_str);
	}

	else if (OS == OSX)
	{
		res_file = popen("system_profiler SPDisplaysDataType | awk '/Resolution:/ {print $2\"x\"$4\" \"}' | tr -d '\\n'", "r");
		fgets(str, MAX_STRLEN, res_file);
		pclose(res_file);
	}

	else if (OS == LINUX)
	{
		res_file = popen("xdpyinfo | sed -n 's/.*dim.* \\([0-9]*x[0-9]*\\) .*/\\1/pg' | sed ':a;N;$!ba;s/\\n/ /g' | tr -d '\\n' 2> /dev/null", "r");
		fgets(str, MAX_STRLEN, res_file);
		pclose(res_file);
	}

	else if (ISBSD())
	{
		res_file = popen("xdpyinfo | sed -n 's/.*dim.* \\([0-9]*x[0-9]*\\) .*/\\1/pg' | tr '\\n' ' '", "r");
		fgets(str, MAX_STRLEN, res_file);
		pclose(res_file);
	}

	if (verbose)
		VERBOSE_OUT("Found resolution as ", str);

	return;
}

/*  detect_de
    detects the desktop environment currently running on top of the OS
    argument char* str: the char array to be filled with the DE name
*/
void detect_de(char* str)
{
	FILE* de_file;

	if (OS == CYGWIN)
	{
		int version;

		de_file = popen("wmic os get version | grep -o '^[0-9]'", "r");
		fscanf(de_file, "%d", &version);
		pclose(de_file);

		if (version == 6 || version == 7)
		{
			safe_strncpy(str, "Aero", MAX_STRLEN);
		}
		else
		{
			safe_strncpy(str, "Luna", MAX_STRLEN);
		}
	}

	else if (OS == OSX)
	{
		safe_strncpy(str, "Aqua", MAX_STRLEN);
	}

	else if (OS == LINUX || ISBSD())
	{
		/* find a way to self contain and execute detectde.sh */
	}

	if (verbose)
		VERBOSE_OUT("Found DE as ", str);

	return;
}

/* detect_wm
   detects the window manager currently running on top of the OS
   argument char* str: the char array to be filled with the WM name
*/
void detect_wm(char* str)
{
	FILE* wm_file;

	if (OS == CYGWIN)
	{
		wm_file = popen("tasklist | grep -o 'bugn' | tr -d '\\r\\n'", "r");
		/* test for bugn */
		pclose(wm_file);

		wm_file = popen("tasklist | grep -o 'Windawesome' | tr -d '\\r \\n'", "r");
		/* test for Windawesome */
		pclose(wm_file);

		/* else */
		safe_strncpy(str, "DWM", MAX_STRLEN);
	}

	else if (OS == OSX)
	{
		safe_strncpy(str, "Quartz Compositor", MAX_STRLEN);
	}

	else if (OS == LINUX || ISBSD())
	{
		/* find a way to self contain and execute detectwm.sh */
	}

	if (verbose)
		VERBOSE_OUT("Found WM as ", str);

	return;
}

/*  detect_wm_theme
    detects the theme associated with the WM detected in detect_wm()
    argument char* str: the char array to be filled with the WM Theme name
*/
void detect_wm_theme(char* str)
{
	FILE* wm_theme_file;

	if (OS == CYGWIN)
	{
		/* nasty one-liner */
		wm_theme_file = popen("reg query 'HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes' /v 'CurrentTheme' | grep -o '[A-Z]:\\\\.*' | awk -F\"\\\\\" '{print $NF}' | grep -o '[0-9A-z. ]*$' | grep -o '^[0-9A-z ]*' | tr -d '\\r\\n'", "r");
		fgets(str, MAX_STRLEN, wm_theme_file);
		pclose(wm_theme_file);
	}

	else if (OS == OSX)
	{
		safe_strncpy(str, "Aqua", MAX_STRLEN);
	}

	else if (OS == LINUX || ISBSD())
	{
		/* find a way to self contain and execute detectwmtheme.sh */
	}

	if (verbose)
		VERBOSE_OUT("Found WM theme as ", str);

	return;
}

/*  detect_gtk
    detects the theme, icon(s), and font(s) associated with a GTK DE (if present)
    argument char* str: the char array to be filled with the GTK info
*/
void detect_gtk(char* str)
{
	FILE* gtk_file;

	char font_str[MAX_STRLEN];

	if (OS == CYGWIN)
	{
		/* get the terminal's font */
		gtk_file = popen("cat $HOME/.minttyrc | grep '^Font=.*' | grep -o '[0-9A-z ]*$'", "r");
		fgets(font_str, MAX_STRLEN, gtk_file);
		pclose(gtk_file);
	}

	else if (OS == OSX)
	{
		/* OSX doesn't use GTK, so maybe detect Fonts here? */
	}

	else if (OS == LINUX || ISBSD())
	{
		/* find a way to self contain and execute detectgtk.sh */
	}

	if (verbose)
		VERBOSE_OUT("Found GTK as ", str);

	return;
}

/*  fill_detected_arr
    fills an array of 15 strings with the data gathered from the detect functions
    argument char* arr[]: the array of strings to be filled
    arguments char* distro...gtk: the strings to be placed in arr[]
    --
    CAVEAT: the order of the parameters is NOT the order of the array
    --
*/
void fill_detected_arr(char* arr[], char* distro, char* arch, char* host, char* kernel, char* uptime, char* pkgs, char* cpu, char* gpu, char* disk, char* mem, char* shell, char* res, char* de, char* wm, char* wm_theme, char* gtk)
{
	arr[0] = host;
	arr[1] = distro;
	arr[2] = kernel;
	arr[3] = arch;
	arr[4] = cpu;
	arr[5] = gpu;
	arr[6] = shell;
	arr[7] = pkgs;
	arr[8] = disk;
	arr[9] = mem;
	arr[10] = uptime;
	arr[11] = res;
	arr[12] = de;
	arr[13] = wm;
	arr[14] = wm_theme;
	arr[15] = gtk;

	return;
}

/*  **  END DETECTION FUNCTIONS  **  */


/*  **  BEGIN AUXILIARY FUNCTIONS  **  */

/*  safe_strncpy
    calls strncpy with the given params, then inserts a null char at the last position
    returns a pointer to a string containing the copied data (same as destination)
*/
char* safe_strncpy(char* destination, const char* source, size_t num)
{
	char* ret = strncpy(destination, source, num);
	ret[num - 1] = '\0';
	return ret;
}

/*  split_uptime
    splits param uptime into individual time-units
    argument float uptime: the uptime, in seconds, to be split
    arguments int* secs...days: pointers to ints where the split uptime will be stored
    --
    CAVEAT: uptime MUST be in seconds
    --
*/
void split_uptime(float uptime, int* secs, int* mins, int* hrs, int* days)
{
	*secs = (int) uptime % 60;
	*mins = (int) (uptime / 60) % 60;
	*hrs = (int) (uptime / 3600) % 24;
	*days = (int) (uptime / 86400);

	return;
}

/*  **  END AUXILIARY FUNCTIONS  **  */


/*  **  BEGIN FLAG/OUTPUT/MISC FUNCTIONS  **  */

/*  main_output
    the primary output for screenfetch-c - all info and ASCII art is printed here
    arguments char* data[], char* data_names[]: string arrays containing the names and data acquired
*/
void main_ascii_output(char* data[], char* data_names[])
{
	int i;

	if (OS == CYGWIN)
	{
		for (i = 0; i < 16; i++)
		{
			printf("%s %s%s%s%s%s\n", windows_logo[i], TRED, data_names[i], TWHT, data[i], TNRM);
		}
	}

	else if (OS == OSX)
	{
		for (i = 0; i < 16; i++)
		{
			printf("%s %s%s%s%s\n", macosx_logo[i], TLBL, data_names[i], TNRM, data[i]);
		}
	}

	else if (OS == LINUX)
	{
		if (STRCMP(distro_str, "Arch Linux - Old"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s%s\n", oldarch_logo[i], TLBL, data_names[i], TNRM, data[i], TNRM);
				else
					printf("%s\n", oldarch_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Arch Linux"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s%s\n", oldarch_logo[i], TLCY, data_names[i], TNRM, data[i], TNRM);
				else
					printf("%s\n", arch_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "LinuxMint"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s%s\n", mint_logo[i], TLGN, data_names[i], TNRM, data[i], TNRM);
				else
					printf("%s\n", mint_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "LMDE"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", lmde_logo[i], TLGN, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", lmde_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Ubuntu"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s%s\n", ubuntu_logo[i], TLRD, data_names[i], TNRM, data[i], TNRM);
				else
					printf("%s\n", ubuntu_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Debian"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", debian_logo[i], TLRD, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", debian_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "CrunchBang"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", crunchbang_logo[i], TLGY, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", crunchbang_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Gentoo"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", gentoo_logo[i], TLPR, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", gentoo_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Funtoo"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", funtoo_logo[i], TLPR, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", funtoo_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Fedora"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", fedora_logo[i], TLBL, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", fedora_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Mandriva") || STRCMP(distro_str, "Mandrake"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", mandriva_mandrake_logo[i], TLBL, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", mandriva_mandrake_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "OpenSUSE"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", opensuse_logo[i], TLGN, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", opensuse_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Slackware"))
		{
			for (i = 0; i < 21; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", slackware_logo[i], TLBL, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", slackware_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Red Hat Linux"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", redhat_logo[i], TRED, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", redhat_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Frugalware"))
		{
			for (i = 0; i < 23; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", frugalware_logo[i], TLCY, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", frugalware_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Peppermint"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", peppermint_logo[i], TLRD, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", peppermint_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "SolusOS"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", solusos_logo[i], TLGY, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", solusos_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Mageia"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", mageia_logo[i], TLCY, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", mageia_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "ParabolaGNU/Linux-libre"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", parabolagnu_linuxlibre_logo[i], TLPR, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", parabolagnu_linuxlibre_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Viperr"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", viperr_logo[i], TLGY, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", viperr_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "LinuxDeepin"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", linuxdeepin_logo[i], TLGN, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", linuxdeepin_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Chakra"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", chakra_logo[i], TLBL, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", chakra_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Fuduntu"))
		{
			for (i = 0; i < 21; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", fuduntu_logo[i], TLRD, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", fuduntu_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Trisquel"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", trisquel_logo[i], TLBL, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", trisquel_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Manjaro"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s\n", manjaro_logo[i], data_names[i], data[i]);
				else
					printf("%s\n", manjaro_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "elementary OS"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", elementaryos_logo[i], TLGN, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", elementaryos_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Scientific Linux"))
		{
			for (i = 0; i < 20; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", scientificlinux_logo[i], TLRD, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", scientificlinux_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Backtrack Linux"))
		{
			for (i = 0; i < 20; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", backtracklinux_logo[i], TLRD, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", backtracklinux_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Sabayon"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", sabayon_logo[i], TLBL, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", sabayon_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Linux"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", linux_logo[i], TLGY, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", linux_logo[i]);
			}
		}
	}

	else if (OS == FREEBSD)
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s\n", freebsd_logo[i], TLRD, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", freebsd_logo[i]);
		}
	}

	else if (OS == OPENBSD)
	{
		/* i'm going to add these later */
	}

	else if (OS == NETBSD)
	{
		/* i'm going to add these later */
	}

	else if (OS == DFBSD)
	{
		/* i'm going to add these later */
	}

	return;
}

/*  main_text_output
	the secondary output for screenfetch-c - all info WITHOUT ASCII art is printed here
	arguments char* data[], char* data_names[]: string arrays containing the names and data acquired
*/
void main_text_output(char* data[], char* data_names[])
{
	int i;

	for (i = 0; i < 16; i++)
		printf("%s %s\n", data_names[i], data[i]);

	return;
}

/*  display_version
    called if the -v flag is tripped, outputs the current version of screenfetch-c
*/
void display_version(void)
{
	printf("%s\n", TBLU "screenfetch-c - Version 1.0 BETA");
	printf("%s\n", "Warning: This version of screenfetch is not yet finished.");
	printf("%s\n", "It may be lacking support for certain Linux/BSD distros." TNRM);

	return;
}

/*  display_help
    called if the -h flag is tripped, tells the user where to find the manpage
*/
void display_help(void)
{
	printf("%s\n", TBLU "screenfetch-c");
	printf("%s\n", "A rewrite of screenFetch, the popular shell script, in C.");
	printf("%s\n", "Operating Systems currently supported:");
	printf("%s\n", "Windows (via Cygwin), Linux, *BSD, and OS X.");
	printf("%s\n", "Using screenfetch-c on an OS not listed above may not work entirely or at all (and is disabled by default).");
	printf("%s\n", "Please access 'man screenfetch' for in-depth information on compatibility and usage." TNRM);
	return;
}

/*  take_screenshot
    takes a screenshot and saves it to $HOME 
    --
    CAVEAT: THIS FUNCTION MAKES SYSTEM CALLS
    --
*/
void take_screenshot(void)
{
	FILE* ss_file;
	if (OS == CYGWIN || OS == UNKNOWN)
	{
		/* cygwin does not currently have a simple screenshot solution */
		/* potential solutions: "import -window root screenfetch_screenshot.jpg" - requires X */
		ERROR_OUT("Error: ", "This program does not currently support screenshots on your OS.");
		return;
	}

	printf("%s", "Taking shot in 3..");
	fflush(stdout);
	sleep(1);
	printf("%s", "2..");
	fflush(stdout);
	sleep(1);
	printf("%s", "1..");
	fflush(stdout);
	sleep(1);
	printf("%s\n", "0");

	if (OS == OSX)
	{
		system("screencapture -x ~/screenfetch_screenshot.png 2> /dev/null");	
	}

	else if (OS == LINUX || ISBSD())
	{
		system("scrot ~/screenfetch_screenshot.png 2> /dev/null");
	}

	char* loc = getenv("HOME");
	strcat(loc, "/screenfetch_screenshot.png");
	ss_file = fopen(loc, "r");
	if (ss_file != NULL && verbose)
	{
		fclose(ss_file);
		VERBOSE_OUT("Screenshot successfully saved.", "");
	}
		
	else if (ss_file == NULL)
	{
		ERROR_OUT("Error: ", "Problem saving screenshot.");
	}

	return;
}

/*  **  END FLAG/OUTPUT/MISC FUNCTIONS  **  */


/* ** EOF ** */
