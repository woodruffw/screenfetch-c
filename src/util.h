/*	util.h
 *	Author: William Woodruff
 *	-------------
 *
 *	Function prototypes for util.c.
 *	Like the rest of screenfetch-c, this file is licensed under the MIT license.
 */

#ifndef SCREENFETCH_C_UTIL_H
#define SCREENFETCH_C_UTIL_H

char *safe_strncpy(char *destination, const char *source, size_t num);
void split_uptime(long uptime, int *secs, int *mins, int *hrs, int *days);
void take_screenshot(bool verbose);
int manual_input(char **data, bool verbose);

#endif /* SCREENFETCH_C_UTIL_H */
