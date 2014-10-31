/*	screenfetch-c.c
	Author: William Woodruff
	-------------

	A rewrite of screenFetch.sh in C.
	This is primarily an experiment borne out of an awareness of the slow execution time on the 
	screenfetch-dev.sh script. 
	Hopefully this port will execute faster, although it's more for self education than anything else.
	
	------

	NOTES:
	I used many of Brett Bohnenkamper's awk/sed/grep/etc oneliners in my popen() calls, 
	although some were modified to change/improve the output.
	Many thanks to him for discovering bugs and formatting issues as well.

	The shell scripts detectwm.sh, detectwmtheme.sh, and detectde.sh are all partially or completely based upon
	screenfetch-dev. They are called within the program, as detecting the WM/WM theme/DE/GTK within C would be a pain in the ass.

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

	I hereby regrant screenfetch-c under the same MIT license.
	If you have any questions, please contact me on github (http://www.github.com/woodruffw/screenfetch-c) or at william @ tuffbizz.com
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include "screenfetch-c.h" /* detection function prototypes, macros */
#include "detect.h"
#include "disp.h" /* version and help output functions */
#include "logos.h" /* ascii logos */
#include "colors.h" /* terminal color codes */
#include "misc.h" /* misc macros */
#include "util.h" /* convenience functions */
#include "thread.h" /* for cross-platform threading */

/* string definitions - set to Unknown by default */
static char distro_str[MAX_STRLEN] = "Unknown";
static char arch_str[MAX_STRLEN] = "Unknown";
static char host_str[MAX_STRLEN] = "Unknown";
static char kernel_str[MAX_STRLEN] = "Unknown";
static char uptime_str[MAX_STRLEN] = "Unknown";
static char pkgs_str[MAX_STRLEN] = "Unknown";
static char cpu_str[MAX_STRLEN] = "Unknown";
static char gpu_str[MAX_STRLEN] = "Unknown";
static char disk_str[MAX_STRLEN] = "Unknown";
static char mem_str[MAX_STRLEN] = "Unknown";
static char shell_str[MAX_STRLEN] = "Unknown";
static char res_str[MAX_STRLEN] = "Unknown";
static char de_str[MAX_STRLEN] = "Unknown";
static char wm_str[MAX_STRLEN] = "Unknown";
static char wm_theme_str[MAX_STRLEN] = "Unknown";
static char gtk_str[MAX_STRLEN] = "Unknown";

/* output string definitions */
static char *detected_arr[16] = {
									host_str,
									distro_str,
									kernel_str,
									arch_str,
									cpu_str,
									gpu_str,
									shell_str,
									pkgs_str,
									disk_str,
									mem_str,
									uptime_str,
									res_str,
									de_str,
									wm_str,
									wm_theme_str,
									gtk_str
								};

static char *detected_arr_names[16] = {
										"",
										"OS: ",
										"Kernel: ",
										"Arch: ",
										"CPU: ",
										"GPU: ",
										"Shell: ",
										"Packages: ",
										"Disk: ",
										"Memory: ",
										"Uptime: ",
										"Resolution: ",
										"DE: ",
										"WM: ",
										"WM Theme: ",
										"GTK: "
									};

/* other definitions */
bool manual = false;
bool logo = true;
bool error = true;
bool verbose = false;
bool screenshot = false;

int main(int argc, char **argv)
{
	/* warn unknown OSes about using this program */
	if (OS == UNKNOWN)
	{
		ERROR_OUT("Warning: ", "This program isn't designed for your OS.");
		ERROR_OUT("Even if it did compile successfully, it will not execute correctly.", "");
		ERROR_OUT("It is HIGHLY recommended, therefore, that you use manual mode.", "");
	}

	struct option long_options[] =
	{
		{ "manual", no_argument, 0, 'm' },
		{ "verbose", no_argument, 0, 'v' },
		{ "no-logo", no_argument, 0, 'n' },
		{ "screenshot", no_argument, 0, 's' },
		{ "distro", required_argument, 0, 'D' },
		{ "suppress-errors", no_argument, 0, 'E' },
		{ "version", no_argument, 0, 'V' },
		{ "help", no_argument, 0, 'h' },
		{ "logo-only", required_argument, 0, 'L' },
		{ 0, 0, 0, 0 }
	};

	signed char c;
	int index = 0;
	while ((c = getopt_long(argc, argv, "mvnsD:EVhL:", long_options, &index)) != -1)
	{
		switch (c)
		{
			case 'm':
				manual = true;
				break;
			case 'v':
				verbose = true;
				break;
			case 'n':
				logo = false;
				break;
			case 's':
				screenshot = true;
				break;
			case 'D':
				SET_DISTRO(optarg);
				break;
			case 'E':
				error = false;
				break;
			case 'V':
				display_version();
				return EXIT_SUCCESS;
			case 'h':
				display_help();
				return EXIT_SUCCESS;
			case 'L':
				output_logo_only(optarg);
				return EXIT_SUCCESS;
			case '?':
				if (optopt == 'D')
					ERROR_OUT("Error: ", "The -D (--distro) flag requires an argument.");
				else if (optopt == 'L')
					ERROR_OUT("Error: ", "The -L (--logo-only) flag requires an argument.");
				else
					ERROR_OUT("Error: ", "Unknown option or option character.");
				return EXIT_FAILURE;
		}
	}

	if (manual) /* if the user has decided to enter manual mode */
	{
		int stat = manual_input(verbose);

		if (stat == EXIT_SUCCESS)
		{
			/* these sections are ALWAYS detected */
			detect_uptime(uptime_str);
			detect_pkgs(pkgs_str);
			detect_disk(disk_str);
			detect_mem(mem_str);

			/* if the user specifies an asterisk, fill the data in for them */
			if (STRCMP(distro_str, "*"))
				detect_distro(distro_str, error);
			if (STRCMP(arch_str, "*"))
				detect_arch(arch_str);
			if (STRCMP(host_str, "*"))
				detect_host(host_str);
			if (STRCMP(kernel_str, "*"))
				detect_kernel(kernel_str);
			if (STRCMP(cpu_str, "*"))
				detect_cpu(cpu_str);
			if (STRCMP(gpu_str, "*"))
				detect_gpu(gpu_str, error);
			if (STRCMP(shell_str, "*"))
				detect_shell(shell_str, error);
			if (STRCMP(res_str, "*"))
				detect_res(res_str, error);
			if (STRCMP(de_str, "*"))
				detect_de(de_str);
			if (STRCMP(wm_str, "*"))
				detect_wm(wm_str);
			if (STRCMP(wm_theme_str, "*"))
				detect_wm_theme(wm_theme_str);
			if (STRCMP(gtk_str, "*"))
				detect_gtk(gtk_str);
		}

		else /* if the user decided to leave manual mode without input */
			return EXIT_SUCCESS;
	}

	else /* each string is filled by its respective function */
	{
		if (OS != CYGWIN)
		{
			detect_distro(distro_str, error);

			THREAD arch_thread;
			create_thread(&arch_thread, (void *) detect_arch, (void *) arch_str);

			THREAD host_thread;
			create_thread(&host_thread, (void *) detect_host, (void *) host_str);

			THREAD kernel_thread;
			create_thread(&kernel_thread, (void *) detect_kernel, (void *) kernel_str);

			THREAD uptime_thread;
			create_thread(&uptime_thread, (void *) detect_uptime, (void *) uptime_str);

			THREAD pkgs_thread;
			create_thread(&pkgs_thread, (void *) detect_pkgs, (void *) pkgs_str);

			THREAD cpu_thread;
			create_thread(&cpu_thread, (void *) detect_cpu, (void *) cpu_str);

			detect_gpu(gpu_str, error);

			THREAD disk_thread;
			create_thread(&disk_thread, (void *) detect_disk, (void *) disk_str);

			THREAD mem_thread;
			create_thread(&mem_thread, (void *) detect_mem, (void *) mem_str);

			detect_shell(shell_str, error);

			detect_res(res_str, error);

			THREAD de_thread;
			create_thread(&de_thread, (void *) detect_de, (void *) de_str);

			THREAD wm_thread;
			create_thread(&wm_thread, (void *) detect_wm, (void *) wm_str);

			THREAD wm_theme_thread;
			create_thread(&wm_theme_thread, (void *) detect_wm_theme, (void *) wm_theme_str);

			THREAD gtk_thread;
			create_thread(&gtk_thread, (void *) detect_gtk, (void *) gtk_str);

			join_thread(arch_thread);
			join_thread(host_thread);
			join_thread(kernel_thread);
			join_thread(uptime_thread);
			join_thread(pkgs_thread);
			join_thread(cpu_thread);
			join_thread(disk_thread);
			join_thread(mem_thread);
			join_thread(de_thread);
			join_thread(wm_thread);
			join_thread(wm_theme_thread);
			join_thread(gtk_thread);
		}

		else /* i haven't perfected threading on windows yet, so don't change this conditional */
		{
			detect_distro(distro_str, error);
			detect_arch(arch_str);
			detect_host(host_str);
			detect_kernel(kernel_str);
			detect_uptime(uptime_str);
			detect_pkgs(pkgs_str);
			detect_cpu(cpu_str);
			detect_gpu(gpu_str, error);
			detect_disk(disk_str);
			detect_mem(mem_str);
			detect_shell(shell_str, error);
			detect_res(res_str, error);
			detect_de(de_str);
			detect_wm(wm_str);
			detect_wm_theme(wm_theme_str);
			detect_gtk(gtk_str);
		}
	}

	if (verbose)
	{
		VERBOSE_OUT("Found distro as ", distro_str);
		VERBOSE_OUT("Found system arch as ", arch_str);
		VERBOSE_OUT("Found hostname ", host_str);
		VERBOSE_OUT("Found kernel as ", kernel_str);
		VERBOSE_OUT("Found uptime as ", uptime_str);
		VERBOSE_OUT("Found package count as ", pkgs_str);
		VERBOSE_OUT("Found CPU as ", cpu_str);
		VERBOSE_OUT("Found GPU as ", gpu_str);
		VERBOSE_OUT("Found disk usage as ", disk_str);
		VERBOSE_OUT("Found memory usage as ", mem_str);
		VERBOSE_OUT("Found shell as ", shell_str);
		VERBOSE_OUT("Found resolution as ", res_str);
		VERBOSE_OUT("Found DE as ", de_str);
		VERBOSE_OUT("Found WM as ", res_str);
		VERBOSE_OUT("Found WM theme as ", wm_theme_str);
		VERBOSE_OUT("Found GTK as ", gtk_str);
	}

	if (logo)
		main_ascii_output(detected_arr, detected_arr_names);
	else
		main_text_output(detected_arr, detected_arr_names);

	if (screenshot)
		take_screenshot(verbose, error);

	return EXIT_SUCCESS;
}

/*  **  BEGIN DETECTION FUNCTIONS  ** */

/*	detect_pkgs
	detects the number of packages installed on the computer
	argument char *str: the char array to be filled with the number of packages
*/
void detect_pkgs(char *str)
{
	FILE *pkgs_file;

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
		if (FILE_EXISTS("/usr/local/bin/brew"))
		{
			#if (defined(__APPLE__) || defined(__MACH__))
				glob_t gl;

				if (glob("/usr/local/Cellar/*", GLOB_NOSORT, NULL, &gl) == 0)
				{
					packages = gl.gl_pathc;
				}
				else if (error)
				{
					ERROR_OUT("Error: ", "Failure while globbing packages.");
				}

				globfree(&gl);
			#endif
		}

		if (FILE_EXISTS("/opt/local/bin/port"))
		{
			int port_pkgs = 0;
			pkgs_file = popen("port installed | wc -l", "r");
			fscanf(pkgs_file, "%d", &port_pkgs);
			pclose(pkgs_file);

			packages += port_pkgs;
		}

		if (FILE_EXISTS("/sw/bin/fink"))
		{
			int fink_pkgs = 0;
			pkgs_file = popen("/sw/bin/fink list -i | wc -l", "r");
			fscanf(pkgs_file, "%d", &fink_pkgs);
			pclose(pkgs_file);

			packages += fink_pkgs;
		}
	}

	else if (OS == LINUX)
	{
		if (STRCMP(distro_str, "Arch Linux") || STRCMP(distro_str, "ParabolaGNU/Linux-libre") || STRCMP(distro_str, "Chakra") || STRCMP(distro_str, "Manjaro"))
		{
			#if defined(__linux__)
				glob_t gl;

				if (glob("/var/lib/pacman/local/*", GLOB_NOSORT, NULL, &gl) == 0)
				{
					packages = gl.gl_pathc;
				}
				else if (error)
				{
					ERROR_OUT("Error: ", "Failure while globbing packages.");
				}

				globfree(&gl);
			#endif
		}

		else if (STRCMP(distro_str, "Frugalware"))
		{
			pkgs_file = popen("pacman-g2 -Q | wc -l", "r");
			fscanf(pkgs_file, "%d", &packages);
			pclose(pkgs_file);
		}

		else if (STRCMP(distro_str, "Ubuntu") || STRCMP(distro_str, "Lubuntu") || STRCMP(distro_str, "Xubuntu") || STRCMP(distro_str, "LinuxMint") || STRCMP(distro_str, "SolusOS") || STRCMP(distro_str, "Debian") || STRCMP(distro_str, "LMDE") || STRCMP(distro_str, "CrunchBang") || STRCMP(distro_str, "Peppermint") || STRCMP(distro_str, "LinuxDeepin") || STRCMP(distro_str, "Trisquel") || STRCMP(distro_str, "elementary OS") || STRCMP(distro_str, "Backtrack Linux"))
		{
			pkgs_file = popen("dpkg --get-selections | wc -l", "r");
			fscanf(pkgs_file, "%d", &packages);
			pclose(pkgs_file);
		}

		else if (STRCMP(distro_str, "Slackware"))
		{
			#if defined(__linux__)
				glob_t gl;

				if (glob("/var/log/packages/*", GLOB_NOSORT, NULL, &gl) == 0)
				{
					packages = gl.gl_pathc;
				}
				else if (error)
				{
					ERROR_OUT("Error: ", "Failure while globbing packages.");
				}

				globfree(&gl);
			#endif
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

		else if (STRCMP(distro_str, "Angstrom"))
		{
			pkgs_file = popen("opkg list-installed | wc -l", "r");
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
		pkgs_file = popen("pkg_info | wc -l", "r");
		fscanf(pkgs_file, "%d", &packages);
		pclose(pkgs_file);
	}

	else if (OS == NETBSD || OS == DFBSD)
	{
		safe_strncpy(str, "Not Found", MAX_STRLEN);

		if (error)
			ERROR_OUT("Error: ", "Could not find packages on current OS.");
	}

	else if (OS == SOLARIS)
	{
		pkgs_file = popen("pkg list | wc -l", "r");
		fscanf(pkgs_file, "%d", &packages);
		pclose(pkgs_file);
	}

	snprintf(str, MAX_STRLEN, "%d", packages);

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
	FILE *wm_file;

	if (OS == CYGWIN)
	{
		/* wm_file = popen("tasklist | grep -o 'bugn' | tr -d '\\r\\n'", "r"); */
		/* test for bugn */
		/* pclose(wm_file); */

		/* wm_file = popen("tasklist | grep -o 'Windawesome' | tr -d '\\r \\n'", "r"); */
		/* test for Windawesome */
		/* pclose(wm_file); */

		/* else */
		safe_strncpy(str, "DWM", MAX_STRLEN);
	}

	else if (OS == OSX)
	{
		safe_strncpy(str, "Quartz Compositor", MAX_STRLEN);
	}

	else if (OS == LINUX || ISBSD() || OS == SOLARIS)
	{
		wm_file = popen("detectwm 2> /dev/null", "r");
		fgets(str, MAX_STRLEN, wm_file);
		pclose(wm_file);
	}

	return;
}

/*	detect_wm_theme
	detects the theme associated with the WM detected in detect_wm()
	argument char *str: the char array to be filled with the WM Theme name
	--
	CAVEAT: On *BSDs and Linux distros, this function relies on the presence of 
	'detectwmtheme', a shell script. If it isn't present in the working directory, the WM Theme will be set as 'Unknown'
	--
*/
void detect_wm_theme(char *str)
{
	FILE *wm_theme_file;

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

	else if (OS == LINUX || ISBSD() || OS == SOLARIS)
	{
		wm_theme_file = popen("detectwmtheme 2> /dev/null", "r");
		fgets(str, MAX_STRLEN, wm_theme_file);
		pclose(wm_theme_file);
	}

	return;
}

/*	detect_gtk
	detects the theme, icon(s), and font(s) associated with a GTK DE (if present)
	argument char *str: the char array to be filled with the GTK info
	--
	CAVEAT: On *BSDs and Linux distros, this function relies on the presence of 
	'detectgtk', a shell script. If it isn't present in the working directory, the GTK will be set as 'Unknown'
	--
*/
void detect_gtk(char *str)
{
	FILE *gtk_file;

	char gtk2_str[MAX_STRLEN] = "Unknown";
	char gtk3_str[MAX_STRLEN] = "Unknown";
	char gtk_icons_str[MAX_STRLEN] = "Unknown";
	char font_str[MAX_STRLEN] = "Unknown";

	if (OS == CYGWIN)
	{
		/* get the terminal's font */
		gtk_file = popen("grep '^Font=.*' < $HOME/.minttyrc | grep -o '[0-9A-z ]*$' | tr -d '\\r\\n'", "r");
		fgets(font_str, MAX_STRLEN, gtk_file);
		pclose(gtk_file);

		snprintf(str, MAX_STRLEN, "%s (Font)", font_str);
	}

	else if (OS == OSX)
	{
		safe_strncpy(str, "Not Applicable", MAX_STRLEN);
	}

	else if (OS == LINUX || ISBSD())
	{
		gtk_file = popen("detectgtk 2> /dev/null", "r");
		fscanf(gtk_file, "%s%s%s%s", gtk2_str, gtk3_str, gtk_icons_str, font_str);
		pclose(gtk_file);

		if (STRCMP(gtk3_str, "Unknown"))
			snprintf(str, MAX_STRLEN, "%s (GTK2), %s (Icons)", gtk2_str, gtk_icons_str);
		else if (STRCMP(gtk2_str, "Unknown"))
			snprintf(str, MAX_STRLEN, "%s (GTK3), %s (Icons)", gtk3_str, gtk_icons_str);
		else
			snprintf(str, MAX_STRLEN, "%s (GTK2), %s (GTK3)", gtk2_str, gtk3_str);
	}

	else if (OS == SOLARIS)
	{
		/* detectgtk needs to be made compatible with Solaris's awk */
	}

	return;
}

/*  manual_input
	generates (or reads) the ~/.screenfetchc file based upon user input
	returns an int indicating status (SUCCESS or FAILURE)
*/
int manual_input(bool verbose)
{
	FILE *config_file;
	char config_file_loc[MAX_STRLEN];

	safe_strncpy(config_file_loc, getenv("HOME"), MAX_STRLEN);
	strncat(config_file_loc, "/.screenfetchc", MAX_STRLEN);

	if (!FILE_EXISTS(config_file_loc))
	{
		if (OS == CYGWIN)
		{
			printf("%s\n", TBLU "WARNING: There is currenly a bug involving manual mode on Windows." TNRM);
			printf("%s\n", TBLU "Only continue if you are ABSOLUTELY sure." TNRM);
		}

		printf("%s\n", "This appears to be your first time running screenfetch-c in manual mode.");
		printf("%s", "Would you like to continue? (y/n) ");

		char choice = getchar();
		getchar(); /* flush the newline */

		if (choice == 'y' || choice == 'Y')
		{
			config_file = fopen(config_file_loc, "w");

			printf("%s\n", "We are now going to begin the manual mode input process.");
			printf("%s\n", "Please enter exactly what is asked for.");
			printf("%s\n", "If you are unsure about format, please consult the manpage.");

			printf("%s", "Please enter the name of your distribution/OS: ");
			fgets(distro_str, MAX_STRLEN, stdin);
			fputs(distro_str, config_file);

			printf("%s", "Please enter your architecture: ");
			fgets(arch_str, MAX_STRLEN, stdin);
			fputs(arch_str, config_file);

			printf("%s", "Please enter your username@hostname: ");
			fgets(host_str, MAX_STRLEN, stdin);
			fputs(host_str, config_file);

			printf("%s", "Please enter your kernel: ");
			fgets(kernel_str, MAX_STRLEN, stdin);
			fputs(kernel_str, config_file);

			printf("%s", "Please enter your CPU name: ");
			fgets(cpu_str, MAX_STRLEN, stdin);
			fputs(cpu_str, config_file);

			printf("%s", "Please enter your GPU name: ");
			fgets(gpu_str, MAX_STRLEN, stdin);
			fputs(gpu_str, config_file);

			printf("%s", "Please enter your shell name and version: ");
			fgets(shell_str, MAX_STRLEN, stdin);
			fputs(shell_str, config_file);

			printf("%s", "Please enter your monitor resolution: ");
			fgets(res_str, MAX_STRLEN, stdin);
			fputs(res_str, config_file);

			printf("%s", "Please enter your DE name: ");
			fgets(de_str, MAX_STRLEN, stdin);
			fputs(de_str, config_file);

			printf("%s", "Please enter your WM name: ");
			fgets(wm_str, MAX_STRLEN, stdin);
			fputs(wm_str, config_file);

			printf("%s", "Please enter your WM Theme name: ");
			fgets(wm_theme_str, MAX_STRLEN, stdin);
			fputs(wm_theme_str, config_file);

			printf("%s", "Please enter any GTK info: ");
			fgets(gtk_str, MAX_STRLEN, stdin);
			fputs(gtk_str, config_file);

			printf("%s\n", "That concludes the manual input.");
			printf("%s\n", "From now on, screenfetch-c will use this information when called with -m.");

			fclose(config_file);

			/* i am deeply ashamed of this solution */
			distro_str[strlen(distro_str) - 1] = '\0';
			arch_str[strlen(arch_str) - 1] = '\0';
			host_str[strlen(host_str) - 1] = '\0';
			kernel_str[strlen(kernel_str) - 1] = '\0';
			cpu_str[strlen(cpu_str) - 1] = '\0';
			gpu_str[strlen(gpu_str) - 1] = '\0';
			shell_str[strlen(shell_str) - 1] = '\0';
			res_str[strlen(res_str) - 1] = '\0';
			de_str[strlen(de_str) - 1] = '\0';
			wm_str[strlen(wm_str) - 1] = '\0';
			wm_theme_str[strlen(wm_theme_str) - 1] = '\0';
			gtk_str[strlen(gtk_str) - 1] = '\0';

			return EXIT_SUCCESS;
		}

		else
		{
			printf("%s\n", "Exiting manual mode and screenfetch-c.");
			printf("%s\n", "If you wish to run screenfetch-c normally, do not use the -m (--manual) flag next time.");

			return EXIT_FAILURE;
		}
	}

	else
	{
		if (verbose)
			VERBOSE_OUT("Found config file. Reading...", "");

		config_file = fopen(config_file_loc, "r");

		fgets(distro_str, MAX_STRLEN, config_file);
		fgets(arch_str, MAX_STRLEN, config_file);
		fgets(host_str, MAX_STRLEN, config_file);
		fgets(kernel_str, MAX_STRLEN, config_file);
		fgets(cpu_str, MAX_STRLEN, config_file);
		fgets(gpu_str, MAX_STRLEN, config_file);
		fgets(shell_str, MAX_STRLEN, config_file);
		fgets(res_str, MAX_STRLEN, config_file);
		fgets(de_str, MAX_STRLEN, config_file);
		fgets(wm_str, MAX_STRLEN, config_file);
		fgets(wm_theme_str, MAX_STRLEN, config_file);
		fgets(gtk_str, MAX_STRLEN, config_file);

		fclose(config_file);

		/* i am deeply ashamed of this solution */
		distro_str[strlen(distro_str) - 1] = '\0';
		arch_str[strlen(arch_str) - 1] = '\0';
		host_str[strlen(host_str) - 1] = '\0';
		kernel_str[strlen(kernel_str) - 1] = '\0';
		cpu_str[strlen(cpu_str) - 1] = '\0';
		gpu_str[strlen(gpu_str) - 1] = '\0';
		shell_str[strlen(shell_str) - 1] = '\0';
		res_str[strlen(res_str) - 1] = '\0';
		de_str[strlen(de_str) - 1] = '\0';
		wm_str[strlen(wm_str) - 1] = '\0';
		wm_theme_str[strlen(wm_theme_str) - 1] = '\0';
		gtk_str[strlen(gtk_str) - 1] = '\0';

		return EXIT_SUCCESS;
	}
}

/*  **  END FLAG/OUTPUT/MISC FUNCTIONS  **  */
