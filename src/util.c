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

#if defined(__CYGWIN__) || defined(__MSYS__) || defined(__MINGW32__)
#include <windows.h>
#include "plat/win32/bitmap.h"
#endif

/*	safe_strncpy
  calls strncpy with the given params, then inserts a terminating NULL
  returns a pointer to a string containing the copied data
*/
char *safe_strncpy(char *destination, const char *source, size_t num) {
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
void split_uptime(long uptime, unsigned int *secs, unsigned int *mins, unsigned int *hrs,
                  unsigned int *days) {
  *secs = (unsigned int)uptime % 60;
  *mins = (unsigned int)(uptime / 60) % 60;
  *hrs = (unsigned int)(uptime / 3600) % 24;
  *days = (unsigned int)(uptime / 86400);

  return;
}

/*	take_screenshot
  takes a screenshot and saves it to $HOME/screenfetch_screenshot.png
*/
void take_screenshot(bool verbose) {
#if !defined(__CYGWIN__) && !defined(__MSYS__) && !defined(__MINGW32__)
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

#if defined(__CYGWIN__) || defined(__MSYS__) || defined(__MINGW32__)
  HDC screen_dc = GetDC(NULL);
  HDC mem_dc = CreateCompatibleDC(screen_dc);

  int horiz = GetDeviceCaps(screen_dc, HORZRES);
  int vert = GetDeviceCaps(screen_dc, VERTRES);

  HBITMAP bitmap = CreateCompatibleBitmap(screen_dc, horiz, vert);
  HBITMAP old_bitmap = SelectObject(mem_dc, bitmap);

  BitBlt(mem_dc, 0, 0, horiz, vert, screen_dc, 0, 0, SRCCOPY);
  bitmap = SelectObject(mem_dc, old_bitmap);

  if (createBitmapFile("screenfetch_screenshot.bmp", bitmap, mem_dc) == 0) {
    VERBOSE_OUT("Screenshot successfully saved.", "");
  } else {
    ERR_REPORT("Problem saving screenshot.");
  }

  DeleteDC(screen_dc);
  DeleteDC(mem_dc);

#elif defined(__APPLE__) && defined(__MACH__)
  call_status = system("screencapture -x ~/screenfetch_screenshot.png 2> /dev/null");
#else
  call_status = system("scrot ~/screenfetch_screenshot.png 2> /dev/null");
#endif

#if !defined(__CYGWIN__) && !defined(__MSYS__) && !defined(__MINGW32__)
  safe_strncpy(file_loc, getenv("HOME"), MAX_STRLEN);
  strncat(file_loc, "/screenfetch_screenshot.png", MAX_STRLEN);

  if (FILE_EXISTS(file_loc) && verbose) {
    VERBOSE_OUT("Screenshot successfully saved.", "");
  } else if (call_status && error) {
    ERR_REPORT("Problem saving screenshot.");
  }
#endif

  return;
}

/*	returns 1 if command is in PATH, otherwise 0
 */
#ifdef __linux
int command_in_path(const char *command) {
  char *env, *env_copy, *str, *token;
  char *file = NULL;
  int j, len, rv = 0;

  env = getenv("PATH");

  if (!command || !env) {
    return 0;
  }

  env_copy = strdup(env);

  /*	though not really needed, stop after 9999 iterations
    to prevent any potential endless loop
  */
  for (j = 0, str = env_copy; j < 9999; ++j, str = NULL) {
    len = strlen(command);
    token = strtok(str, ":");

    if (token == NULL || len == 0) {
      rv = 0;
      break;
    }

    file = malloc(strlen(token) + len + 2);
    sprintf(file, "%s/%s", token, command);

    if (FILE_EXISTS(file)) {
      rv = 1;
      free(file);
      break;
    }
    free(file);
  }

  free(env_copy);
  return rv;
}
#endif /* __linux */
