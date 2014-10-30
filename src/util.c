/*	util.c
	Author: William Woodruff
	-------------

	Utility functions used by screenfetch-c.
	Like the rest of screenfetch-c, this file is licensed under the MIT license.
	You should have received a copy of it with this code.
*/

#include <string.h>

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

