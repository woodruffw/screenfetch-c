/*	main.c
	Author: William Woodruff
	-------------

	screenfetch-c is a rewrite of screenFetch.sh in C.
	This is primarily an experiment borne out of an awareness of the slow execution time on the 
	screenfetch-dev.sh script. 
	Hopefully this rewrite will execute faster, although it's more for self education than anything else.
	
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

/* standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>

#include "detect.h" /* detection function prototypes */
#include "disp.h" /* version and help output functions */
#include "logos.h" /* ascii logos */
#include "colors.h" /* terminal color codes */
#include "misc.h" /* misc macros */
#include "util.h" /* convenience functions */

int main(int argc, char **argv)
{
	/* string definitions - set to Unknown by default */
	char distro_str[MAX_STRLEN] = "Unknown";
	char arch_str[MAX_STRLEN] = "Unknown";
	char host_str[MAX_STRLEN] = "Unknown";
	char kernel_str[MAX_STRLEN] = "Unknown";
	char uptime_str[MAX_STRLEN] = "Unknown";
	char pkgs_str[MAX_STRLEN] = "Unknown";
	char cpu_str[MAX_STRLEN] = "Unknown";
	char gpu_str[MAX_STRLEN] = "Unknown";
	char disk_str[MAX_STRLEN] = "Unknown";
	char mem_str[MAX_STRLEN] = "Unknown";
	char shell_str[MAX_STRLEN] = "Unknown";
	char res_str[MAX_STRLEN] = "Unknown";
	char de_str[MAX_STRLEN] = "Unknown";
	char wm_str[MAX_STRLEN] = "Unknown";
	char wm_theme_str[MAX_STRLEN] = "Unknown";
	char gtk_str[MAX_STRLEN] = "Unknown";

	/* output string definitions */
	char *detected_arr[16] =
	{
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

	char *detected_arr_names[16] =
	{
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

	bool manual = false, logo = true, error = true, verbose = false, screenshot = false;

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
		int stat = manual_input(detected_arr, verbose);

		if (stat == EXIT_SUCCESS)
		{
			/* these sections are ALWAYS detected */
			detect_uptime(uptime_str);
			detect_pkgs(pkgs_str, distro_str, error);
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
		detect_distro(distro_str, error);
		detect_arch(arch_str);
		detect_host(host_str);
		detect_kernel(kernel_str);
		detect_uptime(uptime_str);
		detect_pkgs(pkgs_str, distro_str, error);
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
