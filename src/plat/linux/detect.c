/*	detect.c
	Author: William Woodruff
	-------------

	The detection functions used by screenfetch-c on Linux are implemented here.
	Like the rest of screenfetch-c, this file is licensed under the MIT license.
	You should have received a copy of it with this code.
*/

/* standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

/* linux-specific includes */
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <glob.h>

/* program includes */
#include "../../misc.h"
#include "../../disp.h"
#include "../../util.h"

/*	detect_distro
	detects the computer's distribution (really only relevant on Linux)
	argument char *str: the char array to be filled with the distro name
	argument bool error: true for verbose errors, false for silent errors
*/
void detect_distro(char *str, bool error)
{
	if (STRCMP(str, "Unknown") || STRCMP(str, "*")) /* if distro_str was NOT set by the -D flag or manual mode */
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
				else if (FILE_EXISTS("/etc/manjaro-release"))
				{
					safe_strncpy(str, "Manjaro", MAX_STRLEN);
				}
				else if (FILE_EXISTS("/etc/lsb-release"))
				{
					distro_file = fopen("/etc/lsb-release", "r");
					fgets(distro_name_str, MAX_STRLEN, distro_file);
					distro_name_str[strlen(distro_name_str) - 1] = '\0';
					fclose(distro_file);

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
	argument char *str: the char array to be filled with the user and hostname in format 'user@host'
*/
void detect_host(char *str)
{
	char *given_user = "Unknown";
	char given_host[MAX_STRLEN] = "Unknown";

	given_user = getlogin(); /* getlogin is apparently buggy on linux, so this might be changed */

	struct utsname host_info;
	uname(&host_info);
	safe_strncpy(given_host, host_info.nodename, MAX_STRLEN);

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
	uname(&kern_info);
	snprintf(str, MAX_STRLEN, "%s %s", kern_info.sysname, kern_info.release);

	return;
}

/*	detect_uptime
	detects the computer's uptime
	argument char *str: the char array to be filled with the uptime in format '$d $h $m $s' where $ is a number
*/
void detect_uptime(char *str)
{
	long uptime = 0;

	int secs = 0;
	int mins = 0;
	int hrs = 0;
	int days = 0;

	struct sysinfo si_upt;
	sysinfo(&si_upt);

	uptime = si_upt.uptime;

	split_uptime(uptime, &secs, &mins, &hrs, &days);

	if (days > 0)
		snprintf(str, MAX_STRLEN, "%dd %dh %dm %ds", days, hrs, mins, secs);
	else
		snprintf(str, MAX_STRLEN, "%dh %dm %ds", hrs, mins, secs);

	return;
}

/*	detect_pkgs
	detects the number of packages installed on the computer
	argument char *str: the char array to be filled with the number of packages
*/
void detect_pkgs(char *str, const char *distro_str, bool error)
{
	FILE *pkgs_file;

	int packages = 0;

	if (STRCMP(distro_str, "Arch Linux") || STRCMP(distro_str, "ParabolaGNU/Linux-libre") || STRCMP(distro_str, "Chakra") || STRCMP(distro_str, "Manjaro"))
	{
		#if defined(__linux__)
			glob_t gl;

			if (glob("/var/lib/pacman/local/*", GLOB_NOSORT, NULL, &gl) == 0)
			{
				packages = gl.gl_pathc;
			}
			else if (error)
			{
				ERROR_OUT("Error: ", "Failure while globbing packages.");
			}

			globfree(&gl);
		#endif
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
		#if defined(__linux__)
			glob_t gl;

			if (glob("/var/log/packages/*", GLOB_NOSORT, NULL, &gl) == 0)
			{
				packages = gl.gl_pathc;
			}
			else if (error)
			{
				ERROR_OUT("Error: ", "Failure while globbing packages.");
			}

			globfree(&gl);
		#endif
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

	cpu_file = popen("awk 'BEGIN{FS=\":\"} /model name/ { print $2; exit }' /proc/cpuinfo | sed -e 's/ @/\\n/' -e 's/^ *//g' -e 's/ *$//g' | head -1 | tr -d '\\n'", "r");
	fgets(str, MAX_STRLEN, cpu_file);
	pclose(cpu_file);

	if (STRCMP(str, "ARMv6-compatible processor rev 7 (v6l)"))
	{
		safe_strncpy(str, "BCM2708 (Raspberry Pi)", MAX_STRLEN); /* quick patch for the Raspberry Pi */
	}

	return;
}

/*	detect_gpu
	detects the computer's GPU brand/name-string
	argument char *str: the char array to be filled with the GPU name
	argument bool error: true for verbose errors, false for silent
*/
void detect_gpu(char *str, bool error)
{
	Display *disp = NULL;
	Window wind = NULL;
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
				safe_strncpy(str, (const char *) glGetString(GL_RENDERER), MAX_STRLEN);
			}
			else if (error)
			{
				ERROR_OUT("Error: ", "Failed to create OpenGL context.");
			}
		}
		else if (error)
		{
			ERROR_OUT("Error: ", "Failed to select a proper X visual.");
		}
	}
	else if (error)
	{
		safe_strncpy(str, "No X Server", MAX_STRLEN);
		ERROR_OUT("Error: ", "Could not open an X display.");
	}

	/* cleanup */
	if (context)
		glXDestroyContext(disp, context);
	if (visual_info)
		XFree((void *) visual_info);
	if (disp)
		XCloseDisplay(disp);

	return;
}

/*	detect_disk
	detects the computer's total disk capacity and usage
	argument char *str: the char array to be filled with the disk data in format '$G / $G ($G%)', where $ is a number
*/
void detect_disk(char *str)
{
	FILE *disk_file;

	int disk_total = 0;
	int disk_used = 0;
	int disk_percentage = 0;

	disk_file = popen("df -H 2> /dev/null | grep -vE '^[A-Z]\\:\\/|File' | awk '{ print $2 }' | head -1 | tr -d '\\r\\n G'", "r");
	fscanf(disk_file, "%d", &disk_total);
	pclose(disk_file);

	disk_file = popen("df -H 2> /dev/null | grep -vE '^[A-Z]\\:\\/|File' | awk '{ print $3 }' | head -1 | tr -d '\\r\\n G'", "r");
	fscanf(disk_file, "%d", &disk_used);
	pclose(disk_file);

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

	return;
}

/*	detect_mem
	detects the computer's total and used RAM
	argument char *str: the char array to be filled with the memory data in format '$MB / $MB', where $ is a number
*/
void detect_mem(char *str)
{
	long long total_mem = 0; /* each of the following MAY contain bytes/kbytes/mbytes/pages */
	long long free_mem = 0;
	long long used_mem = 0;

	/* known problem: because linux utilizes free ram extensively in caches/buffers,
	   the amount of memory sysinfo reports as free is very small.
	*/
	struct sysinfo si_mem;
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
	argument bool error: true for verbose errors, false for silent
	--
	CAVEAT: shell version detection relies on the standard versioning format for 
	each shell. If any shell's older (or newer versions) suddenly begin to use a new
	scheme, the version may be displayed incorrectly.
	--
*/
void detect_shell(char *str, bool error)
{
	FILE *shell_file;

	char *shell_name;
	char vers_str[MAX_STRLEN];

	shell_name = getenv("SHELL");

	if (shell_name == NULL)
	{
		if (error)
			ERROR_OUT("Error: ", "Problem detecting shell.");

		return;
	}

	if (strstr(shell_name, "bash"))
	{
		shell_file = popen("bash --version | head -1", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);
		/* evil pointer arithmetic */
		snprintf(str, MAX_STRLEN, "bash %.*s", 17, vers_str + 10);
		pclose(shell_file);
	}

	else if (strstr(shell_name, "zsh"))
	{
		shell_file = popen("zsh --version", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);	
		/* evil pointer arithmetic */
		snprintf(str, MAX_STRLEN, "zsh %.*s", 5, vers_str + 4);
		pclose(shell_file);
	}

	else if (strstr(shell_name, "csh"))
	{
		shell_file = popen("csh --version | head -1", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);
		/* evil pointer arithmetic */
		snprintf(str, MAX_STRLEN, "csh %.*s", 7, vers_str + 5);
		pclose(shell_file);
	}

	else if (strstr(shell_name, "fish"))
	{
		shell_file = popen("fish --version", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);
		/* evil pointer arithmetic */
		snprintf(str, MAX_STRLEN, "fish %.*s", 13, vers_str + 6);
		pclose(shell_file);
	}

	else if (strstr(shell_name, "dash") || strstr(shell_name, "ash") || strstr(shell_name, "ksh"))
	{
		/* i don't have a version detection system for these, yet */
		safe_strncpy(str, shell_name, MAX_STRLEN);
	}

	return;
}

/*	detect_res
	detects the combined resolution of all monitors attached to the computer
	argument char *str: the char array to be filled with the resolution in format '$x$', where $ is a number
	argument bool error: true for verbose errors, false for silent
*/
void detect_res(char *str, bool error)
{
	int width = 0;
	int height = 0;

	Display *disp;

	if ((disp = XOpenDisplay(NULL)))
	{
		Screen *screen = XDefaultScreenOfDisplay(disp);
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

	XCloseDisplay(disp);

	return;
}

/*	detect_de
	detects the desktop environment currently running on top of the OS
	argument char *str: the char array to be filled with the DE name
	--
	CAVEAT: This function relies on the presence of 'detectde', a shell script. 
	If it isn't present somewhere in the PATH, the WM Theme will be set as 'Unknown'
	--
*/
void detect_de(char *str)
{
	FILE *de_file;

	de_file = popen("detectde 2> /dev/null", "r");
	fgets(str, MAX_STRLEN, de_file);
	pclose(de_file);

	return;
}

/*	detect_wm
	detects the window manager currently running on top of the OS
	argument char *str: the char array to be filled with the WM name
	--
	CAVEAT: This function relies on the presence of 'detectwm', a shell script. 
	If it isn't present somewhere in the PATH, the WM Theme will be set as 'Unknown'
	--
*/
void detect_wm(char *str)
{
	FILE *wm_file;

	wm_file = popen("detectwm 2> /dev/null", "r");
	fgets(str, MAX_STRLEN, wm_file);
	pclose(wm_file);

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
void detect_wm_theme(char *str)
{
	FILE *wm_theme_file;

	wm_theme_file = popen("detectwmtheme 2> /dev/null", "r");
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

	if (STRCMP(gtk3_str, "Unknown"))
		snprintf(str, MAX_STRLEN, "%s (GTK2), %s (Icons)", gtk2_str, gtk_icons_str);
	else if (STRCMP(gtk2_str, "Unknown"))
		snprintf(str, MAX_STRLEN, "%s (GTK3), %s (Icons)", gtk3_str, gtk_icons_str);
	else
		snprintf(str, MAX_STRLEN, "%s (GTK2), %s (GTK3)", gtk2_str, gtk3_str);

	return;
}