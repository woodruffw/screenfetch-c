/*	util.c
**	Author: William Woodruff
**	-------------
**
**	Utility functions used by screenfetch-c.
**	Like the rest of screenfetch-c, this file is licensed under the MIT license.
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
void split_uptime(long uptime, unsigned int *secs, unsigned int *mins,
					unsigned int *hrs, unsigned int *days)
{
	*secs = (unsigned int) uptime % 60;
	*mins = (unsigned int) (uptime / 60) % 60;
	*hrs = (unsigned int) (uptime / 3600) % 24;
	*days = (unsigned int) (uptime / 86400);

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
