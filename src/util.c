/*	util.c
 *	Author: William Woodruff
 *	-------------
 *
 *	Utility functions used by screenfetch-c.
 *	Like the rest of screenfetch-c, this file is licensed under the MIT license.
 */

/* standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

/* program includes */
#include "disp.h"
#include "misc.h"
#include "error_flag.h"

#if defined(__CYGWIN__)
	#include <Windows.h>
#endif

/*	safe_strncpy
	calls strncpy with the given params, then inserts a terminating NULL
	returns a pointer to a string containing the copied data
*/
char *safe_strncpy(char *destination, const char *source, size_t num)
{
	char *ret = strncpy(destination, source, num);
	ret[num - 1] = '\0';
	return ret;
}

/*	split_uptime
	splits param uptime into individual time-units
	argument long uptime: the uptime, in seconds, to be split
	arguments int *secs..*days: pointers to ints for storing the uptime
	--
	CAVEAT: uptime MUST be in seconds
	--
*/
void split_uptime(long uptime, int *secs, int *mins, int *hrs, int *days)
{
	*secs = (int) uptime % 60;
	*mins = (int) (uptime / 60) % 60;
	*hrs = (int) (uptime / 3600) % 24;
	*days = (int) (uptime / 86400);

	return;
}

/*	take_screenshot
	takes a screenshot and saves it to $HOME/screenfetch_screenshot.png
*/
void take_screenshot(bool verbose)
{
#if !defined(__CYGWIN__)
	int call_status = 1;
	char file_loc[MAX_STRLEN];
#endif

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

#if defined(__CYGWIN__)
	/* terrible hack, the printscreen key is simulated */
	keybd_event(VK_SNAPSHOT, 0, 0, 0);

	if (verbose)
		VERBOSE_OUT("Screenshot has been saved to the clipboard.", "");

	/* NOT FINSISHED - HBITMAP needs to be saved
	HDC screen_dc = GetDC(NULL);
	HDC mem_dc = CreateCompatibleDC(screen_dc);

	int horiz = GetDeviceCaps(screen_dc, HORZRES);
	int vert = GetDeviceCaps(screen_dc, VERTRES);

	HBITMAP bitmap = CreateCompatibleBitmap(screen_dc, horiz, vert);
	HBITMAP old_bitmap = SelectObject(mem_dc, bitmap);

	BitBlt(mem_dc, 0, 0, horiz, vert, screen_dc, 0, 0, SRCCOPY);
	bitmap = SelectObject(mem_dc, old_bitmap);

	DeleteDC(screen_dc);
	DeleteDC(mem_dc);
	*/
#elif defined(__APPLE__) && defined(__MACH__)
		call_status = system("screencapture -x ~/screenfetch_screenshot.png 2> /dev/null");
#else
		call_status = system("scrot ~/screenfetch_screenshot.png 2> /dev/null");
#endif

#if !defined(__CYGWIN__)
	safe_strncpy(file_loc, getenv("HOME"), MAX_STRLEN);
	strncat(file_loc, "/screenfetch_screenshot.png", MAX_STRLEN);

	if (FILE_EXISTS(file_loc) && verbose)
	{
		VERBOSE_OUT("Screenshot successfully saved.", "");
	}
	else if (call_status && error)
	{
		ERR_REPORT("Problem saving screenshot.");
	}
#endif

	return;
}

/*  manual_input
	generates (or reads) the ~/.screenfetchc file based upon user input
	returns an int indicating status (SUCCESS or FAILURE)
*/
int manual_input(char **data, bool verbose)
{
	FILE *config_file;
	char config_file_loc[MAX_STRLEN];

	safe_strncpy(config_file_loc, getenv("HOME"), MAX_STRLEN);
	strncat(config_file_loc, "/.screenfetchc", MAX_STRLEN);

	if (!FILE_EXISTS(config_file_loc))
	{
		#if defined(__CYGWIN__)
			printf("%s\n", TBLU "WARNING: There is currenly a bug involving manual mode on Windows." TNRM);
			printf("%s\n", TBLU "Only continue if you are ABSOLUTELY sure." TNRM);
		#endif

		printf("%s\n", "This appears to be your first time running screenfetch-c in manual mode.");
		printf("%s", "Would you like to continue? (y/n) ");

		char choice = getchar();
		getchar(); /* flush the newline */

		if (choice == 'y' || choice == 'Y')
		{
			char input[MAX_STRLEN];
			config_file = fopen(config_file_loc, "w");

			printf("%s\n", "We are now going to begin the manual mode input process.");
			printf("%s\n", "Please enter exactly what is asked for.");
			printf("%s\n", "If you are unsure about format, please consult the manpage.");

			printf("%s", "Please enter the name of your distribution/OS: ");
			fgets(input, MAX_STRLEN, stdin);
			fprintf(config_file, "%s", input);

			printf("%s", "Please enter your architecture: ");
			fgets(input, MAX_STRLEN, stdin);
			fprintf(config_file, "%s", input);

			printf("%s", "Please enter your username@hostname: ");
			fgets(input, MAX_STRLEN, stdin);
			fprintf(config_file, "%s", input);

			printf("%s", "Please enter your kernel: ");
			fgets(input, MAX_STRLEN, stdin);
			fprintf(config_file, "%s", input);

			printf("%s", "Please enter your CPU name: ");
			fgets(input, MAX_STRLEN, stdin);
			fprintf(config_file, "%s", input);

			printf("%s", "Please enter your GPU name: ");
			fgets(input, MAX_STRLEN, stdin);
			fprintf(config_file, "%s", input);

			printf("%s", "Please enter your shell name and version: ");
			fgets(input, MAX_STRLEN, stdin);
			fprintf(config_file, "%s", input);

			printf("%s", "Please enter your monitor resolution: ");
			fgets(input, MAX_STRLEN, stdin);
			fprintf(config_file, "%s", input);

			printf("%s", "Please enter your DE name: ");
			fgets(input, MAX_STRLEN, stdin);
			fprintf(config_file, "%s", input);

			printf("%s", "Please enter your WM name: ");
			fgets(input, MAX_STRLEN, stdin);
			fprintf(config_file, "%s", input);

			printf("%s", "Please enter your WM Theme name: ");
			fgets(input, MAX_STRLEN, stdin);
			fprintf(config_file, "%s", input);

			printf("%s", "Please enter any GTK info: ");
			fgets(input, MAX_STRLEN, stdin);
			fprintf(config_file, "%s", input);

			printf("%s\n", "That concludes the manual input.");
			printf("%s\n", "From now on, screenfetch-c will use this information when called with -m.");

			fclose(config_file);
		}
		else
		{
			printf("%s\n", "Exiting manual mode and screenfetch-c.");
			printf("%s\n", "If you wish to run screenfetch-c normally, do not use the -m (--manual) flag next time.");
		}
		
		return EXIT_FAILURE;
	}
	else
	{
		if (verbose)
			VERBOSE_OUT("Found config file. Reading...", "");

		config_file = fopen(config_file_loc, "r");

		/*	store anything without a newline it it,
			then swallow any whitespace characters (" ").
		 */
		fscanf(config_file, "%[^\n] ", data[1]);
		fscanf(config_file, "%[^\n] ", data[3]);
		fscanf(config_file, "%[^\n] ", data[0]);
		fscanf(config_file, "%[^\n] ", data[2]);
		fscanf(config_file, "%[^\n] ", data[4]);
		fscanf(config_file, "%[^\n] ", data[5]);
		fscanf(config_file, "%[^\n] ", data[6]);
		fscanf(config_file, "%[^\n] ", data[11]);
		fscanf(config_file, "%[^\n] ", data[12]);
		fscanf(config_file, "%[^\n] ", data[13]);
		fscanf(config_file, "%[^\n] ", data[14]);
		fscanf(config_file, "%[^\n] ", data[15]);

		fclose(config_file);

		return EXIT_SUCCESS;
	}
}
