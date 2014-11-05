/*	util.c
	Author: William Woodruff
	-------------

	Utility functions used by screenfetch-c.
	Like the rest of screenfetch-c, this file is licensed under the MIT license.
	You should have received a copy of it with this code.
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

#if defined(__CYGWIN__)
	#include <Windows.h>
#endif

/*	safe_strncpy
	calls strncpy with the given params, then inserts a null char at the last position
	returns a pointer to a string containing the copied data (same as destination)
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
	arguments int *secs...*days: pointers to ints where the split uptime will be stored
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
	takes a screenshot and saves it to $HOME/screenfetch_screenshot.jpg
*/
void take_screenshot(bool verbose, bool error)
{
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
			system("screencapture -x ~/screenfetch_screenshot.png 2> /dev/null");
	#else
			system("scrot ~/screenfetch_screenshot.png 2> /dev/null");
	#endif

	#if !defined(__CYGWIN__)
		/* change this to getpwuid() */
		char file_loc[MAX_STRLEN];
		safe_strncpy(file_loc, getenv("HOME"), MAX_STRLEN);
		strncat(file_loc, "/screenfetch_screenshot.png", MAX_STRLEN);

		if (FILE_EXISTS(file_loc) && verbose)
		{
			VERBOSE_OUT("Screenshot successfully saved.", "");
		}
		else if (verbose)
		{
			ERROR_OUT("Error: ", "Problem saving screenshot.");
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
			config_file = fopen(config_file_loc, "w");

			printf("%s\n", "We are now going to begin the manual mode input process.");
			printf("%s\n", "Please enter exactly what is asked for.");
			printf("%s\n", "If you are unsure about format, please consult the manpage.");

			printf("%s", "Please enter the name of your distribution/OS: ");
			fgets(data[1], MAX_STRLEN, stdin);
			fputs(data[1], config_file);

			printf("%s", "Please enter your architecture: ");
			fgets(data[3], MAX_STRLEN, stdin);
			fputs(data[3], config_file);

			printf("%s", "Please enter your username@hostname: ");
			fgets(data[0], MAX_STRLEN, stdin);
			fputs(data[0], config_file);

			printf("%s", "Please enter your kernel: ");
			fgets(data[2], MAX_STRLEN, stdin);
			fputs(data[2], config_file);

			printf("%s", "Please enter your CPU name: ");
			fgets(data[4], MAX_STRLEN, stdin);
			fputs(data[4], config_file);

			printf("%s", "Please enter your GPU name: ");
			fgets(data[5], MAX_STRLEN, stdin);
			fputs(data[5], config_file);

			printf("%s", "Please enter your shell name and version: ");
			fgets(data[6], MAX_STRLEN, stdin);
			fputs(data[6], config_file);

			printf("%s", "Please enter your monitor resolution: ");
			fgets(data[11], MAX_STRLEN, stdin);
			fputs(data[11], config_file);

			printf("%s", "Please enter your DE name: ");
			fgets(data[12], MAX_STRLEN, stdin);
			fputs(data[12], config_file);

			printf("%s", "Please enter your WM name: ");
			fgets(data[13], MAX_STRLEN, stdin);
			fputs(data[13], config_file);

			printf("%s", "Please enter your WM Theme name: ");
			fgets(data[14], MAX_STRLEN, stdin);
			fputs(data[14], config_file);

			printf("%s", "Please enter any GTK info: ");
			fgets(data[15], MAX_STRLEN, stdin);
			fputs(data[15], config_file);

			printf("%s\n", "That concludes the manual input.");
			printf("%s\n", "From now on, screenfetch-c will use this information when called with -m.");

			fclose(config_file);

			/* i am deeply ashamed of this solution */
			data[1][strlen(data[1]) - 1] = '\0';
			data[3][strlen(data[3]) - 1] = '\0';
			data[0][strlen(data[0]) - 1] = '\0';
			data[2][strlen(data[2]) - 1] = '\0';
			data[4][strlen(data[4]) - 1] = '\0';
			data[5][strlen(data[5]) - 1] = '\0';
			data[6][strlen(data[6]) - 1] = '\0';
			data[11][strlen(data[11]) - 1] = '\0';
			data[12][strlen(data[12]) - 1] = '\0';
			data[13][strlen(data[13]) - 1] = '\0';
			data[14][strlen(data[14]) - 1] = '\0';
			data[15][strlen(data[15]) - 1] = '\0';

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

		fgets(data[1], MAX_STRLEN, config_file);
		fgets(data[3], MAX_STRLEN, config_file);
		fgets(data[0], MAX_STRLEN, config_file);
		fgets(data[2], MAX_STRLEN, config_file);
		fgets(data[4], MAX_STRLEN, config_file);
		fgets(data[5], MAX_STRLEN, config_file);
		fgets(data[6], MAX_STRLEN, config_file);
		fgets(data[11], MAX_STRLEN, config_file);
		fgets(data[12], MAX_STRLEN, config_file);
		fgets(data[13], MAX_STRLEN, config_file);
		fgets(data[14], MAX_STRLEN, config_file);
		fgets(data[15], MAX_STRLEN, config_file);

		fclose(config_file);

		/* i am deeply ashamed of this solution */
		data[1][strlen(data[1]) - 1] = '\0';
		data[3][strlen(data[3]) - 1] = '\0';
		data[0][strlen(data[0]) - 1] = '\0';
		data[2][strlen(data[2]) - 1] = '\0';
		data[4][strlen(data[4]) - 1] = '\0';
		data[5][strlen(data[5]) - 1] = '\0';
		data[6][strlen(data[6]) - 1] = '\0';
		data[11][strlen(data[11]) - 1] = '\0';
		data[12][strlen(data[12]) - 1] = '\0';
		data[13][strlen(data[13]) - 1] = '\0';
		data[14][strlen(data[14]) - 1] = '\0';
		data[15][strlen(data[15]) - 1] = '\0';

		return EXIT_SUCCESS;
	}
}
