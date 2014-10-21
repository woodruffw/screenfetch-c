/*	screenfetch-c.h
	Author: William Woodruff
	-------------
	
	The functions and macros used in screenfetch-c.c are defined/prototyped here.
	Like the rest of screenfetch-c, this file is licensed under the MIT license.
	You should have received a copy of it with this code.
*/

#ifndef SCREENFETCH_C_H
#define SCREENFETCH_C_H

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
	#include <glob.h>
	#include <Availability.h>
	#if __MAC_OS_X_VERSION_MIN_REQUIRED <= 1070
		#include <CoreServices/CoreServices.h> /* for Gestalt */
	#endif
#elif defined(__linux__)
	#define OS LINUX
	#include <sys/sysinfo.h>
	#include <sys/utsname.h>
	#include <X11/Xlib.h>
	#include <glob.h>
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
void take_screenshot(void);

#endif /* SCREENFETCH_C_H */
