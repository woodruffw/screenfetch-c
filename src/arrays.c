/*	arrays.c
**	Authors: Aaron Caffrey, William Woodruff
**	-------------
**
**	The strings used by screenfetch-c to store system information are
**	initialized in this file.
*/


#include "misc.h"
#include "colors.h"

char given_distro_str[MAX_STRLEN] = "Unknown";
char distro_str[MAX_STRLEN] = "Unknown";
char host_str[MAX_STRLEN] = "Unknown";
char kernel_str[MAX_STRLEN] = "Unknown";
char uptime_str[MAX_STRLEN] = "Unknown";
char pkgs_str[MAX_STRLEN] = "Unknown";
char cpu_str[MAX_STRLEN] = "Unknown";
char gpu_str[MAX_STRLEN] = "Unknown";
char disk_str[MAX_STRLEN] = "Unknown";
char mem_str[MAX_STRLEN] = "Unknown";
char shell_str[MAX_STRLEN] = "Unknown";
char res_str[MAX_STRLEN] = "Unknown";
char de_str[MAX_STRLEN] = "Unknown";
char wm_str[MAX_STRLEN] = "Unknown";
char wm_theme_str[MAX_STRLEN] = "Unknown";
char gtk_str[MAX_STRLEN] = "Unknown";
char icon_str[MAX_STRLEN] = "Unknown";
char font_str[MAX_STRLEN] = "Unknown";

char host_color[MAX_STRLEN] = TNRM;

char *detected_arr[17] =
{
	host_str,
	distro_str,
	kernel_str,
	cpu_str,
	gpu_str,
	shell_str,
	pkgs_str,
	disk_str,
	mem_str,
	uptime_str,
	res_str,
	de_str,
	wm_str,
	wm_theme_str,
	gtk_str,
	icon_str,
	font_str
};

char *detected_arr_names[17] =
{
	"",
	"OS: ",
	"Kernel: ",
	"CPU: ",
	"GPU: ",
	"Shell: ",
	"Packages: ",
	"Disk: ",
	"Memory: ",
	"Uptime: ",
	"Resolution: ",
	"DE: ",
	"WM: ",
	"WM Theme: ",
	"GTK: ",
	"Icon Theme: ",
	"Font: "
};
