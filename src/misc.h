/*	misc.h
	Author: William Woodruff

	Miscellaneous macros used in screenfetch-c.
	Like the rest of screenfetch-c, this file is licensed under the MIT license.
	You should have received a copy of it with this code.
*/

#ifndef SCREENFETCH_C_MISC_H
#define SCREENFETCH_C_MISC_H

#include "colors.h"

#define KB 1024
#define MB 1048576
#define MAX_STRLEN 128
#define SET_DISTRO(str) (safe_strncpy(distro_str, str, MAX_STRLEN))
#define STRCMP(x, y) (!strcmp(x, y))
#define FILE_EXISTS(file) (!access(file, F_OK))

#endif /* SCREENFETCH_C_MISC_H */