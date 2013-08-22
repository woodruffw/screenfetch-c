/* screenFetch-c
	A rewrite of screenFetch.sh 3.0.5 in C.
	This is primarily an experiment borne out of an awareness of the slow execution time on the 
	screenfetch-dev.sh script. 
	Hopefully this port will execute faster, although it's more for self education than anything else.

	NOTES:
	I used many of Brett Bohnenkamper's awk/sed/grep/etc oneliners in my popen() calls, 
	although some were modified to change/improve the output.

	PLANNED IMPROVEMENTS:
	Add libcpuid to decrease reliance on shell utilities.

	TODO:
	Figure out how to include ASCII output (i.e. logos)
	Figure out shell version/DE/WM/WM theme/GTK/Android detection

	**From the original author:**

	--

	Copyright (c) 2010-2012 Brett Bohnenkamper < kittykatt AT archlinux DOT us >

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software
	and associated documentation files (the "Software"), to deal in the Software without restriction,
	including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
	and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
	subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
	WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
	COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
	ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

	Yes, I do realize some of this is horribly ugly coding. Any ideas/suggestions would be
	appreciated by emailing me or by stopping by http://github.com/KittyKatt/screenFetch . You
	could also drop in on my IRC network, SilverIRC, at irc://kittykatt.silverirc.com:6667/screenFetch
	to put forth suggestions/ideas. Thank you.

	--

	I hereby regrant this version of screenFetch under the same MIT license.
	If you have any questions, please contact me at woodrufw@bxscience.edu.
*/

#include <stdio.h> //for a medley of I/O operations, including popen
#include <stdlib.h> //for getenv, etc
#include <stdbool.h> //for the bool type
#include <string.h> //for strcmp, strncpy, etc.
#include <unistd.h> //for sleep, getopt

//OS definitions - allows linux machines to use sysinfo while maintaining compatability w/ OSX + Cygwin + BSD

//a number is assigned to each OS
#define UNKNOWN 0
#define CYGWIN 1
#define OSX 2
#define LINUX 3
#define FREEBSD 4
#define NETBSD 5
#define OPENBSD 6
#define DFBSD 7

//quick macro for when all BSDs have the same syntax
#define ISBSD() ((OS >= 4 && OS <= 7) ? true : false)

#ifdef __CYGWIN__
	#define OS CYGWIN
	//popen and pclose are implicit on Cygwin, so define them here:
	FILE* popen(const char* command, const char* type);
	int pclose(FILE* stream);
#elif defined __APPLE__&&__MACH__
	#define OS OSX
#elif defined __linux__
	#define OS LINUX
	#include <sys/sysinfo.h>
#elif defined __FreeBSD__
	#define OS FREEBSD
#elif defined __NetBSD__
	#define OS NETBSD
#elif defined __OpenBSD__
	#define OS OPENBSD
#elif defined __DragonFly__
	#define OS DFBSD
#else 
	#define OS UNKNOWN
#endif

//end OS definitions


//text definitions
#define TNRM "\x1B[0m"
#define TBLD "\x1B[1m"
#define TULN "\x1B[4m"
#define TBLK "\x1B[30m"
#define TRED "\x1B[31m"
#define TGRN "\x1B[32m"
#define TBRN "\x1B[33m"
#define TBLU "\x1B[34m"
#define TPUR "\x1B[35m"
#define TCYN "\x1B[36m"
#define TLGY "\x1B[37m"
#define TDGY "\x1B[1;30m"
#define TLRD "\x1B[1;31m"
#define TLGN "\x1B[1;32m"
#define TYLW "\x1B[1;33m"
#define TLBL "\x1B[1;34m"
#define TLPR "\x1B[1;35m"
#define TLCY "\x1B[1;36m"
#define TWHT "\x1B[1;37m"

//other definitions, use with caution (not type safe)
#define MAX_STRLEN 128
#define SET_VERBOSE(flag) (verbose = flag)
#define SET_ERROR(flag) (error = flag)
#define SET_SCREENSHOT(flag) (screenshot = flag)
#define SET_DISTRO(distro) (safe_strncpy(distro_str, distro, MAX_STRLEN))
#define STRCMP(x, y) (!strcmp(x, y))
#define ERROR_OUT(str1, str2) (fprintf(stderr, TWHT "[[ " TLRD "!" TWHT " ]] " TNRM "%s%s\n", str1, str2))
#define VERBOSE_OUT(str1, str2) (fprintf(stdout, TLRD ":: " TNRM "%s%s\n", str1, str2))


//screenfetch detection function definitions
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
void detect_shell_version(char* str);
void detect_res(char* str);
void detect_de(char* str);
void detect_wm(char* str);
void detect_wm_theme(char* str);
void detect_gtk(char* str);

//other function definitions
void take_screenshot(void);
void display_version(void);
void display_help(void);
void split_uptime(float uptime, int* secs, int* mins, int* hrs, int* days);
char* safe_strncpy(char* destination, const char* source, size_t num);

//string definitions
static char distro_str[MAX_STRLEN];
static char arch_str[MAX_STRLEN];
static char host_str[MAX_STRLEN];
static char kernel_str[MAX_STRLEN];
static char uptime_str[MAX_STRLEN];
static char pkgs_str[MAX_STRLEN];
static char cpu_str[MAX_STRLEN];
static char gpu_str[MAX_STRLEN];
static char disk_str[MAX_STRLEN];
static char mem_str[MAX_STRLEN];
static char shell_str[MAX_STRLEN];
static char shell_version_str[MAX_STRLEN];
static char res_str[MAX_STRLEN];
static char de_str[MAX_STRLEN];
static char wm_str[MAX_STRLEN];
static char wm_theme_str[MAX_STRLEN];
static char gtk_str[MAX_STRLEN];

//other definitions
bool error = true;
bool verbose = false;
bool screenshot = false;
bool ascii = false;


int main(int argc, char** argv)
{
	char* opt_str = NULL;

	char c;

	while ((c = getopt(argc, argv, "vnNsS:D:A:EVh")) != -1)
	{
		switch (c)
		{
			case 'v':
				SET_VERBOSE(true);
				break;
			case 'n':
				//something like SET_LOGO(false);
				break;
			case 'N':
				//something like SET_COLOR(false);
				break;
			case 's':
				SET_SCREENSHOT(true);
				break;
			case 'S':
				SET_SCREENSHOT(true);
				opt_str = optarg;
				//do something with opt_str
				break;
			case 'D':
				SET_DISTRO(optarg);
				break;
			case 'A':
				//something like SET_DISTRO_ART(optarg);
				break;
			case 'E':
				SET_ERROR(false);
				break;
			case 'V':
				display_version();
				return EXIT_SUCCESS;
			case 'h':
				display_help();
				return EXIT_SUCCESS;
			case '?':
				if (optopt == 'S' || optopt == 'D' || optopt == 'A')
					ERROR_OUT("Error: One or more tripped flag(s) requires an argument.", "");
				else
					ERROR_OUT("Error: Unknown option or option character.", "");
				return EXIT_FAILURE;
		}
	}

	//copy 'Unknown' to each string and append a null character
	safe_strncpy(distro_str, "Unknown", MAX_STRLEN);
	safe_strncpy(arch_str, "Unknown", MAX_STRLEN);
	safe_strncpy(host_str, "Unknown", MAX_STRLEN);
	safe_strncpy(kernel_str, "Unknown", MAX_STRLEN);
	safe_strncpy(uptime_str, "Unknown", MAX_STRLEN);
	safe_strncpy(pkgs_str, "Unknown", MAX_STRLEN);
	safe_strncpy(cpu_str, "Unknown", MAX_STRLEN);
	safe_strncpy(gpu_str, "Unknown", MAX_STRLEN);
	safe_strncpy(disk_str, "Unknown", MAX_STRLEN);
	safe_strncpy(mem_str, "Unknown", MAX_STRLEN);
	safe_strncpy(shell_str, "Unknown", MAX_STRLEN);
	safe_strncpy(shell_version_str, "Unknown", MAX_STRLEN);
	safe_strncpy(res_str, "Unknown", MAX_STRLEN);
	safe_strncpy(de_str, "Unknown", MAX_STRLEN);
	safe_strncpy(wm_str, "Unknown", MAX_STRLEN);
	safe_strncpy(wm_theme_str, "Unknown", MAX_STRLEN);
	safe_strncpy(gtk_str, "Unknown", MAX_STRLEN);

	//each string is filled by its respective function (optional return)
	detect_distro(distro_str);
	detect_arch(arch_str);
	detect_host(host_str);
	detect_kernel(kernel_str);
	detect_uptime(uptime_str);
	detect_pkgs(pkgs_str);
	detect_cpu(cpu_str);
	detect_gpu(gpu_str);
	detect_disk(disk_str);
	detect_mem(mem_str);
	detect_shell(shell_str);
	detect_shell_version(shell_version_str);
	detect_res(res_str);
	detect_de(de_str);
	detect_wm(wm_str);
	detect_wm_theme(wm_theme_str);
	detect_gtk(gtk_str);

	//ugly testing section
	printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n", distro_str, arch_str, host_str, kernel_str, uptime_str, pkgs_str, cpu_str, gpu_str, disk_str, mem_str, shell_str, shell_version_str, res_str, de_str, wm_str, wm_theme_str, gtk_str);

	//main_output();

	return EXIT_SUCCESS;
}

/*  **  BEGIN DETECTION FUNCTIONS  ** */

//detect_distro
//detects the computer's distribution (really only relevant on Linux)
//returns a string containing the distro name (may vary in format)
void detect_distro(char* str)
{
	FILE* distro_file;

	char distro_name_str[MAX_STRLEN];
	char distro_name_str_inc[MAX_STRLEN];

	if (OS == CYGWIN)
	{
		//uname -o?

		distro_file = popen("wmic os get name | head -2 | tail -1", "r");
		fgets(distro_name_str_inc, sizeof(distro_name_str_inc), distro_file);
		pclose(distro_file);

		//distro_file = popen("expr match " distro_name_str_inc " '\\(Microsoft Windows [A-Za-z0-9]\\+\\)'", "r");
		//fgets(distro_name_str, sizeof(distro_name_str), distro_file);
		//pclose(distro_file);

		//safe_strncpy(str, distro_name_str, MAX_STRLEN);
	}

	else if (OS == OSX)
	{
		safe_strncpy(str, "Mac OS X", MAX_STRLEN);

		//distro_file = popen("sw_vers | grep ProductVersion | tr -d 'ProductVersion: ", "r");
		//cat version onto str
		//pclose(distro_file);
	}

	else if (OS == LINUX)
	{
		distro_file = fopen("/etc/lsb-release", "r");

		if (distro_file != NULL)
		{
			//get and parse /etc/lsb-release
			fclose(distro_file);
		}
		else
		{
			distro_file = fopen("/proc/version", "r");

			if (distro_file != NULL)
			{
				//get and parse /proc/version
				fclose(distro_file);
			}
			else
			{
				distro_file = fopen("/etc/issue", "r");

				if (distro_file != NULL)
				{
					//get and parse /etc/issue
					fclose(distro_file);
				}
				else
				{
					distro_file = fopen("/etc/version", "r");

					if (distro_file != NULL)
					{
						//get and parse /etc/version
						fclose(distro_file);
					}
					else
					{
						safe_strncpy(str, "Linux", MAX_STRLEN);

						if (error)
						{
							ERROR_OUT("Error: Failed to detect specific Linux distro.", "");
						}
					}
				}
			}
		}
	}

	else if (ISBSD())
	{
		distro_file = popen("uname -sr", "r");
		fgets(str, sizeof(str), distro_file);
		pclose(distro_file);
	}

	if (verbose)
	{
		VERBOSE_OUT("Found distro as ", str);
	}

	return;
}

//detect_arch
//detects the computer's architecture
//returns a string containing the arch
void detect_arch(char* str)
{
	FILE* arch_file;

	if (OS == CYGWIN)
	{
		arch_file = popen("wmic os get OSArchitecture | head -2 | tail -1 | tr -d '\\r\\n '", "r");
		fgets(str, MAX_STRLEN, arch_file);
		pclose(arch_file);
	}

	else if (OS == OSX || OS == LINUX || ISBSD())
	{
		arch_file = popen("uname -m | tr -d '\\n'", "r");
		fgets(str, MAX_STRLEN, arch_file);
		pclose(arch_file);
	}

	if (verbose)
	{
		VERBOSE_OUT("Found system arch as ", str);
	}

	return;
}

//detect_host
//detects the computer's hostname and active user and formats them
//returns a string of format "user@hostname"
void detect_host(char* str)
{
	char* given_user;
	char given_host[MAX_STRLEN];

	given_user = getenv("USER");

	FILE* host_file = popen("hostname | tr -d '\\r\\n '", "r");
	fgets(given_host, MAX_STRLEN, host_file);
	pclose(host_file);

	//format str
	snprintf(str, MAX_STRLEN, "%s@%s", given_user, given_host);

	if (verbose)
	{
		VERBOSE_OUT("Found host as ", str);
	}

	return;
}

//detect_kernel
//detects the computer's kernel
//returns a string containing the kernel name, version, etc
void detect_kernel(char* str)
{
	FILE* kernel_file = popen("uname -srm | tr -d '\\r\\n'", "r");
	fgets(str, MAX_STRLEN, kernel_file);
	pclose(kernel_file);

	if (verbose)
	{
		VERBOSE_OUT("Found kenel as ", str);
	}

	return;
}

//detect_uptime
//detects the computer's uptime
//returns a string of format "<>d <>h <>m <>s", where <> is a number
void detect_uptime(char* str)
{
	FILE* uptime_file;

	long uptime;
	long now, boottime; //may or may not be used depending on OS
	int secs;
	int mins;
	int hrs;
	int days;

	if (OS == CYGWIN || NETBSD)
	{
		uptime_file = popen("cat /proc/uptime | cut -d ' ' -f 1", "r");
		fscanf(uptime_file, "%ld", &uptime);
		pclose(uptime_file);
	}

	else if (OS == OSX || OS == FREEBSD || OS == DFBSD)
	{
		uptime_file = popen("sysctl -n kern.boottime | cut -d \"=\" -f 2 | cut -d \",\" -f 1", "r");
		fscanf(uptime_file, "%ld", &boottime); //get boottime in secs
		pclose(uptime_file);

		uptime_file = popen("date +%s", "r");
		fscanf(uptime_file, "%ld", &now); //get current time in secs
		pclose(uptime_file);

		uptime = now - boottime;
	}

	else if (OS == LINUX)
	{
		#ifdef __linux__
		struct sysinfo si_upt;
		sysinfo(&si_upt);

		uptime = si_upt.uptime;
		#endif
	}

	else if (OS == OPENBSD)
	{
		uptime_file = popen("sysctl -n kern.boottime", "r");
		fscanf(uptime_file, "%ld", &boottime); //get current boottime in secs
		pclose(uptime_file);

		uptime_file = popen("date +%s", "r");
		fscanf(uptime_file, "%ld", &now); //get current time in secs
		pclose(uptime_file);

		uptime = now - boottime;
	}

	split_uptime(uptime, &secs, &mins, &hrs, &days);
	snprintf(str, MAX_STRLEN, "%dd %dh %dm %ds", days, hrs, mins, secs);

	if (verbose)
	{
		VERBOSE_OUT("Found uptime as ", str);
	}

	return;
}

//detect_pkgs
//detects the number of packages installed on the computer
//returns a string containing the number of packages
void detect_pkgs(char* str)
{
	FILE* pkgs_file;

	int packages = 0;

	if (OS == CYGWIN)
	{
		pkgs_file = popen("cygcheck -cd | wc -l", "r");
		fscanf(pkgs_file, "%d", &packages);
		packages -= 2;
		pclose(pkgs_file);

		snprintf(str, MAX_STRLEN, "%d", packages);
	}

	else if (OS == OSX)
	{
		pkgs_file = popen("ls /usr/local/bin | wc -w", "r");
		fscanf(pkgs_file, "%d", &packages);
		pclose(pkgs_file);

		pkgs_file = fopen("/usr/local/bin/brew", "r"); //test for existence of homebrew

		if (pkgs_file != NULL)
		{
			fclose(pkgs_file);

			int brew_pkgs = 0;
			pkgs_file = popen("brew list -1 | wc -l", "r");
			fscanf(pkgs_file, "%d", &brew_pkgs);
			pclose(pkgs_file);

			packages += brew_pkgs;
		}

		//test for existence of macports, fink, etc here
	}

	else if (OS == LINUX)
	{
		if (STRCMP(distro_str, "Arch Linux") || STRCMP(distro_str, "ParabolaGNU/Linux-libre") || STRCMP(distro_str, "Chakra") || STRCMP(distro_str, "Manjaro"))
		{
			pkgs_file = popen("pacman -Qq | wc -l", "r");
			fscanf(pkgs_file, "%d", &packages);
			pclose(pkgs_file);
		}

		else if (STRCMP(distro_str, "Frugalware"))
		{
			pkgs_file = popen("pacman-g2 -Q | wc -l", "r");
			fscanf(pkgs_file, "%d", &packages);
			pclose(pkgs_file);
		}

		else if (STRCMP(distro_str, "Fuduntu") || STRCMP(distro_str, "Ubuntu") || STRCMP(distro_str, "Mint") || STRCMP(distro_str, "SolusOS") || STRCMP(distro_str, "Debian") || STRCMP(distro_str, "LMDE") || STRCMP(distro_str, "CrunchBang") || STRCMP(distro_str, "Peppermint") || STRCMP(distro_str, "LinuxDeepin") || STRCMP(distro_str, "Trisquel") || STRCMP(distro_str, "Elementary OS"))
		{
			pkgs_file = popen("dpkg --get-selections | wc -l", "r");
			fscanf(pkgs_file, "%d", &packages);
			pclose(pkgs_file);
		}

		else if (STRCMP(distro_str, "Slackware"))
		{
			pkgs_file = popen("ls -l /var/log/packages | wc -l", "r");
			fscanf(pkgs_file, "%d", &packages);
			pclose(pkgs_file);
		}

		else if (STRCMP(distro_str, "Gentoo") || STRCMP(distro_str, "Sabayon") || STRCMP(distro_str, "Funtoo"))
		{
			pkgs_file = popen("ls -d /var/db/pkg/*/* | wc -l", "r");
			fscanf(pkgs_file, "%d", &packages);
			pclose(pkgs_file);
		}

		else if (STRCMP(distro_str, "Fedora") || STRCMP(distro_str, "openSUSE") || STRCMP(distro_str, "Red Hat Linux") || STRCMP(distro_str, "Mandriva") || STRCMP(distro_str, "Mandrake") || STRCMP(distro_str, "Mageia") || STRCMP(distro_str, "Viperr"))
		{
			pkgs_file = popen("rpm -qa | wc -l", "r");
			fscanf(pkgs_file, "%d", &packages);
			pclose(pkgs_file);
		}

		//if linux disto detection failed
		else if (STRCMP(distro_str, "Linux") && error)
		{
			ERROR_OUT("Error: Packages cannot be detected on an unknown Linux distro.", "");
		}
	}

	else if (OS == FREEBSD || OS == OPENBSD)
	{
		pkgs_file = popen("pkg_info | wc -l | awk '{sub(\" \", \"\");print $1}'", "r");

		pclose(pkgs_file);
	}

	else if (OS == NETBSD)
	{

	}

	else if (OS == DFBSD)
	{

	}

	if (verbose)
	{
		VERBOSE_OUT("Found package count as ", str);
	}

	return;
}

//detect_cpu
//detects the computer's CPU brand/name-string
//returns a string containing the CPU string
void detect_cpu(char* str)
{
	FILE* cpu_file;

	if (OS == CYGWIN)
	{
		cpu_file = popen("wmic cpu get name | tail -2 | tr -d '\\r\\n'", "r");
		fgets(str, MAX_STRLEN, cpu_file);
		pclose(cpu_file);
	}

	else if (OS == OSX)
	{
		cpu_file = popen("sysctl -n machdep.cpu.brand_string | tr -d '\\n'", "r");
		fgets(str, MAX_STRLEN, cpu_file);
		pclose(cpu_file);
	}

	else if (OS == LINUX || OS == NETBSD)
	{
		cpu_file = popen("awk 'BEGIN{FS=\":\"} /model name/ { print $2; exit }' /proc/cpuinfo | sed 's/ @/\\n/' | head -1", "r");
		fgets(str, MAX_STRLEN, cpu_file);
		pclose(cpu_file);
	}

	else if (OS == DFBSD || OS == FREEBSD || OS == OPENBSD)
	{
		cpu_file = popen("sysctl -n hw.model", "r");
		fgets(str, MAX_STRLEN, cpu_file);
		pclose(cpu_file);
	}

	if (verbose)
	{
		VERBOSE_OUT("Found CPU as ", str);
	}

	return;
}

//detect_gpu
//detects the computer's GPU brand/name-string
//returns a string containing the GPU string
void detect_gpu(char* str)
{
	FILE* gpu_file;

	if (OS == CYGWIN)
	{
		gpu_file = popen("wmic path Win32_VideoController get caption | tail -2 | tr -d '\\r\\n'", "r");
		fgets(str, MAX_STRLEN, gpu_file);
		pclose(gpu_file);
	}

	else if (OS == OSX)
	{
		gpu_file = popen("system_profiler SPDisplaysDataType | awk -F': ' '/^\\ *Chipset Model:/ {print $2}' | tr -d '\\n'", "r");
		fgets(str, MAX_STRLEN, gpu_file);
		pclose(gpu_file);
	}

	else if (OS == LINUX || ISBSD())
	{

	}

	if (verbose)
	{
		VERBOSE_OUT("Found GPU as ", str);
	}

	return;
}

//detect_disk
//detects the computer's total HDD/SSD capacity and usage
//returns a string of format: "<>G / <>G (<>%)", where <> is a number
void detect_disk(char* str)
{
	FILE* disk_file;

	char disk_total_str[MAX_STRLEN];
	char disk_free_str[MAX_STRLEN];

	if (OS == CYGWIN || OS == LINUX || OS == OSX)
	{
		disk_file = popen("df -H | grep -vE '^[A-Z]\\:\\/|File' | awk '{ print $2 }' | head -1 | tr -d '\\r\\n '", "r");
		fgets(disk_total_str, MAX_STRLEN, disk_file);
		pclose(disk_file);

		disk_file = popen("df -H | grep -vE '^[A-Z]\\:\\/|File' | awk '{ print $4 }' | head -1 | tr -d '\\r\\n '", "r");
		fgets(disk_free_str, MAX_STRLEN, disk_file);
		pclose(disk_file);
	}

	else if (ISBSD())
	{
		disk_file = popen("df -h | grep -vE '^[A-Z]\\:\\/|File' | awk '{ print $2 }' | head -1 | tr -d '\\r '", "r");
		fgets(disk_total_str, MAX_STRLEN, disk_file);
		pclose(disk_file);

		disk_file = popen("df -h | grep -vE '^[A-Z]\\:\\/|File' | awk '{ print $4 }' | head -1 | tr -d '\\r '", "r");
		fgets(disk_free_str, MAX_STRLEN, disk_file);
		pclose(disk_file);
	}

	snprintf(str, MAX_STRLEN, "%s / %s", disk_free_str, disk_total_str);

	if (verbose)
	{
		VERBOSE_OUT("Found disk usage as ", str);
	}

	return;
}

//detect_mem
//detects the computer's total and used RAM
//returns a string of format: "<>MB / <>MB", where <> is a number
void detect_mem(char* str)
{
	FILE* mem_file;

	int kb = 1024;
	int mb = kb * kb;
	//char total_mem_str[MAX_STRLEN];
	//char free_mem_str[MAX_STRLEN];
	//char used_mem_str[MAX_STRLEN]; //may or may not be used, depending on OS
	int total_mem_int; // each of the following _may_ contain _either_ bytes, kbytes, or mbytes
	int free_mem_int; //depending on the OS
	int used_mem_int;

	if (OS == CYGWIN || OS == NETBSD)
	{
		mem_file = popen("awk '/MemTotal/ { print $2 }' /proc/meminfo", "r");
		fscanf(mem_file, "%d", &total_mem_int);
		pclose(mem_file);

		mem_file = popen("awk '/MemFree/ { print $2 }' /proc/meminfo", "r");
		fscanf(mem_file, "%d", &free_mem_int);
		pclose(mem_file);

		total_mem_int /= (int) kb;
		free_mem_int /= (int) kb;
		used_mem_int = total_mem_int - free_mem_int;

		snprintf(str, MAX_STRLEN, "%d%s / %d%s", used_mem_int, "MB", total_mem_int, "MB");
	}

	else if (OS == OSX)
	{
		mem_file = popen("sysctl -n hw.memsize", "r");
		fscanf(mem_file, "%d", &total_mem_int);
		pclose(mem_file);

		total_mem_int /= (int) mb;
	}

	else if (OS == LINUX)
	{
		#ifdef __linux__
		struct sysinfo si_mem;
		sysinfo(&si_mem);

		int total_mem_int = (int) si_mem.totalram / mb;
		int free_mem_int = (int) si_mem.freeram / mb;
		int used_mem_int = (int) total_mem_int - free_mem_int;

		snprintf(str, MAX_STRLEN, "%d%s / %d%s", free_mem_int, "MB", total_mem_int, "MB");
		#endif
	}

	else if (OS == FREEBSD)
	{
		//it's unknown whether FreeBSD's /proc/meminfo is in the same format as Cygwin's

		//mem_file = popen("awk '/MemTotal/ { print $2 }' /proc/meminfo", "r");
		//fgets(total_mem_str, sizeof(total_mem_str), mem_file);
		//total_mem_int = (int) atoi(total_mem_str) / kb;
		//pclose(mem_file);
	}

	else if (OS == OPENBSD)
	{
		mem_file = popen("top -1 1 | awk '/Real:/ {k=split($3,a,\"/\");print a[k] }' | tr -d 'M'", "r");
		fscanf(mem_file, "%d", &total_mem_int);
		pclose(mem_file);

		mem_file = popen("top -1 1 | awk '/Real:/ {print $3}' | sed 's/M.*//'", "r");
		fscanf(mem_file, "%d", &used_mem_int);
		pclose(mem_file);

		snprintf(str, MAX_STRLEN, "%d%s / %d%s", used_mem_int, "MB", used_mem_int, "MB");
	}

	else if (OS == DFBSD)
	{

	}

	if (verbose)
	{
		VERBOSE_OUT("Found memory usage as ", str);
	}

	return;
}

//detect_shell
//detects the shell currently running on the computer
//returns a string containing the name of that shell
void detect_shell(char* str)
{
	FILE* shell_file;

	shell_file = popen("echo $SHELL | awk -F \"/\" '{print $NF}' | tr -d '\\r\\n'", "r");
	fgets(str, 128, shell_file);
	pclose(shell_file);

	return;
}

//detect_shell_version
//detects the version of the shell detected in detect_shell()
//returns a string containing the version
void detect_shell_version(char* str)
{
	FILE* shell_version_file;

	char temp_vers_str[MAX_STRLEN];

	if (STRCMP(shell_str, "bash"))
	{
		shell_version_file = popen("bash --version | head -1", "r");
		fgets(temp_vers_str, MAX_STRLEN, shell_version_file);
		//evil pointer arithmetic
		snprintf(str, MAX_STRLEN, "%.*s", 17, temp_vers_str + 10);
		pclose(shell_version_file);
	}

	else if (STRCMP(shell_str, "zsh"))
	{
		shell_version_file = popen("zsh --version", "r");
		fgets(temp_vers_str, MAX_STRLEN, shell_version_file);	
		//evil pointer arithmetic
		snprintf(str, MAX_STRLEN, "%.*s", 5, temp_vers_str + 4);
		pclose(shell_version_file);
	}

	else if (STRCMP(shell_str, "csh"))
	{
		shell_version_file = popen("csh --version | head -1", "r");
		fgets(temp_vers_str, MAX_STRLEN, shell_version_file);
		//evil pointer arithmetic
		snprintf(str, MAX_STRLEN, "%.*s", 7, temp_vers_str + 5);
		pclose(shell_version_file);
	}

	else if (STRCMP(shell_str, "ksh"))
	{
		
	}
	
	else if (STRCMP(shell_str, "fish"))
	{
		shell_version_file = popen("fish --version", "r");
		fgets(temp_vers_str, MAX_STRLEN, shell_version_file);
		//evil pointer arithmetic
		snprintf(str, MAX_STRLEN, "%.*s", 13, str + 6);
		pclose(shell_version_file);
	}
	
	if (verbose)
	{
		VERBOSE_OUT("Found shell version as ", str);
	}

	return;
}

//detect_res
//detects the combined resoloution of all monitors attached to the computer
//returns a string of format: "<>x<>", where <> is a number
void detect_res(char* str)
{
	FILE* res_file;

	char width_str[MAX_STRLEN];
	char height_str[MAX_STRLEN];

	if (OS == CYGWIN)
	{
		res_file = popen("wmic desktopmonitor get screenwidth | tail -2 | tr -d '\\r\\n '", "r");
		fgets(width_str, MAX_STRLEN, res_file);
		pclose(res_file);

		res_file = popen("wmic desktopmonitor get screenheight | tail -2 | tr -d '\\r\\n '", "r");
		fgets(height_str, MAX_STRLEN, res_file);
		pclose(res_file);

		snprintf(str, MAX_STRLEN, "%sx%s", width_str, height_str);
	}

	else if (OS == OSX)
	{
		res_file = popen("system_profiler SPDisplaysDataType | awk '/Resolution:/ {print $2\"x\"$4\" \"}' | tr -d '\\n'", "r");
		fgets(str, MAX_STRLEN, res_file);
		pclose(res_file);
	}

	else if (OS == LINUX)
	{
		res_file = popen("xdpyinfo | sed -n 's/.*dim.* \\([0-9]*x[0-9]*\\) .*/\\1/pg' | sed ':a;N;$!ba;s/\\n/ /g'", "r");
		fgets(str, MAX_STRLEN, res_file);
		pclose(res_file);
	}

	else if (ISBSD())
	{
		res_file = popen("xdpyinfo | sed -n 's/.*dim.* \\([0-9]*x[0-9]*\\) .*/\\1/pg' | tr '\\n' ' '", "r");
		fgets(str, MAX_STRLEN, res_file);
		pclose(res_file);
	}

	else //if (OS == UNKNOWN)
	{
		safe_strncpy(str, "No X Server", MAX_STRLEN);
		if (error)
		{
			ERROR_OUT("Error: Could not find an X Server on the current OS.", "");
		}
	}

	if (verbose)
	{
		VERBOSE_OUT("Found resolution as ", str);
	}

	return;
}

//detect_de
//detects the desktop environment currently running on top of the OS
//returns a string containing the name of the DE
void detect_de(char* str)
{
	FILE* de_file;

	if (OS == CYGWIN)
	{
		int version;

		de_file = popen("wmic os get version | grep -o '^[0-9]'", "r");
		fscanf(de_file, "%d", &version);
		pclose(de_file);

		if (version == 6 || version == 7)
		{
			safe_strncpy(str, "Aero", MAX_STRLEN);
		}
		else
		{
			safe_strncpy(str, "Luna", MAX_STRLEN);
		}
	}

	else if (OS == OSX)
	{
		safe_strncpy(str, "Aqua", MAX_STRLEN);
	}

	else if (OS == LINUX || ISBSD())
	{
		//this is going to be complicated
	}

	if (verbose)
	{
		VERBOSE_OUT("Found DE as ", str);
	}

	return;
}

//detect_wm
//detects the window manager currently running on top of the OS
//returns a string containing the name of the WM
void detect_wm(char* str)
{
	FILE* wm_file;

	char test_str[MAX_STRLEN];

	if (OS == CYGWIN)
	{
		wm_file = popen("tasklist | grep -o 'bugn' | tr -d '\\r\\n'", "r");
		//test for bugn
		pclose(wm_file);

		wm_file = popen("tasklist | grep -o 'Windawesome' | tr -d '\\r \\n'", "r");
		//test for Windawesome
		pclose(wm_file);

		//else
		safe_strncpy(str, "DWM", MAX_STRLEN);
	}

	else if (OS == OSX)
	{
		safe_strncpy(str, "Quartz Compositor", MAX_STRLEN);
	}

	else if (OS == LINUX || ISBSD())
	{

	}

	if (verbose)
	{
		VERBOSE_OUT("Found WM as ", str);
	}

	return;
}

//detect_wm_theme
//detects the theme associated with the WM detected in detect_wm()
//returns a string containing the name of the WM theme
void detect_wm_theme(char* str)
{
	if (verbose)
	{
		VERBOSE_OUT("Found WM theme as ", str);
	}

	return;
}

//detect_gtk
//detects the theme, icon(s), and font(s) associated with a GTK DE (if present)
//returns a string containing the name of the gtk theme
void detect_gtk(char* str)
{
	if (verbose)
	{
		VERBOSE_OUT("Found GTK as ", str);
	}

	return;
}

/*  **  END DETECTION FUNCTIONS  **  */


/*  **  BEGIN AUXILIARY FUNCTIONS  **  */

//safe_strncpy
//calls strncpy with the given params, then inserts a null char at the last position
//returns a string containing the copied data
char* safe_strncpy(char* destination, const char* source, size_t num)
{
	char* ret = strncpy(destination, source, num);
	ret[num - 1] = '\0';
	return ret;
}

//split_uptime
//splits param uptime into individual time-units
//PREREQ: uptime _must_ be in seconds
void split_uptime(float uptime, int* secs, int* mins, int* hrs, int* days)
{
	*secs = (int) uptime % 60;
	*mins = (int) (uptime / 60) % 60;
	*hrs = (int) (uptime / 3600) % 24;
	*days = (int) (uptime / 86400);

	return;
}

/*  **  END AUXILIARY FUNCTIONS  **  */


/*  **  BEGIN FLAG/OUTPUT/MISC FUNCTIONS  **  */

//display_version
//called if the -v flag is tripped, outputs the current version of screenfetch-c
void display_version(void)
{
	printf("%s\n", TBLU "screenfetch-c - Version 0.1 ALPHA");
	printf("%s\n", "Warning: This version of screenfetch is not yet finished");
	printf("%s\n", "and as such may contain bugs and security holes. Use with caution." TNRM);
}

//display_help
//called if the -h flag is tripped, tells the user where to find the manpage
void display_help(void)
{
	printf("%s\n", TBLU "screenfetch-c");
	printf("%s\n", "A rewrite of screenFetch, the popular shell script, in C.");
	printf("%s\n", "Operating Systems currently supported:");
	printf("%s\n", "Windows (via Cygwin), Linux, *BSD, and OS X.");
	printf("%s\n", "Using screenfetch-c on an OS not listed above may not work entirely or at all.");
	printf("%s\n", "Please access 'man screenfetch' for in-depth information on flags and usage." TNRM);
	return;
}

//take_screenshot
//takes a screenshot and saves it to $HOME 
//SECURITY WARNING: THIS FUNCTION MAKES SYSTEM CALLS
void take_screenshot(void)
{
	FILE* ss_file;

	if (OS == CYGWIN || OS == UNKNOWN)
	{
		//cygwin does not currently have a simple screenshot solution
		//potential solutions: "import -window root screenfetch_screenshot.jpg" - requires X
		ERROR_OUT("Error: This program does not currently support screenshots on your OS.", "");
	}

	else if (OS == OSX)
	{
		//this is really ugly
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

		system("screencapture -x screenfetch_screenshot.png");

		ss_file = fopen("~/screenfetch_screenshot.png", "r");

		if (ss_file != NULL && verbose)
		{
			fclose(ss_file);
			VERBOSE_OUT("Screenshot successfully saved.", "");
		}
		else
		{
			ERROR_OUT("Error: Problem saving screenshot.", "");
		}
	}

	else if (OS == LINUX || ISBSD())
	{
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

		system("scrot -cd3 screenfetch_screenshot.png");

		ss_file = fopen("~/screenfetch_screenshot.png", "r");

		if (ss_file != NULL && verbose)
		{
			fclose(ss_file);
			VERBOSE_OUT("Screenshot successfully saved.", "");
		}
		else
		{
			ERROR_OUT("Error: Problem saving screenshot.", "");
		}
	}

	return;
}

/*  **  END FLAG/OUTPUT/MISC FUNCTIONS  **  */
