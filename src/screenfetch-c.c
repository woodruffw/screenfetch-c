/* 	screenfetch-c.c
	Author: William Woodruff
	-------------

	A rewrite of screenFetch.sh in C.
	This is primarily an experiment borne out of an awareness of the slow execution time on the 
	screenfetch-dev.sh script. 
	Hopefully this port will execute faster, although it's more for self education than anything else.
	
	------

	NOTES:
	I used many of Brett Bohnenkamper's awk/sed/grep/etc oneliners in my popen() calls, 
	although some were modified to change/improve the output.
	Many thanks to him for discovering bugs and formatting issues as well.

	The shell scripts detectwm.sh, detectwmtheme.sh, and detectde.sh are all partially or completely based upon
	screenfetch-dev. They are called within the program, as detecting the WM/WM theme/DE/GTK within C would be a pain in the ass.

	------

	**From the original author:**

	------

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

	------

	I hereby regrant this version of screenFetch under the same MIT license.
	If you have any questions, please contact me on github (http://www.github.com/woodruffw/screenfetch-c) or at william @ tuffbizz.com
*/

#include "screenfetch-c.h" /* contains all other includes, function prototypes, macros, ascii logos */
#include "thread.h" /* for cross-platform threading */

/* string definitions - set to Unknown by default */
static char distro_str[MAX_STRLEN] = "Unknown";
static char arch_str[MAX_STRLEN] = "Unknown";
static char host_str[MAX_STRLEN] = "Unknown";
static char kernel_str[MAX_STRLEN] = "Unknown";
static char uptime_str[MAX_STRLEN] = "Unknown";
static char pkgs_str[MAX_STRLEN] = "Unknown";
static char cpu_str[MAX_STRLEN] = "Unknown";
static char gpu_str[MAX_STRLEN] = "Unknown";
static char disk_str[MAX_STRLEN] = "Unknown";
static char mem_str[MAX_STRLEN] = "Unknown";
static char shell_str[MAX_STRLEN] = "Unknown";
static char res_str[MAX_STRLEN] = "Unknown";
static char de_str[MAX_STRLEN] = "Unknown";
static char wm_str[MAX_STRLEN] = "Unknown";
static char wm_theme_str[MAX_STRLEN] = "Unknown";
static char gtk_str[MAX_STRLEN] = "Unknown";

/* output string definitions */
static char* detected_arr[16] = {host_str, distro_str, kernel_str, arch_str, cpu_str, gpu_str, shell_str, pkgs_str, disk_str, mem_str, uptime_str, res_str, de_str, wm_str, wm_theme_str, gtk_str};
static char* detected_arr_names[16] = {"", "OS: ", "Kernel: ", "Arch: ", "CPU: ", "GPU: ", "Shell: ", "Packages: ", "Disk: ", "Memory: ", "Uptime: ", "Resolution: ", "DE: ", "WM: ", "WM Theme: ", "GTK: "};

/* other definitions */
bool manual = false;
bool logo = true;
bool error = true;
bool verbose = false;
bool screenshot = false;

int main(int argc, char** argv)
{
	/* warn unknown OSes about using this program */
	if (OS == UNKNOWN)
	{
		ERROR_OUT("Warning: ", "This program isn't designed for your OS.");
		ERROR_OUT("Even if it did compile successfully, it will not execute correctly.", "");
		ERROR_OUT("It is HIGHLY recommended, therefore, that you use manual mode.", "");
	}

	struct option long_options[] =
	{
		{"manual", no_argument, 0, 'm'},
		{"verbose", no_argument, 0, 'v'},
		{"no-logo", no_argument, 0, 'n'},
		{"screenshot", no_argument, 0, 's'},
		{"distro", required_argument, 0, 'D'},
		{"suppress-errors", no_argument, 0, 'E'},
		{"version", no_argument, 0, 'V'},
		{"help", no_argument, 0, 'h'},
		{"logo-only", required_argument, 0, 'L'},
		{0, 0, 0, 0}
	};

	signed char c;
	int index = 0;
	while ((c = getopt_long(argc, argv, "mvnsD:EVhL:", long_options, &index)) != -1)
	{
		switch (c)
		{
			case 'm':
				manual = true;
				break;
			case 'v':
				verbose = true;
				break;
			case 'n':
				logo = false;
				break;
			case 's':
				screenshot = true;
				break;
			case 'D':
				SET_DISTRO(optarg);
				break;
			case 'E':
				error = false;
				break;
			case 'V':
				display_version();
				return EXIT_SUCCESS;
			case 'h':
				display_help();
				return EXIT_SUCCESS;
			case 'L':
				output_logo_only(optarg);
				return EXIT_SUCCESS;
			case '?':
				if (optopt == 'D')
					ERROR_OUT("Error: ", "The -D (--distro) flag requires an argument.");
				else if (optopt == 'L')
					ERROR_OUT("Error: ", "The -L (--logo-only) flag requires an argument.");
				else
					ERROR_OUT("Error: ", "Unknown option or option character.");
				return EXIT_FAILURE;
		}
	}

	if (manual) /* if the user has decided to enter manual mode */
	{
		int stat = manual_input();

		if (stat == EXIT_SUCCESS)
		{
			/* these sections are ALWAYS detected */
			detect_uptime(uptime_str);
			detect_pkgs(pkgs_str);
			detect_disk(disk_str);
			detect_mem(mem_str);

			/* if the user specifies an asterisk, fill the data in for them */
			if (STRCMP(distro_str, "*"))
				detect_distro(distro_str);
			if (STRCMP(arch_str, "*"))
				detect_arch(arch_str);
			if (STRCMP(host_str, "*"))
				detect_host(host_str);
			if (STRCMP(kernel_str, "*"))
				detect_kernel(kernel_str);
			if (STRCMP(cpu_str, "*"))
				detect_cpu(cpu_str);
			if (STRCMP(gpu_str, "*"))
				detect_gpu(gpu_str);
			if (STRCMP(shell_str, "*"))
				detect_shell(shell_str);
			if (STRCMP(res_str, "*"))
				detect_res(res_str);
			if (STRCMP(de_str, "*"))
				detect_de(de_str);
			if (STRCMP(wm_str, "*"))
				detect_wm(wm_str);
			if (STRCMP(wm_theme_str, "*"))
				detect_wm_theme(wm_theme_str);
			if (STRCMP(gtk_str, "*"))
				detect_gtk(gtk_str);
		}

		else /* if the user decided to leave manual mode without input */
			return EXIT_SUCCESS;
	}

	else /* each string is filled by its respective function */
	{
		if (OS != CYGWIN)
		{
			THREAD distro_thread;
			create_thread(&distro_thread, (void*) detect_distro, (void*) distro_str);
			join_thread(distro_thread); /* a few other functions rely on distro_str, so join the thread detect_distro completes */

			THREAD arch_thread;
			create_thread(&arch_thread, (void*) detect_arch, (void*) arch_str);

			THREAD host_thread;
			create_thread(&host_thread, (void*) detect_host, (void*) host_str);

			THREAD kernel_thread;
			create_thread(&kernel_thread, (void*) detect_kernel, (void*) kernel_str);

			THREAD uptime_thread;
			create_thread(&uptime_thread, (void*) detect_uptime, (void*) uptime_str);

			THREAD pkgs_thread;
			create_thread(&pkgs_thread, (void*) detect_pkgs, (void*) pkgs_str);

			THREAD cpu_thread;
			create_thread(&cpu_thread, (void*) detect_cpu, (void*) cpu_str);

			THREAD gpu_thread;
			create_thread(&gpu_thread, (void*) detect_gpu, (void*) gpu_str);

			THREAD disk_thread;
			create_thread(&disk_thread, (void*) detect_disk, (void*) disk_str);

			THREAD mem_thread;
			create_thread(&mem_thread, (void*) detect_mem, (void*) mem_str);

			THREAD shell_thread;
			create_thread(&shell_thread, (void*) detect_shell, (void*) shell_str);

			THREAD res_thread;
			create_thread(&res_thread, (void*) detect_res, (void*) res_str);

			THREAD de_thread;
			create_thread(&de_thread, (void*) detect_de, (void*) de_str);

			THREAD wm_thread;
			create_thread(&wm_thread, (void*) detect_wm, (void*) wm_str);

			THREAD wm_theme_thread;
			create_thread(&wm_theme_thread, (void*) detect_wm_theme, (void*) wm_theme_str);

			THREAD gtk_thread;
			create_thread(&gtk_thread, (void*) detect_gtk, (void*) gtk_str);

			join_thread(arch_thread);
			join_thread(host_thread);
			join_thread(kernel_thread);
			join_thread(uptime_thread);
			join_thread(pkgs_thread);
			join_thread(cpu_thread);
			join_thread(gpu_thread);
			join_thread(disk_thread);
			join_thread(mem_thread);
			join_thread(shell_thread);
			join_thread(res_thread);
			join_thread(de_thread);
			join_thread(wm_thread);
			join_thread(wm_theme_thread);
			join_thread(gtk_thread);
		}

		else /* i haven't perfected threading on windows yet, so don't change this conditional */
		{
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
			detect_res(res_str);
			detect_de(de_str);
			detect_wm(wm_str);
			detect_wm_theme(wm_theme_str);
			detect_gtk(gtk_str);
		}
	}

	if (logo)
		main_ascii_output(detected_arr, detected_arr_names);
	else
		main_text_output(detected_arr, detected_arr_names);

	if (screenshot)
		take_screenshot();

	return EXIT_SUCCESS;
}

/*  **  BEGIN DETECTION FUNCTIONS  ** */

/*  detect_distro
    detects the computer's distribution (really only relevant on Linux)
    argument char* str: the char array to be filled with the distro name
*/
void detect_distro(char* str)
{
	if (STRCMP(str, "Unknown") || STRCMP(str, "*")) /* if distro_str was NOT set by the -D flag or manual mode */
	{
		FILE* distro_file;

		char distro_name_str[MAX_STRLEN];

		if (OS == CYGWIN)
		{
			#if defined(NTDDI_WIN7)
				safe_strncpy(str, "Microsoft Windows 7", MAX_STRLEN);
			#elif defined(NTDDI_WIN8)
				safe_strncpy(str, "Microsoft Windows 8", MAX_STRLEN);
			#elif defined(NTDDI_WINBLUE)
				safe_strncpy(str, "Microsoft Windows 8.1", MAX_STRLEN);
			#elif defined(NTDDI_VISTA) || defined(NTDDI_VISTASP1)
				safe_strncpy(str, "Microsoft Windows Vista", MAX_STRLEN);
			#elif defined(NTDDI_WINXP) || defined(NTDDI_WINXPSP1) || defined(NTDDI_WINXPSP2) || defined(NTDDI_WINXPSP3)
				safe_strncpy(str, "Microsoft Windows XP", MAX_STRLEN);
			#else /* might add Server 03/08 conditions later */
				safe_strncpy(str, "Microsoft Windows", MAX_STRLEN);
			#endif
		}

		else if (OS == OSX)
		{
			distro_file = popen("sw_vers | grep ProductVersion | tr -d 'ProductVersion: \\t\\n'", "r");
			fgets(distro_name_str, MAX_STRLEN, distro_file);
			pclose(distro_file);

			snprintf(str, MAX_STRLEN, "%s%s", "Mac OS X ", distro_name_str);
		}

		else if (OS == LINUX)
		{
			if (FILE_EXISTS("/system/bin/getprop"))
			{
				safe_strncpy(str, "Android", MAX_STRLEN);
			}

			else
			{
				bool detected = false;

				/* Note: this is a very bad solution, as /etc/issue contains junk on some distros */
				distro_file = fopen("/etc/issue", "r");

				if (distro_file != NULL)
				{
					/* get the first 4 chars, that's all we need */
					fscanf(distro_file, "%4s", distro_name_str);
					fclose(distro_file);

					if (STRCMP(distro_name_str, "Back"))
					{
						safe_strncpy(str, "Backtrack Linux", MAX_STRLEN);
						detected = true;
					}

					else if (STRCMP(distro_name_str, "Crun"))
					{
						safe_strncpy(str, "CrunchBang", MAX_STRLEN);
						detected = true;
					}

					else if (STRCMP(distro_name_str, "LMDE"))
					{
						safe_strncpy(str, "LMDE", MAX_STRLEN);
						detected = true;
					}

					else if (STRCMP(distro_name_str, "Debi") || STRCMP(distro_name_str, "Rasp"))
					{
						safe_strncpy(str, "Debian", MAX_STRLEN);
						detected = true;
					}
				}

				if (!detected)
				{
					if (FILE_EXISTS("/etc/fedora-release"))
					{
						safe_strncpy(str, "Fedora", MAX_STRLEN);
					}

					else if (FILE_EXISTS("/etc/SuSE-release"))
					{
						safe_strncpy(str, "OpenSUSE", MAX_STRLEN);
					}

					else if (FILE_EXISTS("/etc/arch-release"))
					{
						safe_strncpy(str, "Arch Linux", MAX_STRLEN);
					}

					else if (FILE_EXISTS("/etc/gentoo-release"))
					{
						safe_strncpy(str, "Gentoo", MAX_STRLEN);
					}

					else if (FILE_EXISTS("/etc/angstrom-version"))
					{
						safe_strncpy(str, "Angstrom", MAX_STRLEN);
					}

					else if (FILE_EXISTS("/etc/lsb-release"))
					{
						distro_file = popen("head -1 < /etc/lsb-release | tr -d '\\\"\\n'", "r");
						fgets(distro_name_str, MAX_STRLEN, distro_file);
						pclose(distro_file);

						snprintf(str, MAX_STRLEN, "%s", distro_name_str + 11);
					}

					else
					{
						safe_strncpy(str, "Linux", MAX_STRLEN);

						if (error)
						{
							ERROR_OUT("Error: ", "Failed to detect specific Linux distro.");
						}
					}
				}
			}
		}

		else if (ISBSD() || OS == SOLARIS)
		{
			distro_file = popen("uname -sr | tr -d '\\n'", "r");
			fgets(str, MAX_STRLEN, distro_file);
			pclose(distro_file);
		}
	}

	if (verbose)
		VERBOSE_OUT("Found distro as ", str);

	return;
}

/*  detect_arch
    detects the computer's architecture
    argument char* str: the char array to be filled with the architecture
*/
void detect_arch(char* str)
{
	FILE* arch_file;

	if (OS == CYGWIN)
	{
		#if defined(__CYGWIN__)
			SYSTEM_INFO arch_info;
			GetNativeSystemInfo(&arch_info);

			if (arch_info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
			{
				safe_strncpy(str, "AMD64", MAX_STRLEN);
			}

			else if (arch_info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM)
			{
				safe_strncpy(str, "ARM", MAX_STRLEN);
			}

			else if (arch_info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
			{
				safe_strncpy(str, "IA64", MAX_STRLEN);
			}

			else if (arch_info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
			{
				safe_strncpy(str, "x86", MAX_STRLEN);
			}

			else
			{
				safe_strncpy(str, "Unknown", MAX_STRLEN);
			}
		#endif
	}

	else if (OS == OSX || OS == LINUX || OS == SOLARIS || OS == OPENBSD) /* short circuit here */
	{
		#if defined(__linux__) || (defined(__APPLE__) && defined(__MACH__)) || defined(__sun__) || defined(__OpenBSD__)
			struct utsname arch_info;
			uname(&arch_info);
			safe_strncpy(str, arch_info.machine, MAX_STRLEN);
		#endif
	}

	else if (ISBSD()) 
	{
		arch_file = popen("uname -m | tr -d '\\n'", "r");
		fgets(str, MAX_STRLEN, arch_file);
		pclose(arch_file);
	}

	if (verbose)
		VERBOSE_OUT("Found system arch as ", str);

	return;
}

/*  detect_host
    detects the computer's hostname and active user and formats them
    argument char* str: the char array to be filled with the user and hostname in format 'user@host'
    --
    CAVEAT: acceses an environment variable
    --
*/
void detect_host(char* str)
{
	char* given_user = "Unknown"; /* has to be a pointer for getenv()/GetUserName(), god knows why */
	char given_host[MAX_STRLEN] = "Unknown";

	if (OS == CYGWIN)
	{
		#if defined(__CYGWIN__)
			given_user = malloc(sizeof(char) * MAX_STRLEN);
			if (given_user == NULL)
			{
				ERROR_OUT("Error: ", "Failed to allocate sufficient memory in detect_host.");
				exit(1);
			}
			/* why does the winapi require a pointer to a long? */
			unsigned long len = MAX_STRLEN;
			GetUserName(given_user, &len);
			gethostname(given_host, MAX_STRLEN);
		#endif
	}

	else if (OS == OSX || OS == LINUX || OS == SOLARIS)
	{
		#if defined(__linux__) || (defined(__APPLE__) && defined(__MACH__)) || defined(__sun__)
			given_user = getlogin(); /* getlogin is apparently buggy on linux, so this might be changed */
			gethostname(given_host, MAX_STRLEN);
		#endif
	}

	else if (ISBSD())
	{
		given_user = getenv("USER");

		FILE* host_file = popen("hostname | tr -d '\\r\\n '", "r");
		fgets(given_host, MAX_STRLEN, host_file);
		pclose(host_file);
	}

	snprintf(str, MAX_STRLEN, "%s@%s", given_user, given_host);

	#if defined(__CYGWIN__)
		free(given_user);
	#endif

	if (verbose)
		VERBOSE_OUT("Found host as ", str);

	return;
}

/*  detect_kernel
    detects the computer's kernel
    argument char* str: the char array to be filled with the kernel name
*/
void detect_kernel(char* str)
{
	if (OS == CYGWIN)
	{
		#if defined(__CYGWIN__)
			OSVERSIONINFO kern_info;
			ZeroMemory(&kern_info, sizeof(OSVERSIONINFO));
			kern_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			GetVersionEx(&kern_info);
			snprintf(str, MAX_STRLEN, "Windows NT %d.%d build %d", (int) kern_info.dwMajorVersion, (int) kern_info.dwMinorVersion, (int) kern_info.dwBuildNumber);
		#endif
	}

	else if (ISBSD())
	{
		FILE* kernel_file = popen("uname -sr | tr -d '\\n'", "r");

		if (OS != CYGWIN)
			fgets(str, MAX_STRLEN, kernel_file);
		else
			fscanf(kernel_file, "%s", str);

		pclose(kernel_file);
	}

	else if (OS == OSX || OS == LINUX || OS == SOLARIS)
	{
		#if defined(__linux__) || (defined(__APPLE__) && defined(__MACH__)) || defined(__sun__)
			struct utsname kern_info;
			uname(&kern_info);
			snprintf(str, MAX_STRLEN, "%s %s", kern_info.sysname, kern_info.release);
		#endif
	}

	if (verbose)
		VERBOSE_OUT("Found kernel as ", str);

	return;
}

/*  detect_uptime
    detects the computer's uptime
    argument char* str: the char array to be filled with the uptime in format '$d $h $m $s' where $ is a number
*/
void detect_uptime(char* str)
{
	FILE* uptime_file;

	long uptime = 0; 
	#if !defined(__CYGWIN__) && !defined(__linux__)
		long currtime = 0, boottime = 0; /* may or may not be used depending on OS */
	#endif
	int secs = 0;
	int mins = 0;
	int hrs = 0;
	int days = 0;

	if (OS == CYGWIN)
	{
		#if defined(__CYGWIN__)
			uptime = GetTickCount(); /* known problem: will rollover after 49.7 days */
			uptime /= 1000;
		#endif
	}

	else if (OS == NETBSD)
	{
		uptime_file = popen("cut -d ' ' -f 1 < /proc/uptime", "r");
		fscanf(uptime_file, "%ld", &uptime);
		pclose(uptime_file);
	}

	else if (OS == OSX || OS == FREEBSD || OS == DFBSD)
	{
		#if defined(__FreeBSD__) || defined(__DragonFly__) || (defined(__APPLE__) && defined(__MACH__))
			uptime_file = popen("sysctl -n kern.boottime | cut -d '=' -f 2 | cut -d ',' -f 1", "r");
			fscanf(uptime_file, "%ld", &boottime); /* get boottime in secs */
			pclose(uptime_file);

			currtime = time(NULL);

			uptime = currtime - boottime;
		#endif
	}

	else if (OS == LINUX)
	{
		#if defined(__linux__)
			struct sysinfo si_upt;
			sysinfo(&si_upt);

			uptime = si_upt.uptime;
		#endif
	}

	else if (OS == OPENBSD)
	{
		#if defined(__OpenBSD__)
			uptime_file = popen("sysctl -n kern.boottime", "r");
			fscanf(uptime_file, "%ld", &boottime); /* get boottime in secs */
			pclose(uptime_file);

			currtime = time(NULL);

			uptime = currtime - boottime;
		#endif
	}

	else if (OS == SOLARIS)
	{
		#if defined(__sun__)
			currtime = time(NULL);
			struct utmpx* ent;

			while (ent = getutxent())
			{
				if (STRCMP("system boot", ent->ut_line))
				{
					boottime = ent->ut_tv.tv_sec;
				}
			}

			uptime = currtime - boottime;
		#endif
	}

	split_uptime(uptime, &secs, &mins, &hrs, &days);

	if (days > 0)
		snprintf(str, MAX_STRLEN, "%dd %dh %dm %ds", days, hrs, mins, secs);
	else
		snprintf(str, MAX_STRLEN, "%dh %dm %ds", hrs, mins, secs);

	if (verbose)
		VERBOSE_OUT("Found uptime as ", str);

	return;
}

/*  detect_pkgs
    detects the number of packages installed on the computer
    argument char* str: the char array to be filled with the number of packages
*/
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
		pkgs_file = popen("ls /usr/local/bin 2> /dev/null | wc -w", "r");
		fscanf(pkgs_file, "%d", &packages);
		pclose(pkgs_file);

		if (FILE_EXISTS("/usr/local/bin/brew"))
		{
			int brew_pkgs = 0;
			pkgs_file = popen("brew list -1 | wc -l", "r");
			fscanf(pkgs_file, "%d", &brew_pkgs);
			pclose(pkgs_file);

			packages += brew_pkgs;
		}

		if (FILE_EXISTS("/opt/local/bin/port"))
		{
			int port_pkgs = 0;
			pkgs_file = popen("port installed | wc -l", "r");
			fscanf(pkgs_file, "%d", &port_pkgs);
			pclose(pkgs_file);

			packages += port_pkgs;
		}

		if (FILE_EXISTS("/sw/bin/fink"))
		{
			int fink_pkgs = 0;
			pkgs_file = popen("/sw/bin/fink list -i | wc -l", "r");
			fscanf(pkgs_file, "%d", &fink_pkgs);
			pclose(pkgs_file);

			packages += fink_pkgs;
		}
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

		else if (STRCMP(distro_str, "Ubuntu") || STRCMP(distro_str, "Lubuntu") || STRCMP(distro_str, "Xubuntu") || STRCMP(distro_str, "LinuxMint") || STRCMP(distro_str, "SolusOS") || STRCMP(distro_str, "Debian") || STRCMP(distro_str, "LMDE") || STRCMP(distro_str, "CrunchBang") || STRCMP(distro_str, "Peppermint") || STRCMP(distro_str, "LinuxDeepin") || STRCMP(distro_str, "Trisquel") || STRCMP(distro_str, "elementary OS") || STRCMP(distro_str, "Backtrack Linux"))
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

		else if (STRCMP(distro_str, "Fuduntu") || STRCMP(distro_str, "Fedora") || STRCMP(distro_str, "OpenSUSE") || STRCMP(distro_str, "Red Hat Linux") || STRCMP(distro_str, "Mandriva") || STRCMP(distro_str, "Mandrake") || STRCMP(distro_str, "Mageia") || STRCMP(distro_str, "Viperr"))
		{
			pkgs_file = popen("rpm -qa | wc -l", "r");
			fscanf(pkgs_file, "%d", &packages);
			pclose(pkgs_file);
		}

		else if (STRCMP(distro_str, "Angstrom"))
		{
			pkgs_file = popen("opkg list-installed | wc -l", "r");
			fscanf(pkgs_file, "%d", &packages);
			pclose(pkgs_file);
		}

		/* if linux disto detection failed */
		else if (STRCMP(distro_str, "Linux"))
		{
			safe_strncpy(str, "Not Found", MAX_STRLEN);

			if (error)
				ERROR_OUT("Error: ", "Packages cannot be detected on an unknown Linux distro.");
		}
	}

	else if (OS == FREEBSD || OS == OPENBSD)
	{
		pkgs_file = popen("pkg_info | wc -l", "r");
		fscanf(pkgs_file, "%d", &packages);
		pclose(pkgs_file);
	}

	else if (OS == NETBSD || OS == DFBSD)
	{
		safe_strncpy(str, "Not Found", MAX_STRLEN);

		if (error)
			ERROR_OUT("Error: ", "Could not find packages on current OS.");
	}

	else if (OS == SOLARIS)
	{
		pkgs_file = popen("pkg list | wc -l", "r");
		fscanf(pkgs_file, "%d", &packages);
		pclose(pkgs_file);
	}

	snprintf(str, MAX_STRLEN, "%d", packages);

	if (verbose)
		VERBOSE_OUT("Found package count as ", str);

	return;
}

/*  detect_cpu
    detects the computer's CPU brand/name-string
    argument char* str: the char array to be filled with the CPU name
*/
void detect_cpu(char* str)
{
	FILE* cpu_file;

	if (OS == CYGWIN)
	{
		#if defined(__CYGWIN__)
			HKEY hkey;
			DWORD str_size = MAX_STRLEN;
			RegOpenKey(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", &hkey);
			RegQueryValueEx(hkey, "ProcessorNameString", 0, NULL, (BYTE*) str, &str_size);
		#endif
	}

	else if (OS == OSX)
	{
		cpu_file = popen("sysctl -n machdep.cpu.brand_string | sed 's/(\\([Tt][Mm]\\))//g;s/(\\([Rr]\\))//g;s/^//g' | tr -d '\\n' | tr -s ' '", "r");
		fgets(str, MAX_STRLEN, cpu_file);
		pclose(cpu_file);
	}

	else if (OS == LINUX || OS == NETBSD)
	{
		cpu_file = popen("awk 'BEGIN{FS=\":\"} /model name/ { print $2; exit }' /proc/cpuinfo | sed -e 's/ @/\\n/' -e 's/^ *//g' -e 's/ *$//g' | head -1 | tr -d '\\n'", "r");
		fgets(str, MAX_STRLEN, cpu_file);
		pclose(cpu_file);

		if (STRCMP(str, "ARMv6-compatible processor rev 7 (v6l)"))
		{
			safe_strncpy(str, "BCM2708 (Raspberry Pi)", MAX_STRLEN); /* quick patch for Raspberry Pi machines */
		}
	}

	else if (OS == DFBSD || OS == FREEBSD || OS == OPENBSD)
	{
		cpu_file = popen("sysctl -n hw.model | tr -d '\\n'", "r");
		fgets(str, MAX_STRLEN, cpu_file);
		pclose(cpu_file);
	}

	else if (OS == SOLARIS)
	{
		cpu_file = popen("psrinfo -pv | tail -1 | tr -d '\\t\\n'", "r");
		fgets(str, MAX_STRLEN, cpu_file);
		pclose(cpu_file);
	}

	if (verbose)
		VERBOSE_OUT("Found CPU as ", str);

	return;
}

/*  detect_gpu
    detects the computer's GPU brand/name-string
    argument char* str: the char array to be filled with the GPU name
*/
void detect_gpu(char* str)
{
	FILE* gpu_file;

	if (OS == CYGWIN)
	{
		#if defined(__CYGWIN__)
			HKEY hkey;
			DWORD str_size = MAX_STRLEN;
			RegOpenKey(HKEY_LOCAL_MACHINE, "SYSTEM\\ControlSet001\\Control\\Class\\{4D36E968-E325-11CE-BFC1-08002BE10318}\\0000\\Settings", &hkey);
			RegQueryValueEx(hkey, "Device Description", 0, NULL, (BYTE*) str, &str_size);
		#endif
	}

	else if (OS == OSX)
	{
		gpu_file = popen("system_profiler SPDisplaysDataType | awk -F': ' '/^\\ *Chipset Model:/ {print $2}' | tr -d '\\n'", "r");
		fgets(str, MAX_STRLEN, gpu_file);
		pclose(gpu_file);
	}

	else if (OS == LINUX || ISBSD() || OS == SOLARIS)
	{
		gpu_file = popen("detectgpu 2>/dev/null", "r");
		fgets(str, MAX_STRLEN, gpu_file);
		pclose(gpu_file);
	}

	if (verbose)
		VERBOSE_OUT("Found GPU as ", str);

	return;
}

/*  detect_disk
    detects the computer's total disk capacity and usage
    argument char* str: the char array to be filled with the disk data in format '$G / $G ($G%)', where $ is a number
*/
void detect_disk(char* str)
{
	FILE* disk_file;

	int disk_total = 0;
	int disk_used = 0;
	int disk_percentage = 0;

	if (OS == CYGWIN || OS == LINUX || OS == OSX)
	{
		/* Cygwin -- GetDiskFreeSpaceEx? */

		disk_file = popen("df -H 2> /dev/null | grep -vE '^[A-Z]\\:\\/|File' | awk '{ print $2 }' | head -1 | tr -d '\\r\\n G'", "r");
		fscanf(disk_file, "%d", &disk_total);
		pclose(disk_file);

		disk_file = popen("df -H 2> /dev/null | grep -vE '^[A-Z]\\:\\/|File' | awk '{ print $3 }' | head -1 | tr -d '\\r\\n G'", "r");
		fscanf(disk_file, "%d", &disk_used);
		pclose(disk_file);
	}

	else if (OS == FREEBSD) /* short circuit here */
	{
		disk_file = popen("df -h -c 2> /dev/null | grep -vE '^[A-Z]\\:\\/|File' | awk '{ print $2 }' | tail -1 | tr -d '\\r\\n G'", "r");
		fscanf(disk_file, "%d", &disk_total);
		pclose(disk_file);

		disk_file = popen("df -h -c 2> /dev/null | grep -vE '^[A-Z]\\:\\/|File' | awk '{ print $3 }' | tail -1 | tr -d '\\r\\n G'", "r");
		fscanf(disk_file, "%d", &disk_used);
		pclose(disk_file);
	}

	else if (ISBSD())
	{
		disk_file = popen("df -h 2> /dev/null | grep -vE '^[A-Z]\\:\\/|File' | awk '{ print $2 }' | head -1 | tr -d '\\r\\n G'", "r");
        fscanf(disk_file, "%d", &disk_total);
        pclose(disk_file);

        disk_file = popen("df -h 2> /dev/null | grep -vE '^[A-Z]\\:\\/|File' | awk '{ print $3 }' | head -1 | tr -d '\\r\\n G'", "r");
        fscanf(disk_file, "%d", &disk_used);
        pclose(disk_file);
	}

	else if (OS == SOLARIS)
	{
		/* not yet implemented */
	}

	if (disk_total > disk_used)
	{
		disk_percentage = (((float) disk_used / disk_total) * 100);

		snprintf(str, MAX_STRLEN, "%dG / %dG (%d%%)", disk_used, disk_total, disk_percentage);
	}

	else /* when disk_used is in a smaller unit */
	{
		disk_percentage = ((float) disk_used / (disk_total * 1024) * 100);

		snprintf(str, MAX_STRLEN, "%dM / %dG (%d%%)", disk_used, disk_total, disk_percentage);
	}


	if (verbose)
		VERBOSE_OUT("Found disk usage as ", str);

	return;
}

/*  detect_mem
    detects the computer's total and used RAM
    argument char* str: the char array to be filled with the memory data in format '$MB / $MB', where $ is a number
*/
void detect_mem(char* str)
{
	FILE* mem_file;

	long long total_mem = 0; /* each of the following MAY contain bytes/kbytes/mbytes/pages */
	long long free_mem = 0;
	long long used_mem = 0;

	if (OS == CYGWIN)
	{
		#if defined(__CYGWIN__)
			MEMORYSTATUSEX mem_stat;
			mem_stat.dwLength = sizeof(mem_stat);
			GlobalMemoryStatusEx(&mem_stat);

			total_mem = (long long) mem_stat.ullTotalPhys / MB;
			used_mem = total_mem - ((long long) mem_stat.ullAvailPhys / MB);
		#endif
	}

	else if (OS == OSX)
	{
		mem_file = popen("sysctl -n hw.memsize", "r");
		fscanf(mem_file, "%lld", &total_mem);
		pclose(mem_file);

		mem_file = popen("vm_stat | head -2 | tail -1 | tr -d 'Pages free: .'", "r");
		fscanf(mem_file, "%lld", &free_mem);
		pclose(mem_file);

		total_mem /= (long) MB;

		free_mem *= 4096; /* 4KiB is OS X's page size */
		free_mem /= (long) MB;

		used_mem = total_mem - free_mem;
	}

	else if (OS == LINUX)
	{
		/* known problem: because linux utilizes free ram extensively in caches/buffers,
		   the amount of memory sysinfo reports as free is very small.
		*/

		#if defined(__linux__)
			struct sysinfo si_mem;
			sysinfo(&si_mem);

			total_mem = (long long) (si_mem.totalram * si_mem.mem_unit) / MB;
			free_mem = (long long) (si_mem.freeram * si_mem.mem_unit) / MB;
			used_mem = (long long) total_mem - free_mem;
		#endif
	}

	else if (ISBSD())
	{
		mem_file = popen("sysctl -n hw.physmem", "r");
		fscanf(mem_file, "%lld", &total_mem);
		pclose(mem_file);

		total_mem /= (long) MB;
	}

	else if (OS == SOLARIS)
	{
		#if defined(__sun__)
			total_mem = sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGE_SIZE);
			total_mem /= (long) MB;
		#endif

		/* sar -r 1 to get free pages? */
	}

	if (ISBSD() || OS == SOLARIS)
		snprintf(str, MAX_STRLEN, "%lld%s", total_mem, "MB");
	else
		snprintf(str, MAX_STRLEN, "%lld%s / %lld%s", used_mem, "MB", total_mem, "MB");

	if (verbose)
		VERBOSE_OUT("Found memory usage as ", str);

	return;
}

/*  detect_shell
    detects the shell currently running on the computer
    argument char* str: the char array to be filled with the shell name and version
    --
    CAVEAT: shell version detection relies on the standard versioning format for 
    each shell. If any shell's older (or newer versions) suddenly begin to use a new
    scheme, the version may be displayed incorrectly.
    --
*/
void detect_shell(char* str)
{
	FILE* shell_file;

	char shell_name[MAX_STRLEN];
	char vers_str[MAX_STRLEN];

	shell_file = popen("echo $SHELL | awk -F \"/\" '{print $NF}' 2> /dev/null | tr -d '\\r\\n'", "r");
	fgets(shell_name, MAX_STRLEN, shell_file);
	pclose(shell_file);

	if (STRCMP(shell_name, "bash"))
	{
		shell_file = popen("bash --version | head -1", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);
		/* evil pointer arithmetic */
		snprintf(str, MAX_STRLEN, "%s %.*s", shell_name, 17, vers_str + 10);
		pclose(shell_file);
	}

	else if (STRCMP(shell_name, "zsh"))
	{
		shell_file = popen("zsh --version", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);	
		/* evil pointer arithmetic */
		snprintf(str, MAX_STRLEN, "%s %.*s", shell_name, 5, vers_str + 4);
		pclose(shell_file);
	}

	else if (STRCMP(shell_name, "csh"))
	{
		shell_file = popen("csh --version | head -1", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);
		/* evil pointer arithmetic */
		snprintf(str, MAX_STRLEN, "%s %.*s", shell_name, 7, vers_str + 5);
		pclose(shell_file);
	}

	else if (STRCMP(shell_name, "fish"))
	{
		shell_file = popen("fish --version", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);
		/* evil pointer arithmetic */
		snprintf(str, MAX_STRLEN, "%s %.*s", shell_name, 13, vers_str + 6);
		pclose(shell_file);
	}

	else if (STRCMP(shell_name, "dash") || STRCMP(shell_name, "ash") || STRCMP(shell_name, "ksh"))
	{
		/* i don't have a version detection system for these, yet */
		safe_strncpy(str, shell_name, MAX_STRLEN);
	}

	if (verbose)
		VERBOSE_OUT("Found shell as ", str);

	return;
}

/*  detect_res
    detects the combined resolution of all monitors attached to the computer
    argument char* str: the char array to be filled with the resolution in format '$x$', where $ is a number
*/
void detect_res(char* str)
{
	FILE* res_file;

	int width = 0;
	int height = 0;

	if (OS == CYGWIN)
	{
		#if defined(__CYGWIN__)
			width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
			height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
		#endif

		snprintf(str, MAX_STRLEN, "%dx%d", width, height);
	}

	else if (OS == OSX)
	{
		res_file = popen("system_profiler SPDisplaysDataType | awk '/Resolution:/ {print $2\"x\"$4}' | tr -d '\\n'", "r");
		fgets(str, MAX_STRLEN, res_file);
		pclose(res_file);
	}

	else if (OS == LINUX || OS == SOLARIS)
	{
		#if defined(__linux__) || defined(__sun__)
			Display* disp = XOpenDisplay(NULL);
			if (disp != NULL)
			{
				Screen* screen = XDefaultScreenOfDisplay(disp);
				width = WidthOfScreen(screen);
				height = HeightOfScreen(screen);
				snprintf(str, MAX_STRLEN, "%dx%d", width, height);
			}
			else
			{
				safe_strncpy(str, "No X Server", MAX_STRLEN);

				if (error)
					ERROR_OUT("Error: ", "Problem detecting X display resolution.");
			}
		#endif
	}

	else if (ISBSD())
	{
		res_file = popen("xdpyinfo 2> /dev/null | sed -n 's/.*dim.* \\([0-9]*x[0-9]*\\) .*/\\1/pg' | tr '\\n' ' '", "r");
		fgets(str, MAX_STRLEN, res_file);
		pclose(res_file);

		if (STRCMP(str, "Unknown"))
		{
			safe_strncpy(str, "No X Server", MAX_STRLEN);
		}
	}

	if (verbose)
		VERBOSE_OUT("Found resolution as ", str);

	return;
}

/*  detect_de
    detects the desktop environment currently running on top of the OS
    argument char* str: the char array to be filled with the DE name
    --
	CAVEAT: On *BSDs and Linux distros, this function relies on the presence of 
	'detectde', a shell script. If it isn't present in the working directory, the DE will be set as 'Unknown'
    --
*/
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
		de_file = popen("detectde 2> /dev/null", "r");
		fgets(str, MAX_STRLEN, de_file);
		pclose(de_file);
	}

	else if (OS == SOLARIS)
	{
		/* detectde needs to be made compatible with Solaris's AWK */
	}

	if (verbose)
		VERBOSE_OUT("Found DE as ", str);

	return;
}

/*  detect_wm
    detects the window manager currently running on top of the OS
    argument char* str: the char array to be filled with the WM name
    --
	CAVEAT: On *BSDs and Linux distros, this function relies on the presence of 
	'detectwm', a shell script. If it isn't present in the working directory, the WM will be set as 'Unknown'
    --
*/
void detect_wm(char* str)
{
	FILE* wm_file;

	if (OS == CYGWIN)
	{
		/* wm_file = popen("tasklist | grep -o 'bugn' | tr -d '\\r\\n'", "r"); */
		/* test for bugn */
		/* pclose(wm_file); */

		/* wm_file = popen("tasklist | grep -o 'Windawesome' | tr -d '\\r \\n'", "r"); */
		/* test for Windawesome */
		/* pclose(wm_file); */

		/* else */
		safe_strncpy(str, "DWM", MAX_STRLEN);
	}

	else if (OS == OSX)
	{
		safe_strncpy(str, "Quartz Compositor", MAX_STRLEN);
	}

	else if (OS == LINUX || ISBSD() || OS == SOLARIS)
	{
		wm_file = popen("detectwm 2> /dev/null", "r");
		fgets(str, MAX_STRLEN, wm_file);
		pclose(wm_file);
	}

	if (verbose)
		VERBOSE_OUT("Found WM as ", str);

	return;
}

/*  detect_wm_theme
    detects the theme associated with the WM detected in detect_wm()
    argument char* str: the char array to be filled with the WM Theme name
    --
	CAVEAT: On *BSDs and Linux distros, this function relies on the presence of 
	'detectwmtheme', a shell script. If it isn't present in the working directory, the WM Theme will be set as 'Unknown'
    --
*/
void detect_wm_theme(char* str)
{
	FILE* wm_theme_file;

	if (OS == CYGWIN)
	{
		/* nasty one-liner */
		wm_theme_file = popen("reg query 'HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes' /v 'CurrentTheme' | grep -o '[A-Z]:\\\\.*' | awk -F\"\\\\\" '{print $NF}' | grep -o '[0-9A-z. ]*$' | grep -o '^[0-9A-z ]*' | tr -d '\\r\\n'", "r");
		fgets(str, MAX_STRLEN, wm_theme_file);
		pclose(wm_theme_file);
	}

	else if (OS == OSX)
	{
		safe_strncpy(str, "Aqua", MAX_STRLEN);
	}

	else if (OS == LINUX || ISBSD() || OS == SOLARIS)
	{
		wm_theme_file = popen("detectwmtheme 2> /dev/null", "r");
		fgets(str, MAX_STRLEN, wm_theme_file);
		pclose(wm_theme_file);
	}

	if (verbose)
		VERBOSE_OUT("Found WM theme as ", str);

	return;
}

/*  detect_gtk
    detects the theme, icon(s), and font(s) associated with a GTK DE (if present)
    argument char* str: the char array to be filled with the GTK info
    --
	CAVEAT: On *BSDs and Linux distros, this function relies on the presence of 
	'detectgtk', a shell script. If it isn't present in the working directory, the GTK will be set as 'Unknown'
    --
*/
void detect_gtk(char* str)
{
	FILE* gtk_file;

	char gtk2_str[MAX_STRLEN] = "Unknown";
	char gtk3_str[MAX_STRLEN] = "Unknown";
	char gtk_icons_str[MAX_STRLEN] = "Unknown";
	char font_str[MAX_STRLEN] = "Unknown";

	if (OS == CYGWIN)
	{
		/* get the terminal's font */
		gtk_file = popen("grep '^Font=.*' < $HOME/.minttyrc | grep -o '[0-9A-z ]*$' | tr -d '\\r\\n'", "r");
		fgets(font_str, MAX_STRLEN, gtk_file);
		pclose(gtk_file);

		snprintf(str, MAX_STRLEN, "%s (Font)", font_str);
	}

	else if (OS == OSX)
	{
		safe_strncpy(str, "Not Applicable", MAX_STRLEN);
	}

	else if (OS == LINUX || ISBSD())
	{
		gtk_file = popen("detectgtk 2> /dev/null", "r");
		fscanf(gtk_file, "%s%s%s%s", gtk2_str, gtk3_str, gtk_icons_str, font_str);
		pclose(gtk_file);

		if (STRCMP(gtk3_str, "Unknown"))
			snprintf(str, MAX_STRLEN, "%s (GTK2), %s (Icons)", gtk2_str, gtk_icons_str);
		else if (STRCMP(gtk2_str, "Unknown"))
			snprintf(str, MAX_STRLEN, "%s (GTK3), %s (Icons)", gtk3_str, gtk_icons_str);
		else
			snprintf(str, MAX_STRLEN, "%s (GTK2), %s (GTK3)", gtk2_str, gtk3_str);
	}

	else if (OS == SOLARIS)
	{
		/* detectgtk needs to be made compatible with Solaris's awk */
	}

	if (verbose)
		VERBOSE_OUT("Found GTK as ", str);

	return;
}

/*  manual_input
	generates (or reads) the ~/.screenfetchc file based upon user input
	returns an int indicating status (SUCCESS or FAILURE)
*/
int manual_input(void)
{
	FILE* config_file;
	char* config_file_loc;

	config_file_loc = getenv("HOME");
	strncat(config_file_loc, "/.screenfetchc", MAX_STRLEN);

	if (!FILE_EXISTS(config_file_loc))
	{
		if (OS == CYGWIN)
		{
			printf("%s\n", TBLU "WARNING: There is currenly a bug involving manual mode on Windows." TNRM);
			printf("%s\n", TBLU "Only continue if you are ABSOLUTELY sure." TNRM);
		}

		printf("%s\n", "This appears to be your first time running screenfetch-c in manual mode.");
		printf("%s", "Would you like to continue? (y/n) ");

		char choice = getchar();
		getchar(); /* flush the newline */

		if (choice == 'y' || choice == 'Y')
		{
			config_file = fopen(config_file_loc, "w");

			printf("%s\n", "We are now going to begin the manual mode input process.");
			printf("%s\n", "Please enter exactly what is asked for.");
			printf("%s\n", "If you are unsure about format, please consult the manpage.");

			printf("%s", "Please enter the name of your distribution/OS: ");
			fgets(distro_str, MAX_STRLEN, stdin);
			fputs(distro_str, config_file);

			printf("%s", "Please enter your architecture: ");
			fgets(arch_str, MAX_STRLEN, stdin);
			fputs(arch_str, config_file);

			printf("%s", "Please enter your username@hostname: ");
			fgets(host_str, MAX_STRLEN, stdin);
			fputs(host_str, config_file);

			printf("%s", "Please enter your kernel: ");
			fgets(kernel_str, MAX_STRLEN, stdin);
			fputs(kernel_str, config_file);

			printf("%s", "Please enter your CPU name: ");
			fgets(cpu_str, MAX_STRLEN, stdin);
			fputs(cpu_str, config_file);

			printf("%s", "Please enter your GPU name: ");
			fgets(gpu_str, MAX_STRLEN, stdin);
			fputs(gpu_str, config_file);

			printf("%s", "Please enter your shell name and version: ");
			fgets(shell_str, MAX_STRLEN, stdin);
			fputs(shell_str, config_file);

			printf("%s", "Please enter your monitor resolution: ");
			fgets(res_str, MAX_STRLEN, stdin);
			fputs(res_str, config_file);

			printf("%s", "Please enter your DE name: ");
			fgets(de_str, MAX_STRLEN, stdin);
			fputs(de_str, config_file);

			printf("%s", "Please enter your WM name: ");
			fgets(wm_str, MAX_STRLEN, stdin);
			fputs(wm_str, config_file);

			printf("%s", "Please enter your WM Theme name: ");
			fgets(wm_theme_str, MAX_STRLEN, stdin);
			fputs(wm_theme_str, config_file);

			printf("%s", "Please enter any GTK info: ");
			fgets(gtk_str, MAX_STRLEN, stdin);
			fputs(gtk_str, config_file);

			printf("%s\n", "That concludes the manual input.");
			printf("%s\n", "From now on, screenfetch-c will use this information when called with -m.");

			fclose(config_file);

			/* i am deeply ashamed of this solution */
			distro_str[strlen(distro_str) - 1] = '\0';
			arch_str[strlen(arch_str) - 1] = '\0';
			host_str[strlen(host_str) - 1] = '\0';
			kernel_str[strlen(kernel_str) - 1] = '\0';
			cpu_str[strlen(cpu_str) - 1] = '\0';
			gpu_str[strlen(gpu_str) - 1] = '\0';
			shell_str[strlen(shell_str) - 1] = '\0';
			res_str[strlen(res_str) - 1] = '\0';
			de_str[strlen(de_str) - 1] = '\0';
			wm_str[strlen(wm_str) - 1] = '\0';
			wm_theme_str[strlen(wm_theme_str) - 1] = '\0';
			gtk_str[strlen(gtk_str) - 1] = '\0';

			return EXIT_SUCCESS;
		}

		else
		{
			printf("%s\n", "Exiting manual mode and screenfetch-c.");
			printf("%s\n", "If you wish to run screenfetch-c normally, do not use the -m (--manual) flag next time.");

			return EXIT_FAILURE;
		}
	}

	else
	{
		if (verbose)
			VERBOSE_OUT("Found config file. Reading...", "");

		config_file = fopen(config_file_loc, "r");

		fgets(distro_str, MAX_STRLEN, config_file);
		fgets(arch_str, MAX_STRLEN, config_file);
		fgets(host_str, MAX_STRLEN, config_file);
		fgets(kernel_str, MAX_STRLEN, config_file);
		fgets(cpu_str, MAX_STRLEN, config_file);
		fgets(gpu_str, MAX_STRLEN, config_file);
		fgets(shell_str, MAX_STRLEN, config_file);
		fgets(res_str, MAX_STRLEN, config_file);
		fgets(de_str, MAX_STRLEN, config_file);
		fgets(wm_str, MAX_STRLEN, config_file);
		fgets(wm_theme_str, MAX_STRLEN, config_file);
		fgets(gtk_str, MAX_STRLEN, config_file);

		fclose(config_file);

		/* i am deeply ashamed of this solution */
		distro_str[strlen(distro_str) - 1] = '\0';
		arch_str[strlen(arch_str) - 1] = '\0';
		host_str[strlen(host_str) - 1] = '\0';
		kernel_str[strlen(kernel_str) - 1] = '\0';
		cpu_str[strlen(cpu_str) - 1] = '\0';
		gpu_str[strlen(gpu_str) - 1] = '\0';
		shell_str[strlen(shell_str) - 1] = '\0';
		res_str[strlen(res_str) - 1] = '\0';
		de_str[strlen(de_str) - 1] = '\0';
		wm_str[strlen(wm_str) - 1] = '\0';
		wm_theme_str[strlen(wm_theme_str) - 1] = '\0';
		gtk_str[strlen(gtk_str) - 1] = '\0';

		return EXIT_SUCCESS;
	}
}

/*  **  END DETECTION FUNCTIONS  **  */


/*  **  BEGIN AUXILIARY FUNCTIONS  **  */

/*  safe_strncpy
    calls strncpy with the given params, then inserts a null char at the last position
    returns a pointer to a string containing the copied data (same as destination)
*/
char* safe_strncpy(char* destination, const char* source, size_t num)
{
	char* ret = strncpy(destination, source, num);
	ret[num - 1] = '\0';
	return ret;
}

/*  split_uptime
    splits param uptime into individual time-units
    argument long uptime: the uptime, in seconds, to be split
    arguments int* secs...days: pointers to ints where the split uptime will be stored
    --
    CAVEAT: uptime MUST be in seconds
    --
*/
void split_uptime(long uptime, int* secs, int* mins, int* hrs, int* days)
{
	*secs = (int) uptime % 60;
	*mins = (int) (uptime / 60) % 60;
	*hrs = (int) (uptime / 3600) % 24;
	*days = (int) (uptime / 86400);

	return;
}

/*  **  END AUXILIARY FUNCTIONS  **  */


/*  output_logo_only
	outputs an ASCII logo based upon the distro name passed to it
	argument char* distro the name of the distro to output
*/
void output_logo_only(char* distro)
{
	int i = 0;

	if (STRCMP(distro, "Windows"))
	{
		for (i = 0; i < 16; i++)
		{
			printf("%s\n", windows_logo[i]);
		}
	}

	else if (STRCMP(distro, "OS X"))
	{
		for (i = 0; i < 16; i++)
		{
			printf("%s\n", macosx_logo[i]);
		}
	}

	else if (STRCMP(distro, "Arch Linux - Old"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", oldarch_logo[i]);
		}
	}

	else if (STRCMP(distro, "Arch Linux"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", arch_logo[i]);
		}
	}

	else if (STRCMP(distro, "LinuxMint"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", mint_logo[i]);
		}
	}

	else if (STRCMP(distro, "LMDE"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", lmde_logo[i]);
		}
	}

	else if (STRCMP(distro, "Ubuntu") || STRCMP(distro, "Lubuntu") || STRCMP(distro, "Xubuntu"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", ubuntu_logo[i]);
		}
	}

	else if (STRCMP(distro, "Debian"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", debian_logo[i]);
		}
	}

	else if (STRCMP(distro, "CrunchBang"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", crunchbang_logo[i]);
		}
	}

	else if (STRCMP(distro, "Gentoo"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", gentoo_logo[i]);
		}
	}

	else if (STRCMP(distro, "Funtoo"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", funtoo_logo[i]);
		}
	}

	else if (STRCMP(distro, "Fedora"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", fedora_logo[i]);
		}
	}

	else if (STRCMP(distro, "Mandriva") || STRCMP(distro, "Mandrake"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", mandriva_mandrake_logo[i]);
		}
	}

	else if (STRCMP(distro, "OpenSUSE"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", opensuse_logo[i]);
		}
	}

	else if (STRCMP(distro, "Slackware"))
	{
		for (i = 0; i < 21; i++)
		{
			printf("%s\n", slackware_logo[i]);
		}
	}

	else if (STRCMP(distro, "Red Hat Linux"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", redhat_logo[i]);
		}
	}

	else if (STRCMP(distro, "Frugalware"))
	{
		for (i = 0; i < 23; i++)
		{
			printf("%s\n", frugalware_logo[i]);
		}
	}

	else if (STRCMP(distro, "Peppermint"))
	{
		for (i = 0; i < 19; i++)
		{
			printf("%s\n", peppermint_logo[i]);
		}
	}

	else if (STRCMP(distro, "SolusOS"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", solusos_logo[i]);
		}
	}

	else if (STRCMP(distro, "Mageia"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", mageia_logo[i]);
		}
	}

	else if (STRCMP(distro, "ParabolaGNU/Linux-libre"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", parabolagnu_linuxlibre_logo[i]);
		}
	}

	else if (STRCMP(distro, "Viperr"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", viperr_logo[i]);
		}
	}

	else if (STRCMP(distro, "LinuxDeepin"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", linuxdeepin_logo[i]);
		}
	}

	else if (STRCMP(distro, "Chakra"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", chakra_logo[i]);
		}
	}

	else if (STRCMP(distro, "Fuduntu"))
	{
		for (i = 0; i < 21; i++)
		{
			printf("%s\n", fuduntu_logo[i]);
		}
	}

	else if (STRCMP(distro, "Trisquel"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", trisquel_logo[i]);
		}
	}

	else if (STRCMP(distro, "Manjaro"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", manjaro_logo[i]);
		}
	}

	else if (STRCMP(distro, "elementary OS"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", elementaryos_logo[i]);
		}
	}

	else if (STRCMP(distro, "Scientific Linux"))
	{
		for (i = 0; i < 20; i++)
		{
			printf("%s\n", scientificlinux_logo[i]);
		}
	}

	else if (STRCMP(distro, "Backtrack Linux"))
	{
		for (i = 0; i < 21; i++)
		{
			printf("%s\n", backtracklinux_logo[i]);
		}
	}

	else if (STRCMP(distro, "Sabayon"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", sabayon_logo[i]);
		}
	}

	else if (STRCMP(distro, "FreeBSD"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", freebsd_logo[i]);
		}
	}

	else if (STRCMP(distro, "OpenBSD"))
	{
		for (i = 0; i < 23; i++)
		{
			printf("%s\n", openbsd_logo[i]);
		}
	}

	else if (STRCMP(distro, "NetBSD"))
	{
		for (i = 0; i < 20; i++)
		{
			printf("%s\n", netbsd_logo[i]);
		}
	}

	else if (STRCMP(distro, "DragonFly BSD"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", dragonflybsd_logo[i]);
		}
	}

	else if (STRCMP(distro, "Android"))
	{
		for (i = 0; i < 16; i++)
		{
			printf("%s\n", android_logo[i]);
		}
	}

	else if (STRCMP(distro, "Solaris"))
	{
		for (i = 0; i < 17; i++)
		{
			printf("%s\n", solaris_logo[i]);
		}
	}

	else if (STRCMP(distro, "Angstrom"))
	{
		for (i = 0; i < 16; i++)
		{
			printf("%s\n", angstrom_logo[i]);
		}
	}

	else /* if (STRCMP(distro_str, "Linux")) */
	{
		for (i = 0; i < 16; i++)
		{
			printf("%s\n", linux_logo[i]);
		}
	}
}

/*  **  BEGIN FLAG/OUTPUT/MISC FUNCTIONS  **  */

/*  main_ascii_output
    the primary output for screenfetch-c - all info and ASCII art is printed here
    arguments char* data[], char* data_names[]: string arrays containing the names and data acquired
*/
void main_ascii_output(char* data[], char* data_names[])
{
	int i = 0;

	if (OS == CYGWIN)
	{
		for (i = 0; i < 16; i++)
		{
			printf("%s %s%s%s%s%s\n", windows_logo[i], TRED, data_names[i], TWHT, data[i], TNRM);
		}
	}

	else if (OS == OSX)
	{
		for (i = 0; i < 16; i++)
		{
			printf("%s %s%s%s%s\n", macosx_logo[i], TLBL, data_names[i], TNRM, data[i]);
		}
	}

	else if (OS == LINUX)
	{
		if (STRCMP(distro_str, "Arch Linux - Old"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s%s\n", oldarch_logo[i], TLBL, data_names[i], TNRM, data[i], TNRM);
				else
					printf("%s\n", oldarch_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Arch Linux"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s%s\n", arch_logo[i], TLCY, data_names[i], TNRM, data[i], TNRM);
				else
					printf("%s\n", arch_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "LinuxMint"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s%s\n", mint_logo[i], TLGN, data_names[i], TNRM, data[i], TNRM);
				else
					printf("%s\n", mint_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "LMDE"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", lmde_logo[i], TLGN, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", lmde_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Ubuntu") || STRCMP(distro_str, "Lubuntu") || STRCMP(distro_str, "Xubuntu"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s%s\n", ubuntu_logo[i], TLRD, data_names[i], TNRM, data[i], TNRM);
				else
					printf("%s\n", ubuntu_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Debian"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", debian_logo[i], TLRD, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", debian_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "CrunchBang"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", crunchbang_logo[i], TDGY, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", crunchbang_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Gentoo"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", gentoo_logo[i], TLPR, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", gentoo_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Funtoo"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", funtoo_logo[i], TLPR, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", funtoo_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Fedora"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", fedora_logo[i], TLBL, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", fedora_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Mandriva") || STRCMP(distro_str, "Mandrake"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", mandriva_mandrake_logo[i], TLBL, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", mandriva_mandrake_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "OpenSUSE"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", opensuse_logo[i], TLGN, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", opensuse_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Slackware"))
		{
			for (i = 0; i < 21; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", slackware_logo[i], TLBL, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", slackware_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Red Hat Linux"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", redhat_logo[i], TRED, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", redhat_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Frugalware"))
		{
			for (i = 0; i < 23; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", frugalware_logo[i], TLCY, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", frugalware_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Peppermint"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", peppermint_logo[i], TLRD, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", peppermint_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "SolusOS"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", solusos_logo[i], TLGY, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", solusos_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Mageia"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", mageia_logo[i], TLCY, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", mageia_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "ParabolaGNU/Linux-libre"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", parabolagnu_linuxlibre_logo[i], TLPR, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", parabolagnu_linuxlibre_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Viperr"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", viperr_logo[i], TLGY, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", viperr_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "LinuxDeepin"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", linuxdeepin_logo[i], TLGN, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", linuxdeepin_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Chakra"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", chakra_logo[i], TLBL, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", chakra_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Fuduntu"))
		{
			for (i = 0; i < 21; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", fuduntu_logo[i], TLRD, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", fuduntu_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Trisquel"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", trisquel_logo[i], TLBL, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", trisquel_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Manjaro"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s\n", manjaro_logo[i], data_names[i], data[i]);
				else
					printf("%s\n", manjaro_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "elementary OS"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", elementaryos_logo[i], TLGN, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", elementaryos_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Scientific Linux"))
		{
			for (i = 0; i < 20; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", scientificlinux_logo[i], TLRD, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", scientificlinux_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Backtrack Linux"))
		{
			for (i = 0; i < 20; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", backtracklinux_logo[i], TLRD, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", backtracklinux_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Sabayon"))
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", sabayon_logo[i], TLBL, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", sabayon_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Android"))
		{
			for (i = 0; i < 16; i++)
			{
				if (i < 12)
					printf("%s %s%s%s%s\n", android_logo[i], TLGN, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", android_logo[i]);
			}
		}

		else if (STRCMP(distro_str, "Angstrom"))
		{
			for (i = 0; i < 16; i++)
			{
				printf("%s %s%s\n", angstrom_logo[i], data_names[i], data[i]);
			}
		}

		else /* if (STRCMP(distro_str, "Linux")) */
		{
			for (i = 0; i < 18; i++)
			{
				if (i < 16)
					printf("%s %s%s%s%s\n", linux_logo[i], TLGY, data_names[i], TNRM, data[i]);
				else
					printf("%s\n", linux_logo[i]);
			}
		}
	}

	else if (OS == FREEBSD)
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s\n", freebsd_logo[i], TLRD, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", freebsd_logo[i]);
		}
	}

	else if (OS == OPENBSD)
	{
		for (i = 0; i < 23; i++)
		{
			if (i < 16)
				printf("%s %s%s\n", openbsd_logo[i], data_names[i], data[i]);
			else
				printf("%s\n", openbsd_logo[i]);
		}
	}

	else if (OS == NETBSD)
	{
		for (i = 0; i < 23; i++)
		{
			if (i < 16)
				printf("%s %s%s\n", netbsd_logo[i], data_names[i], data[i]);
			else
				printf("%s\n", netbsd_logo[i]);
		}
	}

	else if (OS == DFBSD)
	{
		for (i = 0; i < 23; i++)
		{
			if (i < 16)
				printf("%s %s%s\n", dragonflybsd_logo[i], data_names[i], data[i]);
			else
				printf("%s\n", dragonflybsd_logo[i]);
		}
	}

	else if (OS == SOLARIS)
	{
		for (i = 0; i < 17; i++)
		{
			if (i < 16)
				printf("%s %s%s\n", solaris_logo[i], data_names[i], data[i]);
			else
				printf("%s\n", solaris_logo[i]);
		}
	}

	return;
}

/*  main_text_output
	the secondary output for screenfetch-c - all info WITHOUT ASCII art is printed here
	arguments char* data[], char* data_names[]: string arrays containing the names and data acquired
*/
void main_text_output(char* data[], char* data_names[])
{
	int i;

	for (i = 0; i < 16; i++)
		printf("%s %s\n", data_names[i], data[i]);

	return;
}

/*  display_version
    called if the -v flag is tripped, outputs the current version of screenfetch-c
*/
void display_version(void)
{
	printf("%s\n", "screenfetch-c - Version 1.2, revision 3");
	return;
}

/*  display_help
    called if the -h flag is tripped, tells the user where to find the manpage
*/
void display_help(void)
{
	printf("%s\n", "screenfetch-c");
	printf("%s\n", "A rewrite of screenFetch, the popular shell script, in C.");
	printf("%s\n", "Operating Systems currently supported:");
	printf("%s\n", "Windows (via Cygwin), Linux, *BSD, OS X, and Solaris.");
	printf("%s\n", "Using screenfetch-c on an OS not listed above may not work entirely or at all (and is disabled by default).");
	printf("%s\n", "Please access 'man screenfetch' for in-depth information on compatibility and usage.");
	return;
}

/*  take_screenshot
    takes a screenshot and saves it to $HOME/screenfetch_screenshot.jpg
    --
    CAVEAT: THIS FUNCTION MAKES SYSTEM CALLS
    --
*/
void take_screenshot(void)
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

	if (OS == CYGWIN)
	{
		#ifdef __CYGWIN__
			/* terrible hack, the printscreen key is simulated */
			keybd_event(VK_SNAPSHOT, 0, 0, 0);
			
			if (verbose)
				VERBOSE_OUT("Screenshot has been saved to the clipboard.", "");
			

			/* NOT FINSISHED - HBITMAP needs to be saved
			HDC screen_dc = GetDC(NULL);
			HDC mem_dc = CreateCompatibleDC(screen_dc);

			int horiz = GetDeviceCaps(screen_dc, HORZRES);
			int vert = GetDeviceCaps(screen_dc, VERTRES);

			HBITMAP bitmap = CreateCompatibleBitmap(screen_dc, horiz, vert);
			HBITMAP old_bitmap = SelectObject(mem_dc, bitmap);

			BitBlt(mem_dc, 0, 0, horiz, vert, screen_dc, 0, 0, SRCCOPY);
			bitmap = SelectObject(mem_dc, old_bitmap);

			DeleteDC(screen_dc);
			DeleteDC(mem_dc);
			*/
		#endif
	}

	else
	{
		if (OS == OSX)
		{
			system("screencapture -x ~/screenfetch_screenshot.png 2> /dev/null");	
		}

		else if (OS == LINUX || ISBSD())
		{
			system("scrot ~/screenfetch_screenshot.png 2> /dev/null");
		}

		char* loc = getenv("HOME");
		strncat(loc, "/screenfetch_screenshot.png", MAX_STRLEN);

		if (FILE_EXISTS(loc) && verbose)
		{
			VERBOSE_OUT("Screenshot successfully saved.", "");
		}
			
		else if (verbose)
		{
			ERROR_OUT("Error: ", "Problem saving screenshot.");
		}
	}

	return;
}

/*  **  END FLAG/OUTPUT/MISC FUNCTIONS  **  */

/* EOF */
