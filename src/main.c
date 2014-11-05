/*	main.c
	Author: William Woodruff
	-------------

	screenfetch-c is a rewrite of screenFetch.sh in C.
	This is primarily an experiment borne out of an awareness of the slow execution time on the 
	screenfetch-dev.sh script. 
	Hopefully this rewrite will execute faster, although it's more for self education than anything else.

	If you have any questions, please contact me via:
		- github: http://www.github.com/woodruffw/screenfetch-c
		- email: william @ tuffbizz.com
*/

/* standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>

/* program includes */
#include "detect.h"
#include "disp.h"
#include "logos.h"
#include "colors.h"
#include "misc.h"
#include "util.h"

int main(int argc, char **argv)
{
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

	if (manual) /* triggered by -m (--manual) flag */
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
		display_verbose(detected_arr, detected_arr_names);

	if (logo)
		main_ascii_output(detected_arr, detected_arr_names);
	else
		main_text_output(detected_arr, detected_arr_names);

	if (screenshot)
		take_screenshot(verbose, error);

	return EXIT_SUCCESS;
}
