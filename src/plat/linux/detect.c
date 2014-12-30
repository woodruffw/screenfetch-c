/*	detect.c
 *	Author: William Woodruff
 *	-------------
 *
 *	The detection functions used by screenfetch-c on Linux are implemented here.
 *	Like the rest of screenfetch-c, this file is licensed under the MIT license.
 */

/* standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>

/* linux-specific includes */
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <pwd.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <glob.h>

/* program includes */
#include "../../misc.h"
#include "../../disp.h"
#include "../../util.h"
#include "../../error_flag.h"

/*	detect_distro
	detects the computer's distribution (really only relevant on Linux)
	argument char *str: the char array to be filled with the distro name
*/
void detect_distro(char *str)
{
	/* if distro_str was NOT set by the -D flag or manual mode */
	if (STREQ(str, "Unknown") || STREQ(str, "*"))
	{
		FILE *distro_file;

		char distro_name_str[MAX_STRLEN];

		if (FILE_EXISTS("/system/bin/getprop"))
		{
			safe_strncpy(str, "Android", MAX_STRLEN);
		}
		else
		{
			bool detected = false;

			/* Bad solution, as /etc/issue contains junk on some distros */
			distro_file = fopen("/etc/issue", "r");

			if (distro_file != NULL)
			{
				/* get the first 4 chars, that's all we need */
				fscanf(distro_file, "%4s", distro_name_str);
				fclose(distro_file);

				if (STREQ(distro_name_str, "Kali"))
				{
					safe_strncpy(str, "Kali Linux", MAX_STRLEN);
					detected = true;
				}
				else if (STREQ(distro_name_str, "Back"))
				{
					safe_strncpy(str, "Backtrack Linux", MAX_STRLEN);
					detected = true;
				}
				else if (STREQ(distro_name_str, "Crun"))
				{
					safe_strncpy(str, "CrunchBang", MAX_STRLEN);
					detected = true;
				}
				else if (STREQ(distro_name_str, "LMDE"))
				{
					safe_strncpy(str, "LMDE", MAX_STRLEN);
					detected = true;
				}
				else if (STREQ(distro_name_str, "Debi")
						|| STREQ(distro_name_str, "Rasp"))
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
				else if (FILE_EXISTS("/etc/manjaro-release"))
				{
					safe_strncpy(str, "Manjaro", MAX_STRLEN);
				}
				else if (FILE_EXISTS("/etc/lsb-release"))
				{
					distro_file = fopen("/etc/lsb-release", "r");
					fscanf(distro_file, "%s ", distro_name_str);
					fclose(distro_file);

					snprintf(str, MAX_STRLEN, "%s", distro_name_str + 11);
				}
				else if (FILE_EXISTS("/etc/os-release"))
				{
					/*
						TODO: Parse NAME or PRETTY_NAME from os-release
						Until then, spit out an error message.
					*/
					if (error)
						ERR_REPORT("Failed to detect a Linux distro (1).");
				}
				else
				{
					safe_strncpy(str, "Linux", MAX_STRLEN);

					if (error)
					{
						ERR_REPORT("Failed to detect a Linux distro (2).");
					}
				}
			}
		}
	}

	return;
}

/*	detect_arch
	detects the computer's architecture
	argument char *str: the char array to be filled with the architecture
*/
void detect_arch(char *str)
{
	struct utsname arch_info;
	uname(&arch_info);
	safe_strncpy(str, arch_info.machine, MAX_STRLEN);

	return;
}

/*	detect_host
	detects the computer's hostname and active user and formats them
	argument char *str: the char array to be filled with the host info
*/
void detect_host(char *str)
{
	char given_user[MAX_STRLEN] = "Unknown";
	char given_host[MAX_STRLEN] = "Unknown";
	struct passwd *user_info;
	struct utsname host_info;

	if ((user_info = getpwuid(geteuid())))
	{
		safe_strncpy(given_user, user_info->pw_name, MAX_STRLEN);
	}
	else if (error)
	{
		ERR_REPORT("Could not detect username.");
	}
	
	if (!(uname(&host_info)))
	{
		safe_strncpy(given_host, host_info.nodename, MAX_STRLEN);
	}
	else if (error)
	{
		ERR_REPORT("Could not detect hostname.");
	}

	snprintf(str, MAX_STRLEN, "%s@%s", given_user, given_host);

	return;
}

/*	detect_kernel
	detects the computer's kernel
	argument char *str: the char array to be filled with the kernel name
*/
void detect_kernel(char *str)
{
	struct utsname kern_info;

	if (!(uname(&kern_info)))
	{
		snprintf(str, MAX_STRLEN, "%s %s", kern_info.sysname, kern_info.release);
	}
	else if (error)
	{
		ERR_REPORT("Could not detect kernel information.");
		safe_strncpy(str, "Linux", MAX_STRLEN);
	}

	return;
}

/*	detect_uptime
	detects the computer's uptime
	argument char *str: the char array to be filled with the uptime
*/
void detect_uptime(char *str)
{
	int secs = 0;
	int mins = 0;
	int hrs = 0;
	int days = 0;
	struct sysinfo si_upt;

	if (!(sysinfo(&si_upt)))
	{
		split_uptime(si_upt.uptime, &secs, &mins, &hrs, &days);

		if (days > 0)
			snprintf(str, MAX_STRLEN, "%dd %dh %dm %ds", days, hrs, mins, secs);
		else
			snprintf(str, MAX_STRLEN, "%dh %dm %ds", hrs, mins, secs);
	}
	else
	{
		ERR_REPORT("Could not detect system uptime.");
	}

	return;
}

/*	detect_pkgs
	detects the number of packages installed on the computer
	argument char *str: the char array to be filled with the number of packages
*/
void detect_pkgs(char *str, const char *distro_str)
{
	FILE *pkgs_file;
	int packages = 0;
	glob_t gl;

	if (STREQ(distro_str, "Arch Linux")
		|| STREQ(distro_str, "ParabolaGNU/Linux-libre")
		|| STREQ(distro_str, "Chakra") || STREQ(distro_str, "Manjaro"))
	{
		if (!(glob("/var/lib/pacman/local/*", GLOB_NOSORT, NULL, &gl)))
		{
			packages = gl.gl_pathc;
		}
		else if (error)
		{
			ERR_REPORT("Failure while globbing packages.");
		}

		globfree(&gl);
	}
	else if (STREQ(distro_str, "Frugalware"))
	{
		pkgs_file = popen("pacman-g2 -Q 2> /dev/null | wc -l", "r");
		fscanf(pkgs_file, "%d", &packages);
		pclose(pkgs_file);
	}
	else if (STREQ(distro_str, "Ubuntu") || STREQ(distro_str, "Lubuntu")
			|| STREQ(distro_str, "Xubuntu") || STREQ(distro_str, "LinuxMint")
			|| STREQ(distro_str, "SolusOS") || STREQ(distro_str, "Debian")
			|| STREQ(distro_str, "LMDE") || STREQ(distro_str, "CrunchBang")
			|| STREQ(distro_str, "Peppermint")
			|| STREQ(distro_str, "LinuxDeepin")	|| STREQ(distro_str, "Trisquel")
			|| STREQ(distro_str, "elementary OS")
			|| STREQ(distro_str, "Backtrack Linux")
			|| STREQ(distro_str, "Kali Linux"))
	{
		if (!(glob("/var/lib/dpkg/info/*.list", GLOB_NOSORT, NULL, &gl)))
		{
			packages = gl.gl_pathc;
		}
		else if (error)
		{
			ERR_REPORT("Failure while globbing packages.");
		}

		globfree(&gl);
	}
	else if (STREQ(distro_str, "Slackware"))
	{
		if (!(glob("/var/log/packages/*", GLOB_NOSORT, NULL, &gl)))
		{
			packages = gl.gl_pathc;
		}
		else if (error)
		{
			ERR_REPORT("Failure while globbing packages.");
		}

		globfree(&gl);
	}
	else if (STREQ(distro_str, "Gentoo") || STREQ(distro_str, "Sabayon") 
			|| STREQ(distro_str, "Funtoo"))
	{
		if (!(glob("/var/db/pkg/*/*", GLOB_NOSORT, NULL, &gl)))
		{
			packages = gl.gl_pathc;
		}
		else if (error)
		{
			ERR_REPORT("Failure while globbing packages.");
		}

		globfree(&gl);
	}
	else if (STREQ(distro_str, "Fuduntu") || STREQ(distro_str, "Fedora")
			|| STREQ(distro_str, "OpenSUSE")
			|| STREQ(distro_str, "Red Hat Linux")
			|| STREQ(distro_str, "Mandriva") || STREQ(distro_str, "Mandrake")
			|| STREQ(distro_str, "Mageia") || STREQ(distro_str, "Viperr"))
	{
		/* RPM uses Berkeley DBs internally, so this won't change soon */
		pkgs_file = popen("rpm -qa 2> /dev/null | wc -l", "r");
		fscanf(pkgs_file, "%d", &packages);
		pclose(pkgs_file);
	}
	else if (STREQ(distro_str, "Angstrom"))
	{
		pkgs_file = popen("opkg list-installed 2> /dev/null | wc -l", "r");
		fscanf(pkgs_file, "%d", &packages);
		pclose(pkgs_file);
	}
	else if (STREQ(distro_str, "Linux")) /* if linux disto detection failed */
	{
		safe_strncpy(str, "Not Found", MAX_STRLEN);

		if (error)
			ERR_REPORT("Packages cannot be detected on an unknown "
						"Linux distro.");
	}

	snprintf(str, MAX_STRLEN, "%d", packages);

	return;
}

/*	detect_cpu
	detects the computer's CPU brand/name-string
	argument char *str: the char array to be filled with the CPU name
*/
void detect_cpu(char *str)
{
	FILE *cpu_file;
	char cpuinfo_buf[MAX_STRLEN];
	char *cpuinfo_str;
	int end;

	if ((cpu_file = fopen("/proc/cpuinfo", "r")))
	{
		/* read past the first 4 lines (#5 is model name) */
		for (int i = 0; i < 5; i++)
		{
			if (!(fgets(cpuinfo_buf, MAX_STRLEN, cpu_file)))
			{
				ERR_REPORT("Fatal error while reading /proc/cpuinfo");
				return;
			}
		}

		/* fail to match a colon. this should never happen, but check anyways */
		if (!(cpuinfo_str = strchr(cpuinfo_buf, ':')))
		{
			ERR_REPORT("Fatal error matching in /proc/cpuinfo");
			return;
		}

		cpuinfo_str += 2;
		end = strlen(cpuinfo_str);

		if (cpuinfo_str[end - 1] == '\n')
			cpuinfo_str[end - 1] = '\0';

		if (STREQ(cpuinfo_str, "ARMv6-compatible processor rev 7 (v6l)"))
		{
			safe_strncpy(str, "BCM2708 (Raspberry Pi)", MAX_STRLEN);
		}
		else
		{
			safe_strncpy(str, cpuinfo_str, MAX_STRLEN);
		}
	}
	else if (error)
	{
		ERR_REPORT("Failed to open /proc/cpuinfo. Ancient Linux kernel?");
	}

	return;
}

/*	detect_gpu
	detects the computer's GPU brand/name-string
	argument char *str: the char array to be filled with the GPU name
*/
void detect_gpu(char *str)
{
	Display *disp = NULL;
	Window wind;
	GLint attr[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	XVisualInfo *visual_info = NULL;
	GLXContext context = NULL;

	if ((disp = XOpenDisplay(NULL)))
	{
		wind = DefaultRootWindow(disp);

		if ((visual_info = glXChooseVisual(disp, 0, attr)))
		{
			if ((context = glXCreateContext(disp, visual_info, NULL, GL_TRUE)))
			{
				glXMakeCurrent(disp, wind, context);
				safe_strncpy(str, (const char *) glGetString(GL_RENDERER),
						MAX_STRLEN);

				glXDestroyContext(disp, context);
			}
			else if (error)
			{
				ERR_REPORT("Failed to create OpenGL context.");
			}

			XFree((void *) visual_info);
		}
		else if (error)
		{
			ERR_REPORT("Failed to select a proper X visual.");
		}

		XCloseDisplay(disp);
	}
	else if (error)
	{
		safe_strncpy(str, "No X Server", MAX_STRLEN);
		ERR_REPORT("Could not open an X display (detect_gpu).");
	}

	return;
}

/*	detect_disk
	detects the computer's total disk capacity and usage
	argument char *str: the char array to be filled with the disk data
*/
void detect_disk(char *str)
{
	struct statvfs disk_info;
	unsigned long disk_total = 0, disk_used = 0, disk_percentage = 0;

	if (!(statvfs(getenv("HOME"), &disk_info)))
	{
		disk_total = ((disk_info.f_blocks * disk_info.f_bsize) / GB);
		disk_used = (((disk_info.f_blocks - disk_info.f_bfree) * disk_info.f_bsize) / GB);
		disk_percentage = (((float) disk_used / disk_total) * 100);
		snprintf(str, MAX_STRLEN, "%ldG / %ldG (%ld%%)", disk_used, disk_total, disk_percentage);
	}
	else if (error)
	{
		ERR_REPORT("Could not stat $HOME for filesystem statistics.");
	}

	return;
}

/*	detect_mem
	detects the computer's total and used RAM
	argument char *str: the char array to be filled with the memory data
*/
void detect_mem(char *str)
{
	long long total_mem = 0;
	long long free_mem = 0;
	long long used_mem = 0;
	struct sysinfo si_mem;

	/* known problem: because linux utilizes free ram in caches/buffers,
	   the amount of memory sysinfo reports as free is very small.
	*/
	sysinfo(&si_mem);

	total_mem = (long long) (si_mem.totalram * si_mem.mem_unit) / MB;
	free_mem = (long long) (si_mem.freeram * si_mem.mem_unit) / MB;
	used_mem = (long long) total_mem - free_mem;

	snprintf(str, MAX_STRLEN, "%lld%s / %lld%s", used_mem, "MB", total_mem, "MB");

	return;
}

/*	detect_shell
	detects the shell currently running on the computer
	argument char *str: the char array to be filled with the shell name and version
	--
	CAVEAT: shell version detection relies on the standard versioning format for 
	each shell. If any shell's older (or newer versions) suddenly begin to use a new
	scheme, the version may be displayed incorrectly.
	--
*/
void detect_shell(char *str)
{
	FILE *shell_file;
	char *shell_name;
	char vers_str[MAX_STRLEN];

	if (!(shell_name = getenv("SHELL")))
	{
		if (error)
			ERR_REPORT("Could not detect a shell - $SHELL not defined.");

		return;
	}

	if (STREQ(shell_name, "/bin/sh"))
	{
		safe_strncpy(str, "POSIX sh", MAX_STRLEN);
	}
	else if (strstr(shell_name, "bash"))
	{
		shell_file = popen("bash --version | head -1", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);
		snprintf(str, MAX_STRLEN, "bash %.*s", 17, vers_str + 10);
		pclose(shell_file);
	}
	else if (strstr(shell_name, "zsh"))
	{
		shell_file = popen("zsh --version", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);	
		snprintf(str, MAX_STRLEN, "zsh %.*s", 5, vers_str + 4);
		pclose(shell_file);
	}
	else if (strstr(shell_name, "csh"))
	{
		shell_file = popen("csh --version | head -1", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);
		snprintf(str, MAX_STRLEN, "csh %.*s", 7, vers_str + 5);
		pclose(shell_file);
	}
	else if (strstr(shell_name, "fish"))
	{
		shell_file = popen("fish --version", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);
		snprintf(str, MAX_STRLEN, "fish %.*s", 13, vers_str + 6);
		pclose(shell_file);
	}
	else if (strstr(shell_name, "dash") || strstr(shell_name, "ash")
			|| strstr(shell_name, "ksh"))
	{
		/* i don't have a version detection system for these, yet */
		safe_strncpy(str, shell_name, MAX_STRLEN);
	}

	return;
}

/*	detect_res
	detects the combined resolution of all monitors attached to the computer
	argument char *str: the char array to be filled with the resolution
*/
void detect_res(char *str)
{
	int width = 0;
	int height = 0;
	Display *disp;
	Screen *screen;

	if ((disp = XOpenDisplay(NULL)))
	{
		screen = XDefaultScreenOfDisplay(disp);
		width = WidthOfScreen(screen);
		height = HeightOfScreen(screen);

		snprintf(str, MAX_STRLEN, "%dx%d", width, height);

		XCloseDisplay(disp);
	}
	else
	{
		safe_strncpy(str, "No X Server", MAX_STRLEN);

		if (error)
			ERR_REPORT("Could not open an X display (detect_res)");
	}

	return;
}

/*	detect_de
	detects the desktop environment currently running on top of the OS
	argument char *str: the char array to be filled with the DE name
*/
void detect_de(char *str)
{
	char *curr_de;

	if ((curr_de = getenv("XDG_CURRENT_DESKTOP")))
	{
		safe_strncpy(str, curr_de, MAX_STRLEN);
	}
	else
	{
		if (getenv("GNOME_DESKTOP_SESSION_ID"))
		{
			safe_strncpy(str, "Gnome", MAX_STRLEN);
		}
		else if (getenv("MATE_DESKTOP_SESSION_ID"))
		{
			safe_strncpy(str, "MATE", MAX_STRLEN);
		}
		else if (getenv("KDE_FULL_SESSION"))
		{
			/*	KDE_SESSION_VERSION only exists on KDE4+, so 
				getenv will return NULL on KDE <= 3.
			 */
			snprintf(str, MAX_STRLEN, "KDE%s", getenv("KDE_SESSION_VERSION"));
		}
		else if (error)
		{
			ERR_REPORT("No desktop environment found.");
		}
	}

	return;
}

/*	detect_wm
	detects the window manager currently running on top of the OS
	argument char *str: the char array to be filled with the WM name
*/
void detect_wm(char *str)
{
	Display *disp;
	Atom actual_type;
	int actual_format;
	unsigned long nitems;
	unsigned long bytes;
	char *wm_name = '\0';
	Window *wm_check_window;

	if ((disp = XOpenDisplay(NULL)))
	{
		if (!(XGetWindowProperty(disp, DefaultRootWindow(disp),
			XInternAtom(disp, "_NET_SUPPORTING_WM_CHECK", true),
			0, KB, false,	XA_WINDOW, &actual_type, &actual_format, &nitems,
			&bytes, (unsigned char **) &wm_check_window)))
		{
			if (!(XGetWindowProperty(disp, *wm_check_window,
				XInternAtom(disp, "_NET_WM_NAME", true), 0, KB, false,
				XInternAtom(disp, "UTF8_STRING", true),	&actual_type,
				&actual_format, &nitems, &bytes, (unsigned char **) &wm_name)))
			{
				safe_strncpy(str, wm_name, MAX_STRLEN);
				XFree(wm_name);
			}
			else if (error)
			{
				ERR_REPORT("No _NET_WM_NAME property found.");
			}

			XFree(wm_check_window);
		}
		else if (error)
		{
			ERR_REPORT("No WM detected (non-EWMH compliant?)");
		}

		XCloseDisplay(disp);
	}
	else if (error)
	{
		ERR_REPORT("Could not open an X display. (detect_wm)");
	}

	return;
}

/*	detect_wm_theme
	detects the theme associated with the WM detected in detect_wm()
	argument char *str: the char array to be filled with the WM Theme name
	--
	CAVEAT: This function relies on the presence of 'detectwmtheme', a shell script. 
	If it isn't present somewhere in the PATH, the WM Theme will be set as 'Unknown'
	--
*/
void detect_wm_theme(char *str, const char *wm_str)
{
	char exec_str[MAX_STRLEN];
	FILE *wm_theme_file;

	snprintf(exec_str, MAX_STRLEN, "detectwmtheme %s 2> /dev/null", wm_str);

	wm_theme_file = popen(exec_str, "r");
	fgets(str, MAX_STRLEN, wm_theme_file);
	pclose(wm_theme_file);

	return;
}

/*	detect_gtk
	detects the theme, icon(s), and font(s) associated with a GTK DE (if present)
	argument char *str: the char array to be filled with the GTK info
	--
	CAVEAT: This function relies on the presence of 'detectgtk', a shell script.
	If it isn't present somewhere in the PATH, the GTK will be set as 'Unknown'
	--
*/
void detect_gtk(char *str)
{
	FILE *gtk_file;
	char gtk2_str[MAX_STRLEN] = "Unknown";
	char gtk3_str[MAX_STRLEN] = "Unknown";
	char gtk_icons_str[MAX_STRLEN] = "Unknown";
	char font_str[MAX_STRLEN] = "Unknown";

	gtk_file = popen("detectgtk 2> /dev/null", "r");
	fscanf(gtk_file, "%s%s%s%s", gtk2_str, gtk3_str, gtk_icons_str, font_str);
	pclose(gtk_file);

	if (STREQ(gtk3_str, "Unknown"))
		snprintf(str, MAX_STRLEN, "%s (GTK2), %s (Icons)", gtk2_str,
				gtk_icons_str);
	else if (STREQ(gtk2_str, "Unknown"))
		snprintf(str, MAX_STRLEN, "%s (GTK3), %s (Icons)", gtk3_str,
				gtk_icons_str);
	else
		snprintf(str, MAX_STRLEN, "%s (GTK2), %s (GTK3)", gtk2_str, gtk3_str);

	return;
}
