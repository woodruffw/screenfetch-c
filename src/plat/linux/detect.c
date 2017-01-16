/*	detect.c
**	Author: William Woodruff
**	-------------
**
**	The detection functions used by screenfetch-c on Linux are implemented here.
**	Like the rest of screenfetch-c, this file is licensed under the MIT license.
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
#include <mntent.h>

/* program includes */
#include "../../arrays.h"
#include "../../colors.h"
#include "../../misc.h"
#include "../../disp.h"
#include "../../util.h"
#include "../../error_flag.h"

/*	detect_distro
	detects the computer's distribution (really only relevant on Linux)
*/
void detect_distro(void)
{
	/* if distro_str was NOT set by the -D flag */
	if (STREQ(distro_str, "Unknown"))
	{
		FILE *distro_file;

		char distro_name_str[MAX_STRLEN];

		if (FILE_EXISTS("/system/bin/getprop"))
		{
			safe_strncpy(distro_str, "Android", MAX_STRLEN);
			safe_strncpy(host_color, TLGN, MAX_STRLEN);
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
					safe_strncpy(distro_str, "Kali Linux", MAX_STRLEN);
					detected = true;
					safe_strncpy(host_color, TLBL, MAX_STRLEN);
				}
				else if (STREQ(distro_name_str, "Back"))
				{
					safe_strncpy(distro_str, "Backtrack Linux", MAX_STRLEN);
					detected = true;
					safe_strncpy(host_color, TLRD, MAX_STRLEN);
				}
				else if (STREQ(distro_name_str, "Crun"))
				{
					safe_strncpy(distro_str, "CrunchBang", MAX_STRLEN);
					detected = true;
					safe_strncpy(host_color, TDGY, MAX_STRLEN);
				}
				else if (STREQ(distro_name_str, "LMDE"))
				{
					safe_strncpy(distro_str, "LMDE", MAX_STRLEN);
					detected = true;
					safe_strncpy(host_color, TLGN, MAX_STRLEN);
				}
				else if (STREQ(distro_name_str, "Debi")
						|| STREQ(distro_name_str, "Rasp"))
				{
					safe_strncpy(distro_str, "Debian", MAX_STRLEN);
					detected = true;
					safe_strncpy(host_color, TLRD, MAX_STRLEN);
				}
			}

			if (!detected)
			{
				if (FILE_EXISTS("/etc/redhat-release"))
				{
					safe_strncpy(distro_str, "Red Hat Linux", MAX_STRLEN);
					safe_strncpy(host_color, TLRD, MAX_STRLEN);
				}
				else if (FILE_EXISTS("/etc/fedora-release"))
				{
					safe_strncpy(distro_str, "Fedora", MAX_STRLEN);
					safe_strncpy(host_color, TLBL, MAX_STRLEN);
				}
				else if (FILE_EXISTS("/etc/SuSE-release"))
				{
					safe_strncpy(distro_str, "OpenSUSE", MAX_STRLEN);
					safe_strncpy(host_color, TLGN, MAX_STRLEN);
				}
				else if (FILE_EXISTS("/etc/arch-release"))
				{
					safe_strncpy(distro_str, "Arch Linux", MAX_STRLEN);
					safe_strncpy(host_color, TLCY, MAX_STRLEN);
				}
				else if (FILE_EXISTS("/etc/gentoo-release"))
				{
					safe_strncpy(distro_str, "Gentoo", MAX_STRLEN);
					safe_strncpy(host_color, TLPR, MAX_STRLEN);
				}
				else if (FILE_EXISTS("/etc/angstrom-version"))
				{
					safe_strncpy(distro_str, "Angstrom", MAX_STRLEN);
					safe_strncpy(host_color, TNRM, MAX_STRLEN);
				}
				else if (FILE_EXISTS("/etc/manjaro-release"))
				{
					safe_strncpy(distro_str, "Manjaro", MAX_STRLEN);
					safe_strncpy(host_color, TLGN, MAX_STRLEN);
				}
				else if (FILE_EXISTS("/etc/slackware-release"))
				{
					safe_strncpy(distro_str, "Slackware", MAX_STRLEN);
					safe_strncpy(host_color, TLBL, MAX_STRLEN);
				}
				else if (FILE_EXISTS("/etc/lsb-release"))
				{
					distro_file = fopen("/etc/lsb-release", "r");
					fscanf(distro_file, "%s ", distro_name_str);
					fclose(distro_file);

					snprintf(distro_str, MAX_STRLEN, "%s", distro_name_str + 11);
					safe_strncpy(host_color, TLRD, MAX_STRLEN);
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
					safe_strncpy(distro_str, "Linux", MAX_STRLEN);
					safe_strncpy(host_color, TLGY, MAX_STRLEN);

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

/*	detect_host
	detects the computer's hostname and active user and formats them
*/
void detect_host(void)
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

	snprintf(host_str, MAX_STRLEN, "%s%s%s%s@%s%s%s%s",
		host_color, given_user, TNRM, TWHT, TNRM, host_color, given_host, TNRM);

	return;
}

/*	detect_kernel
	detects the computer's kernel
*/
void detect_kernel(void)
{
	struct utsname kern_info;

	if (!(uname(&kern_info)))
	{
		snprintf(kernel_str, MAX_STRLEN, "%s %s %s", kern_info.sysname,
				kern_info.release, kern_info.machine);
	}
	else if (error)
	{
		ERR_REPORT("Could not detect kernel information.");
		safe_strncpy(kernel_str, "Linux", MAX_STRLEN);
	}

	return;
}

/*	detect_uptime
	detects the computer's uptime
*/
void detect_uptime(void)
{
	unsigned int secs = 0;
	unsigned int mins = 0;
	unsigned int hrs = 0;
	unsigned int days = 0;
	struct sysinfo si_upt;

	if (!(sysinfo(&si_upt)))
	{
		split_uptime(si_upt.uptime, &secs, &mins, &hrs, &days);

		if (days > 0)
			snprintf(uptime_str, MAX_STRLEN, "%dd %dh %dm %ds", days, hrs, mins, secs);
		else
			snprintf(uptime_str, MAX_STRLEN, "%dh %dm %ds", hrs, mins, secs);
	}
	else
	{
		ERR_REPORT("Could not detect system uptime.");
	}

	return;
}

/*	detect_pkgs
	detects the number of packages installed on the computer
*/
void detect_pkgs(void)
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
		safe_strncpy(pkgs_str, "Not Found", MAX_STRLEN);

		if (error)
			ERR_REPORT("Packages cannot be detected on an unknown "
						"Linux distro.");
	}

	snprintf(pkgs_str, MAX_STRLEN, "%d", packages);

	return;
}

/*	detect_cpu
	detects the computer's CPU brand/name-string
*/
void detect_cpu(void)
{
	FILE *cpu_file;
	char cpuinfo_buf[MAX_STRLEN];
	char *cpuinfo_line;
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
		if (!(cpuinfo_line = strchr(cpuinfo_buf, ':')))
		{
			ERR_REPORT("Fatal error matching in /proc/cpuinfo");
			return;
		}

		cpuinfo_line += 2;
		end = strlen(cpuinfo_line);

		if (cpuinfo_line[end - 1] == '\n')
			cpuinfo_line[end - 1] = '\0';

		if (STREQ(cpuinfo_line, "ARMv6-compatible processor rev 7 (v6l)"))
		{
			safe_strncpy(cpu_str, "BCM2708 (Raspberry Pi)", MAX_STRLEN);
		}
		else
		{
			safe_strncpy(cpu_str, cpuinfo_line, MAX_STRLEN);
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
*/
void detect_gpu(void)
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
				safe_strncpy(gpu_str, (const char *) glGetString(GL_RENDERER),
						MAX_STRLEN);

				glXDestroyContext(disp, context);
			}
			else if (error)
			{
				ERR_REPORT("Failed to create OpenGL context.");
			}

			XFree(visual_info);
		}
		else if (error)
		{
			ERR_REPORT("Failed to select a proper X visual.");
		}

		XCloseDisplay(disp);
	}
	else if (error)
	{
		safe_strncpy(gpu_str, "No X Server", MAX_STRLEN);
		ERR_REPORT("Could not open an X display (detect_gpu).");
	}

	return;
}

/*	detect_disk
	detects the computer's total disk capacity and usage
*/
void detect_disk(void)
{
	FILE *mnt_file;
	struct mntent *ent;
	struct statvfs fs;
	unsigned long long disk_total = 0, disk_used = 0, disk_pct = 0;

	if ((mnt_file = setmntent("/etc/mtab", "r")))
	{
		while ((ent = getmntent(mnt_file)))
		{
			/* we only want to get the size of "real" disks (starting with /) */
			if (ent->mnt_dir && ent->mnt_fsname && ent->mnt_fsname[0] == '/')
			{
				if (!statvfs(ent->mnt_dir, &fs))
				{
					disk_total += (fs.f_blocks * fs.f_bsize);
					disk_used += ((fs.f_blocks - fs.f_bfree) * fs.f_bsize);
				}
				else
				{
					ERR_REPORT("Could not stat filesystem for statistics (detect_disk).");
				}
			}
		}

		disk_total /= GB;
		disk_used /= GB;
		disk_pct = (disk_total > 0 ? (((double) disk_used / disk_total) * 100) : 0);

		snprintf(disk_str, MAX_STRLEN, "%lluG / %lluG (%llu%%)", disk_used,
				disk_total, disk_pct);

		endmntent(mnt_file);
	}
	else if (error)
	{
		ERR_REPORT("Could not open /etc/mtab (detect_disk).");
	}

	return;
}

/*	detect_mem
	detects the computer's total and used RAM
*/
void detect_mem(void)
{
	long long total_mem = 0, free_mem = 0, used_mem = 0;
	struct sysinfo si_mem;

	/* known problem: because linux utilizes free ram in caches/buffers,
	   the amount of memory sysinfo reports as free is very small.
	*/
	sysinfo(&si_mem);

	total_mem = (long long) (si_mem.totalram * si_mem.mem_unit) / MB;
	free_mem = (long long) ((si_mem.freeram + si_mem.bufferram) * si_mem.mem_unit) / MB;
	used_mem = (long long) total_mem - free_mem;

	snprintf(mem_str, MAX_STRLEN, "%lld%s / %lld%s", used_mem, "MB", total_mem, "MB");

	return;
}

/*	detect_shell
	detects the shell currently running on the computer
	--
	CAVEAT: shell version detection relies on the standard versioning format for
	each shell. If any shell's older (or newer versions) suddenly begin to use a new
	scheme, the version may be displayed incorrectly.
	--
*/
void detect_shell(void)
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
		safe_strncpy(shell_str, "POSIX sh", MAX_STRLEN);
	}
	else if (strstr(shell_name, "bash"))
	{
		shell_file = popen("bash --version | head -1", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);
		snprintf(shell_str, MAX_STRLEN, "bash %.*s", 17, vers_str + 10);
		pclose(shell_file);
	}
	else if (strstr(shell_name, "zsh"))
	{
		shell_file = popen("zsh --version", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);
		snprintf(shell_str, MAX_STRLEN, "zsh %.*s", 5, vers_str + 4);
		pclose(shell_file);
	}
	else if (strstr(shell_name, "csh"))
	{
		shell_file = popen("csh --version | head -1", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);
		snprintf(shell_str, MAX_STRLEN, "csh %.*s", 7, vers_str + 5);
		pclose(shell_file);
	}
	else if (strstr(shell_name, "fish"))
	{
		shell_file = popen("fish --version", "r");
		fgets(vers_str, MAX_STRLEN, shell_file);
		snprintf(shell_str, MAX_STRLEN, "fish %.*s", 13, vers_str + 6);
		pclose(shell_file);
	}
	else if (strstr(shell_name, "dash") || strstr(shell_name, "ash")
			|| strstr(shell_name, "ksh"))
	{
		/* i don't have a version detection system for these, yet */
		safe_strncpy(shell_str, shell_name, MAX_STRLEN);
	}

	return;
}

/*	detect_res
	detects the combined resolution of all monitors attached to the computer
*/
void detect_res(void)
{
	int width = 0, height = 0;
	Display *disp;
	Screen *screen;

	if ((disp = XOpenDisplay(NULL)))
	{
		screen = XDefaultScreenOfDisplay(disp);
		width = WidthOfScreen(screen);
		height = HeightOfScreen(screen);

		snprintf(res_str, MAX_STRLEN, "%dx%d", width, height);

		XCloseDisplay(disp);
	}
	else
	{
		safe_strncpy(res_str, "No X Server", MAX_STRLEN);

		if (error)
			ERR_REPORT("Could not open an X display (detect_res)");
	}

	return;
}

/*	detect_de
	detects the desktop environment currently running on top of the OS
*/
void detect_de(void)
{
	char *curr_de;

	if ((curr_de = getenv("XDG_CURRENT_DESKTOP")))
	{
		safe_strncpy(de_str, curr_de, MAX_STRLEN);
	}
	else
	{
		if (getenv("GNOME_DESKTOP_SESSION_ID"))
		{
			safe_strncpy(de_str, "Gnome", MAX_STRLEN);
		}
		else if (getenv("MATE_DESKTOP_SESSION_ID"))
		{
			safe_strncpy(de_str, "MATE", MAX_STRLEN);
		}
		else if (getenv("KDE_FULL_SESSION"))
		{
			/*	KDE_SESSION_VERSION only exists on KDE4+, so
				getenv will return NULL on KDE <= 3.
			 */
			snprintf(de_str, MAX_STRLEN, "KDE%s", getenv("KDE_SESSION_VERSION"));
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
*/
void detect_wm(void)
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
			0, KB, false, XA_WINDOW, &actual_type, &actual_format, &nitems,
			&bytes, (unsigned char **) &wm_check_window)))
		{
			if (wm_check_window && !(XGetWindowProperty(disp, *wm_check_window,
				XInternAtom(disp, "_NET_WM_NAME", true), 0, KB, false,
				XInternAtom(disp, "UTF8_STRING", true),	&actual_type,
				&actual_format, &nitems, &bytes, (unsigned char **) &wm_name)))
			{
				safe_strncpy(wm_str, wm_name, MAX_STRLEN);
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
	--
	CAVEAT: This function relies on the presence of 'detectwmtheme', a shell script.
	If it isn't present somewhere in the PATH, the WM Theme will be set as 'Unknown'
	--
*/
void detect_wm_theme(void)
{
	char exec_str[MAX_STRLEN];
	FILE *wm_theme_file;

	snprintf(exec_str, MAX_STRLEN, "detectwmtheme %s 2> /dev/null", wm_str);

	wm_theme_file = popen(exec_str, "r");
	fgets(wm_theme_str, MAX_STRLEN, wm_theme_file);
	pclose(wm_theme_file);

	return;
}

/*	detect_gtk
	detects the theme, icon(s), and font(s) associated with a GTK DE (if present)
	--
	CAVEAT: This function relies on the presence of 'detectgtk', a shell script.
	If it isn't present somewhere in the PATH, the GTK will be set as 'Unknown'
	--
*/
void detect_gtk(void)
{
	FILE *gtk_file;
	char gtk2_str[MAX_STRLEN] = "Unknown";
	char gtk3_str[MAX_STRLEN] = "Unknown";
	char gtk_icons_str[MAX_STRLEN] = "Unknown";
	char gtk_font_str[MAX_STRLEN] = "Unknown";

	gtk_file = popen("detectgtk 2> /dev/null", "r");
	fscanf(gtk_file, "%s%s%s%s", gtk2_str, gtk3_str, gtk_icons_str, gtk_font_str);
	pclose(gtk_file);

	if (STREQ(gtk3_str, "Unknown"))
		snprintf(gtk_str, MAX_STRLEN, "%s (GTK2), %s (Icons)", gtk2_str,
				gtk_icons_str);
	else if (STREQ(gtk2_str, "Unknown"))
		snprintf(gtk_str, MAX_STRLEN, "%s (GTK3), %s (Icons)", gtk3_str,
				gtk_icons_str);
	else
		snprintf(gtk_str, MAX_STRLEN, "%s (GTK2), %s (GTK3)", gtk2_str, gtk3_str);

	snprintf(icon_str, MAX_STRLEN, "%s", gtk_icons_str);

	snprintf(font_str, MAX_STRLEN, "%s", gtk_font_str);

	return;
}
