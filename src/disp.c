/*	disp.c
 *	Author: William Woodruff
 *	-------------
 *
 *	Functions used by screenfetch-c for displaying version
 *	and help output to the user.
 *	Like the rest of screenfetch-c, this file is licensed under the MIT license.
 *	You should have received a copy of it with this code.
 */

/* standard includes */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/* program includes */
#include "version.h"
#include "logos.h"
#include "colors.h"
#include "misc.h"
#include "disp.h"

/*	display_version
	called if the -V (--version) flag is tripped
*/
void display_version(void)
{
	printf("%s\n", "screenfetch-c - Version " SCREENFETCH_C_VERSION_MAJOR "." SCREENFETCH_C_VERSION_MINOR ", revision " SCREENFETCH_C_VERSION_RELEASE ".");
	return;
}

/*	display_help
	called if the -h (--help) flag is tripped
*/
void display_help(void)
{
	display_version();
	printf("\n%s\n", "Options:");
	printf(" %s\n", "-m, --manual\t\t\t Enter manual mode.");
	printf(" %s\n", "-v, --verbose\t\t\t Enable verbosity during output.");
	printf(" %s\n", "-s, --screenshot\t\t Take a screenshot.");
	printf(" %s\n", "-n, --no-logo\t\t\t Print output without a logo.");
	printf(" %s\n", "-D, --distro [DISTRO]\t\t Print output with DISTRO's logo.");
	printf(" %s\n", "-E, --suppress-errors\t\t Suppress error output.");
	printf(" %s\n", "-V, --version\t\t\t Output screenfetch-c's version and exit.");
	printf(" %s\n", "-h, --help\t\t\t Output this help information.");
	printf(" %s\n", "-L, --logo-only [DISTRO]\t Output only the logo associated with DISTRO.");
	printf("\n%s\n", "For more information, consult screenfetch-c's man page.");
	return;
}

/*	display_verbose
	called if the -v (--verbose) flag is tripped
	arguments: char *data[], *data_names[] the system data and names being printed
*/
void display_verbose(char *data[], char *data_names[])
{
	int i = 0;
	for (i = 0; i < 16; i++)
		VERBOSE_OUT(data_names[i], data[i]);
}

/*	output_logo_only
	outputs an ASCII logo based upon the distro name passed to it
	argument char *distro the name of the distro to output
*/
void output_logo_only(char *distro)
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

/*	main_ascii_output
	the primary output for screenfetch-c - all info and ASCII art is printed here
	arguments char *data[], char *data_names[]: string arrays containing the names and data acquired
*/
void main_ascii_output(char *data[], char *data_names[])
{
	int i = 0;

	if (strstr(data[1], "Microsoft"))
	{
		for (i = 0; i < 16; i++)
		{
			printf("%s %s%s%s%s%s\n", windows_logo[i], TRED, data_names[i], TWHT, data[i], TNRM);
		}
	}
	else if (strstr(data[i], "OS X"))
	{
		for (i = 0; i < 16; i++)
		{
			printf("%s %s%s%s%s\n", macosx_logo[i], TLBL, data_names[i], TNRM, data[i]);
		}
	}
	else if (STRCMP(data[1], "Arch Linux - Old"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s%s\n", oldarch_logo[i], TLBL, data_names[i], TNRM, data[i], TNRM);
			else
				printf("%s\n", oldarch_logo[i]);
		}
	}
	else if (STRCMP(data[1], "Arch Linux"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s%s\n", arch_logo[i], TLCY, data_names[i], TNRM, data[i], TNRM);
			else
				printf("%s\n", arch_logo[i]);
		}
	}
	else if (STRCMP(data[1], "LinuxMint"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s%s\n", mint_logo[i], TLGN, data_names[i], TNRM, data[i], TNRM);
			else
				printf("%s\n", mint_logo[i]);
		}
	}
	else if (STRCMP(data[1], "LMDE"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s\n", lmde_logo[i], TLGN, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", lmde_logo[i]);
		}
	}
	else if (STRCMP(data[1], "Ubuntu") || STRCMP(data[1], "Lubuntu") || STRCMP(data[1], "Xubuntu"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s%s\n", ubuntu_logo[i], TLRD, data_names[i], TNRM, data[i], TNRM);
			else
				printf("%s\n", ubuntu_logo[i]);
		}
	}
	else if (STRCMP(data[1], "Debian"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s\n", debian_logo[i], TLRD, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", debian_logo[i]);
		}
	}
	else if (STRCMP(data[1], "CrunchBang"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s\n", crunchbang_logo[i], TDGY, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", crunchbang_logo[i]);
		}
	}
	else if (STRCMP(data[1], "Gentoo"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s\n", gentoo_logo[i], TLPR, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", gentoo_logo[i]);
		}
	}
	else if (STRCMP(data[1], "Funtoo"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s\n", funtoo_logo[i], TLPR, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", funtoo_logo[i]);
		}
	}
	else if (STRCMP(data[1], "Fedora"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s\n", fedora_logo[i], TLBL, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", fedora_logo[i]);
		}
	}
	else if (STRCMP(data[1], "Mandriva") || STRCMP(data[1], "Mandrake"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s\n", mandriva_mandrake_logo[i], TLBL, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", mandriva_mandrake_logo[i]);
		}
	}
	else if (STRCMP(data[1], "OpenSUSE"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s\n", opensuse_logo[i], TLGN, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", opensuse_logo[i]);
		}
	}
	else if (STRCMP(data[1], "Slackware"))
	{
		for (i = 0; i < 21; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s\n", slackware_logo[i], TLBL, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", slackware_logo[i]);
		}
	}
	else if (STRCMP(data[1], "Red Hat Linux"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s\n", redhat_logo[i], TRED, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", redhat_logo[i]);
		}
	}
	else if (STRCMP(data[1], "Frugalware"))
	{
		for (i = 0; i < 23; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s\n", frugalware_logo[i], TLCY, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", frugalware_logo[i]);
		}
	}
	else if (STRCMP(data[1], "Peppermint"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s\n", peppermint_logo[i], TLRD, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", peppermint_logo[i]);
		}
	}
	else if (STRCMP(data[1], "SolusOS"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s\n", solusos_logo[i], TLGY, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", solusos_logo[i]);
		}
	}
	else if (STRCMP(data[1], "Mageia"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s\n", mageia_logo[i], TLCY, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", mageia_logo[i]);
		}
	}
	else if (STRCMP(data[1], "ParabolaGNU/Linux-libre"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s\n", parabolagnu_linuxlibre_logo[i], TLPR, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", parabolagnu_linuxlibre_logo[i]);
		}
	}
	else if (STRCMP(data[1], "Viperr"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s\n", viperr_logo[i], TLGY, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", viperr_logo[i]);
		}
	}
	else if (STRCMP(data[1], "LinuxDeepin"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s\n", linuxdeepin_logo[i], TLGN, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", linuxdeepin_logo[i]);
		}
	}
	else if (STRCMP(data[1], "Chakra"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s\n", chakra_logo[i], TLBL, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", chakra_logo[i]);
		}
	}
	else if (STRCMP(data[1], "Fuduntu"))
	{
		for (i = 0; i < 21; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s\n", fuduntu_logo[i], TLRD, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", fuduntu_logo[i]);
		}
	}
	else if (STRCMP(data[1], "Trisquel"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s\n", trisquel_logo[i], TLBL, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", trisquel_logo[i]);
		}
	}
	else if (STRCMP(data[1], "Manjaro"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s\n", manjaro_logo[i], data_names[i], data[i]);
			else
				printf("%s\n", manjaro_logo[i]);
		}
	}
	else if (STRCMP(data[1], "elementary OS"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s\n", elementaryos_logo[i], TLGN, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", elementaryos_logo[i]);
		}
	}
	else if (STRCMP(data[1], "Scientific Linux"))
	{
		for (i = 0; i < 20; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s\n", scientificlinux_logo[i], TLRD, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", scientificlinux_logo[i]);
		}
	}
	else if (STRCMP(data[1], "Backtrack Linux"))
	{
		for (i = 0; i < 20; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s\n", backtracklinux_logo[i], TLRD, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", backtracklinux_logo[i]);
		}
	}
	else if (STRCMP(data[1], "Sabayon"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s\n", sabayon_logo[i], TLBL, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", sabayon_logo[i]);
		}
	}
	else if (STRCMP(data[1], "Android"))
	{
		for (i = 0; i < 16; i++)
		{
			if (i < 12)
				printf("%s %s%s%s%s\n", android_logo[i], TLGN, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", android_logo[i]);
		}
	}
	else if (STRCMP(data[1], "Angstrom"))
	{
		for (i = 0; i < 16; i++)
		{
			printf("%s %s%s\n", angstrom_logo[i], data_names[i], data[i]);
		}
	}
	else if (STRCMP(data[1], "Linux"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s\n", linux_logo[i], TLGY, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", linux_logo[i]);
		}
	}
	else if (STRCMP(data[1], "FreeBSD"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s%s%s\n", freebsd_logo[i], TLRD, data_names[i], TNRM, data[i]);
			else
				printf("%s\n", freebsd_logo[i]);
		}
	}
	else if (STRCMP(data[1], "OpenBSD"))
	{
		for (i = 0; i < 23; i++)
		{
			if (i < 16)
				printf("%s %s%s\n", openbsd_logo[i], data_names[i], data[i]);
			else
				printf("%s\n", openbsd_logo[i]);
		}
	}
	else if (STRCMP(data[1], "NetBSD"))
	{
		for (i = 0; i < 23; i++)
		{
			if (i < 16)
				printf("%s %s%s\n", netbsd_logo[i], data_names[i], data[i]);
			else
				printf("%s\n", netbsd_logo[i]);
		}
	}
	else if (STRCMP(data[1], "DragonFly BSD"))
	{
		for (i = 0; i < 23; i++)
		{
			if (i < 16)
				printf("%s %s%s\n", dragonflybsd_logo[i], data_names[i], data[i]);
			else
				printf("%s\n", dragonflybsd_logo[i]);
		}
	}
	else if (STRCMP(data[1], "SunOS"))
	{
		for (i = 0; i < 17; i++)
		{
			if (i < 16)
				printf("%s %s%s\n", solaris_logo[i], data_names[i], data[i]);
			else
				printf("%s\n", solaris_logo[i]);
		}
	}
	else
	{
		ERROR_OUT("Error: ", "Could not find a logo corresponding to the distro.");
	}

	return;
}

/*	main_text_output
	the secondary output for screenfetch-c - all info WITHOUT ASCII art is printed here
	arguments char *data[], char *data_names[]: string arrays containing the names and data acquired
*/
void main_text_output(char *data[], char *data_names[])
{
	int i;

	for (i = 0; i < 16; i++)
		printf("%s %s\n", data_names[i], data[i]);

	return;
}
