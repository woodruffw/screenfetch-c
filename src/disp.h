/*	disp.h
	Author: William Woodruff
	-------------

	Function prototypes for disp.c and macros for error/verbose outut.
	Like the rest of screenfetch-c, this file is licensed under the MIT license.
	You should have received a copy of it with this code.
*/

#ifndef SCREENFETCH_C_DISP_H
#define SCREENFETCH_C_DISP_H

#include "colors.h"

void display_version(void);
void display_help(void);

#define ERROR_OUT(str1, str2) (fprintf(stderr, TWHT "[[ " TLRD "!" TWHT " ]] " TNRM "%s%s\n", str1, str2))
#define VERBOSE_OUT(str1, str2) (fprintf(stdout, TLRD ":: " TNRM "%s%s\n", str1, str2))

#endif /* SCREENFETCH_C_DISP_H */