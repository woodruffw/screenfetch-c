/*	main.c
**	Author: William Woodruff
**	-------------
**
**	screenfetch-c is a rewrite of screenFetch.sh in C.
**	This is primarily an experiment borne out of an awareness of the
**	slow execution time on the screenfetch-dev.sh script. 
**	Hopefully this rewrite will execute faster, although it's more for
**	self education than anything else.
**
**	If you have any questions, please feel free to contact me via:
**		- github: http://www.github.com/woodruffw/screenfetch-c
**		- email: william @ tuffbizz.com
*/

/* standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>

/* program includes */
#include "arrays.h"
#include "detect.h"
#include "disp.h"
#include "logos.h"
#include "colors.h"
#include "misc.h"
#include "util.h"
#include "error_flag.h"

int main(int argc, char **argv)
{
	bool logo = true, portrait = false;
	bool verbose = false, screenshot = false;

	struct option options[] =
	{
		{ "verbose", no_argument, 0, 'v' },
		{ "no-logo", no_argument, 0, 'n' },
		{ "screenshot", no_argument, 0, 's' },
		{ "distro", required_argument, 0, 'D' },
		{ "suppress-errors", no_argument, 0, 'E' },
		{ "portrait", no_argument, 0, 'p'},
		{ "version", no_argument, 0, 'V' },
		{ "help", no_argument, 0, 'h' },
		{ "logo-only", required_argument, 0, 'L' },
		{ 0, 0, 0, 0 }
	};

	signed char c;
	int index = 0;
	while ((c = getopt_long(argc, argv, "vnsD:EpVhL:", options, &index)) != -1)
	{
		switch (c)
		{
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
				SET_GIVEN_DISTRO(optarg);
				break;
			case 'E':
				error = false;
				break;
			case 'p':
				portrait = true;
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
			default:
				return EXIT_FAILURE;
		}
	}

	detect_distro();
	detect_host();
	detect_kernel();
	detect_uptime();
	detect_pkgs();
	detect_cpu();
	detect_gpu();
	detect_disk();
	detect_mem();
	detect_shell();
	detect_res(res_str);
	detect_de(de_str);
	detect_wm(wm_str);
	detect_wm_theme(wm_theme_str, wm_str);
	detect_gtk(gtk_str, icon_str, font_str);

	/* if the user specified a different OS to display, set distro_set to it */
	if (!STREQ(given_distro_str, "Unknown"))
		safe_strncpy(distro_str, given_distro_str, MAX_STRLEN);

	if (verbose)
		display_verbose(detected_arr, detected_arr_names);

	if (portrait)
	{
		output_logo_only(distro_str);
		main_text_output(detected_arr, detected_arr_names);
	}
	else if (logo)
		main_ascii_output(detected_arr, detected_arr_names);
	else
		main_text_output(detected_arr, detected_arr_names);

	if (screenshot)
		take_screenshot(verbose);

	return EXIT_SUCCESS;
}
