/*  screenfetch-c.h
	Author: William Woodruff
	-------------
	
	The functions and macros used in screenfetch-c.c are defined/prototyped here.
	Like the rest of screenfetch-c.c, this file is licensed under the MIT license.
	You should have received a copy of it with this code.
*/

#ifndef SCREENFETCH_C_H
#define SCREENFETCH_C_H

/* version macros */
#define SCREENFETCH_C_VERSION_MAJOR "1"
#define SCREENFETCH_C_VERSION_MINOR "2"
#define SCREENFETCH_C_VERSION_RELEASE "7"

/* a number is assigned to each OS or OS family */
#define UNKNOWN 0
#define CYGWIN 1
#define OSX 2
#define LINUX 3
#define FREEBSD 4
#define NETBSD 5
#define OPENBSD 6
#define DFBSD 7
#define SOLARIS 8

/* quick macro for when all BSDs have the same function syntax */
#define ISBSD() ((OS >= 4 && OS <= 7) ? true : false)

#if defined(__CYGWIN__)
	#define OS CYGWIN
	extern FILE* popen(const char* command, const char* type);
	extern int pclose(FILE* stream);
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
#elif defined(__APPLE__) && defined(__MACH__)
	#define OS OSX
	#include <sys/utsname.h>
	#include <time.h>
	#include <Availability.h>
	#if __MAC_OS_X_VERSION_MIN_REQUIRED <= 1070
		#include <CoreServices/CoreServices.h> /* for Gestalt */
	#endif
#elif defined(__linux__)
	#define OS LINUX
	#include <sys/sysinfo.h>
	#include <sys/utsname.h>
	#include <X11/Xlib.h>
#elif defined(__FreeBSD__)
	#define OS FREEBSD
	#include <sys/utsname.h>
	#include <time.h>
#elif defined(__NetBSD__)
	#define OS NETBSD
	#include <sys/utsname.h>
#elif defined(__OpenBSD__)
	#define OS OPENBSD
	#include <sys/utsname.h>
	#include <time.h>
#elif defined(__DragonFly__)
	#define OS DFBSD
	#include <sys/utsname.h>
	#include <time.h>
#elif defined(__sun__)
	#define OS SOLARIS
	#include <utmpx.h>
	#include <time.h>
	#include <sys/types.h>
	#include <sys/param.h>
	#include <sys/utsname.h>
	#include <X11/Xlib.h>
#else 
	#define OS UNKNOWN
#endif

/* color/fmt definitions */
#define TNRM "\x1B[0m" /* normal */
#define TBLK "\x1B[30m" /* black */
#define TRED "\x1B[31m" /* red */
#define TGRN "\x1B[32m" /* green */
#define TBRN "\x1B[33m" /* brown */
#define TBLU "\x1B[34m" /* blue */
#define TPUR "\x1B[35m" /* purple */
#define TCYN "\x1B[36m" /* cyan */
#define TLGY "\x1B[37m" /* light gray */
#define TDGY "\x1B[1;30m" /* dark gray */
#define TLRD "\x1B[1;31m" /* light red */
#define TLGN "\x1B[1;32m" /* light green */
#define TYLW "\x1B[1;33m" /* yellow */
#define TLBL "\x1B[1;34m" /* light blue */
#define TLPR "\x1B[1;35m" /* light purple */
#define TLCY "\x1B[1;36m" /* light cyan */
#define TWHT "\x1B[1;37m" /* white */

/* other definitions, use with caution (not type safe) */
#define KB 1024
#define MB 1048576
#define MAX_STRLEN 128
#define SET_DISTRO(str) (safe_strncpy(distro_str, str, MAX_STRLEN))
#define STRCMP(x, y) (!strcmp(x, y))
#define FILE_EXISTS(file) (!access(file, F_OK))
#define ERROR_OUT(str1, str2) (fprintf(stderr, TWHT "[[ " TLRD "!" TWHT " ]] " TNRM "%s%s\n", str1, str2))
#define VERBOSE_OUT(str1, str2) (fprintf(stdout, TLRD ":: " TNRM "%s%s\n", str1, str2))

/* screenfetch detection function definitions */
void detect_distro(char* str);
void detect_arch(char* str);
void detect_host(char* str);
void detect_kernel(char* str);
void detect_uptime(char* str);
void detect_pkgs(char* str);
void detect_cpu(char* str);
void detect_gpu(char* str);
void detect_disk(char* str);
void detect_mem(char* str);
void detect_shell(char* str);
void detect_res(char* str);
void detect_de(char* str);
void detect_wm(char* str);
void detect_wm_theme(char* str);
void detect_gtk(char* str);

/* other function definitions */
int manual_input(void);
void output_logo_only(char* distro);
void main_ascii_output(char* data[], char* data_names[]);
void main_text_output(char* data[], char* data_names[]);
char* safe_strncpy(char* destination, const char* source, size_t num); 
void split_uptime(long uptime, int* secs, int* mins, int* hrs, int* days);
void display_version(void);
void display_help(void);
void take_screenshot(void);

#endif /* SCREENFETCH_C_H */

/* EOF */
