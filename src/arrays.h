/*	arrays.h
**	Authors: Aaron Caffrey, William Woodruff
**	-------------
**
**	The strings used by screenfetch-c to store system information are
**	externalized in this file.
*/

#ifndef SCREENFETCH_C_ARRAYS_H
#define SCREENFETCH_C_ARRAYS_H

#include "misc.h"

extern char given_distro_str[MAX_STRLEN];
extern char distro_str[MAX_STRLEN];
extern char host_str[MAX_STRLEN];
extern char kernel_str[MAX_STRLEN];
extern char uptime_str[MAX_STRLEN];
extern char pkgs_str[MAX_STRLEN];
extern char cpu_str[MAX_STRLEN];
extern char gpu_str[MAX_STRLEN];
extern char disk_str[MAX_STRLEN];
extern char mem_str[MAX_STRLEN];
extern char shell_str[MAX_STRLEN];
extern char res_str[MAX_STRLEN];
extern char de_str[MAX_STRLEN];
extern char wm_str[MAX_STRLEN];
extern char wm_theme_str[MAX_STRLEN];
extern char gtk_str[MAX_STRLEN];
extern char icon_str[MAX_STRLEN];
extern char font_str[MAX_STRLEN];

extern char host_color[10];

extern char *detected_arr[17];
extern char *detected_arr_names[17];

#endif /* SCREENFETCH_C_ARRAYS_H */
