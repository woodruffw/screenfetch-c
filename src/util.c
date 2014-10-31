/*	util.c
	Author: William Woodruff
	-------------

	Utility functions used by screenfetch-c.
	Like the rest of screenfetch-c, this file is licensed under the MIT license.
	You should have received a copy of it with this code.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
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
