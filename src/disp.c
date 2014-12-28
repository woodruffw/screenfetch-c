/*	disp.c
 *	Author: William Woodruff
 *	-------------
 *
 *	Functions used by screenfetch-c for displaying version
 *	and help output to the user.
 *	Like the rest of screenfetch-c, this file is licensed under the MIT license.
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
	printf("%s\n", "screenfetch-c - Version " SCREENFETCH_C_VERSION_MAJOR "."
			SCREENFETCH_C_VERSION_MINOR ", revision "
			SCREENFETCH_C_VERSION_RELEASE ".");
	return;
}

/*	display_help
	called if the -h (--help) flag is tripped
*/
void display_help(void)
{
	display_version();
	printf("\n%s\n", "Options:\n"
			"  -m, --manual\t\t\t Enter manual mode.\n"
			"  -v, --verbose\t\t\t Enable verbosity during output.\n"
			"  -s, --screenshot\t\t Take a screenshot.\n"
			"  -n, --no-logo\t\t\t Print output without a logo.\n"
			"  -D, --distro [DISTRO]\t\t Print output with DISTRO's logo.\n"
			"  -E, --suppress-errors\t\t Suppress error output.\n"
			"  -p, --portrait\t\t Print output in portrait mode.\n"
			"  -V, --version\t\t\t Output screenfetch-c's version and exit.\n"
			"  -h, --help\t\t\t Output this help information.\n"
			"  -L, --logo-only [DISTRO]\t Output only DISTRO's logo.\n"
			"For more information, consult screenfetch-c's man page.\n");

	return;
}

/*	display_verbose
	called if the -v (--verbose) flag is tripped
	arguments: char *data[], *data_names[] the system data and names
*/
void display_verbose(char *data[], char *data_names[])
{
	int i = 0;
	for (i = 0; i < 16; i++)
		VERBOSE_OUT(data_names[i], data[i]);

	return;
}

/*	output_logo_only
	outputs an ASCII logo based upon the distro name passed to it
	argument char *distro the name of the distro to output
*/
void output_logo_only(char *distro)
{
	int i = 0;

	if (STREQ(distro, "Windows"))
	{
		for (i = 0; i < 16; i++)
		{
			printf("%s\n", windows_logo[i]);
		}
	}
	else if (strstr(distro, "OS X"))
	{
		for (i = 0; i < 16; i++)
		{
			printf("%s\n", macosx_logo[i]);
		}
	}
	else if (STREQ(distro, "Arch Linux - Old"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", oldarch_logo[i]);
		}
	}
	else if (STREQ(distro, "Arch Linux"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", arch_logo[i]);
		}
	}
	else if (STREQ(distro, "LinuxMint"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", mint_logo[i]);
		}
	}
	else if (STREQ(distro, "LMDE"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", lmde_logo[i]);
		}
	}
	else if (STREQ(distro, "Ubuntu") || STREQ(distro, "Lubuntu")
			|| STREQ(distro, "Xubuntu"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", ubuntu_logo[i]);
		}
	}
	else if (STREQ(distro, "Debian"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", debian_logo[i]);
		}
	}
	else if (STREQ(distro, "CrunchBang"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", crunchbang_logo[i]);
		}
	}
	else if (STREQ(distro, "Gentoo"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", gentoo_logo[i]);
		}
	}
	else if (STREQ(distro, "Funtoo"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", funtoo_logo[i]);
		}
	}
	else if (STREQ(distro, "Fedora"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", fedora_logo[i]);
		}
	}
	else if (STREQ(distro, "Mandriva") || STREQ(distro, "Mandrake"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", mandriva_mandrake_logo[i]);
		}
	}
	else if (STREQ(distro, "OpenSUSE"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", opensuse_logo[i]);
		}
	}
	else if (STREQ(distro, "Slackware"))
	{
		for (i = 0; i < 21; i++)
		{
			printf("%s\n", slackware_logo[i]);
		}
	}
	else if (STREQ(distro, "Red Hat Linux"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", redhat_logo[i]);
		}
	}
	else if (STREQ(distro, "Frugalware"))
	{
		for (i = 0; i < 23; i++)
		{
			printf("%s\n", frugalware_logo[i]);
		}
	}
	else if (STREQ(distro, "Peppermint"))
	{
		for (i = 0; i < 19; i++)
		{
			printf("%s\n", peppermint_logo[i]);
		}
	}
	else if (STREQ(distro, "SolusOS"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", solusos_logo[i]);
		}
	}
	else if (STREQ(distro, "Mageia"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", mageia_logo[i]);
		}
	}
	else if (STREQ(distro, "ParabolaGNU/Linux-libre"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", parabolagnu_linuxlibre_logo[i]);
		}
	}
	else if (STREQ(distro, "Viperr"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", viperr_logo[i]);
		}
	}
	else if (STREQ(distro, "LinuxDeepin"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", linuxdeepin_logo[i]);
		}
	}
	else if (STREQ(distro, "Chakra"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", chakra_logo[i]);
		}
	}
	else if (STREQ(distro, "Fuduntu"))
	{
		for (i = 0; i < 21; i++)
		{
			printf("%s\n", fuduntu_logo[i]);
		}
	}
	else if (STREQ(distro, "Trisquel"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", trisquel_logo[i]);
		}
	}
	else if (STREQ(distro, "Manjaro"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", manjaro_logo[i]);
		}
	}
	else if (STREQ(distro, "elementary OS"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", elementaryos_logo[i]);
		}
	}
	else if (STREQ(distro, "Scientific Linux"))
	{
		for (i = 0; i < 20; i++)
		{
			printf("%s\n", scientificlinux_logo[i]);
		}
	}
	else if (STREQ(distro, "Backtrack Linux"))
	{
		for (i = 0; i < 21; i++)
		{
			printf("%s\n", backtracklinux_logo[i]);
		}
	}
	else if (STREQ(distro, "Kali Linux"))
	{
		for (i = 0; i < 21; i++)
		{
			printf("%s\n", kalilinux_logo[i]);
		}
	}
	else if (STREQ(distro, "Sabayon"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", sabayon_logo[i]);
		}
	}
	else if (STREQ(distro, "FreeBSD"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", freebsd_logo[i]);
		}
	}
	else if (STREQ(distro, "OpenBSD"))
	{
		for (i = 0; i < 23; i++)
		{
			printf("%s\n", openbsd_logo[i]);
		}
	}
	else if (STREQ(distro, "NetBSD"))
	{
		for (i = 0; i < 20; i++)
		{
			printf("%s\n", netbsd_logo[i]);
		}
	}
	else if (STREQ(distro, "DragonFly BSD"))
	{
		for (i = 0; i < 18; i++)
		{
			printf("%s\n", dragonflybsd_logo[i]);
		}
	}
	else if (STREQ(distro, "Android"))
	{
		for (i = 0; i < 16; i++)
		{
			printf("%s\n", android_logo[i]);
		}
	}
	else if (STREQ(distro, "Solaris"))
	{
		for (i = 0; i < 17; i++)
		{
			printf("%s\n", solaris_logo[i]);
		}
	}
	else if (STREQ(distro, "Angstrom"))
	{
		for (i = 0; i < 16; i++)
		{
			printf("%s\n", angstrom_logo[i]);
		}
	}
	else /* if (STREQ(distro_str, "Linux")) */
	{
		for (i = 0; i < 16; i++)
		{
			printf("%s\n", linux_logo[i]);
		}
	}

	return;
}

/*	main_ascii_output
	the primary output for screenfetch-c
	arguments char *data[], char *data_names[]:
*/
void main_ascii_output(char *data[], char *data_names[])
{
	int i = 0;

	if (strstr(data[1], "Microsoft"))
	{
		for (i = 0; i < 16; i++)
		{
			printf("%s " TRED "%s" TWHT "%s" TNRM "\n", windows_logo[i],
					data_names[i], data[i]);
		}
	}
	else if (strstr(data[1], "OS X"))
	{
		for (i = 0; i < 16; i++)
		{
			printf("%s " TLBL "%s" TNRM "%s\n", macosx_logo[i], data_names[i],
					data[i]);
		}
	}
	else if (STREQ(data[1], "Arch Linux - Old"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s " TLBL "%s" TNRM "%s" TNRM "\n", oldarch_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", oldarch_logo[i]);
		}
	}
	else if (STREQ(data[1], "Arch Linux"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s " TLCY "%s" TNRM "%s" TNRM "\n", arch_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", arch_logo[i]);
		}
	}
	else if (STREQ(data[1], "LinuxMint"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s " TLGN "%s" TNRM "%s" TNRM "\n", mint_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", mint_logo[i]);
		}
	}
	else if (STREQ(data[1], "LMDE"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s " TLGN "%s" TNRM "%s\n", lmde_logo[i], data_names[i],
						data[i]);
			else
				printf("%s\n", lmde_logo[i]);
		}
	}
	else if (STREQ(data[1], "Ubuntu") || STREQ(data[1], "Lubuntu")
			|| STREQ(data[1], "Xubuntu"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s " TLRD "%s" TNRM "%s" TNRM "\n", ubuntu_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", ubuntu_logo[i]);
		}
	}
	else if (STREQ(data[1], "Debian"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s " TLRD "%s" TNRM "%s\n", debian_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", debian_logo[i]);
		}
	}
	else if (STREQ(data[1], "CrunchBang"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s " TDGY "%s" TNRM "%s\n", crunchbang_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", crunchbang_logo[i]);
		}
	}
	else if (STREQ(data[1], "Gentoo"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s " TLPR "%s" TNRM "%s\n", gentoo_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", gentoo_logo[i]);
		}
	}
	else if (STREQ(data[1], "Funtoo"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s " TLPR "%s" TNRM "%s\n", funtoo_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", funtoo_logo[i]);
		}
	}
	else if (STREQ(data[1], "Fedora"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s " TLBL "%s" TNRM "%s\n", fedora_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", fedora_logo[i]);
		}
	}
	else if (STREQ(data[1], "Mandriva") || STREQ(data[1], "Mandrake"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s " TLBL "%s" TNRM "%s\n", mandriva_mandrake_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", mandriva_mandrake_logo[i]);
		}
	}
	else if (STREQ(data[1], "OpenSUSE"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s " TLGN "%s" TNRM "%s\n", opensuse_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", opensuse_logo[i]);
		}
	}
	else if (STREQ(data[1], "Slackware"))
	{
		for (i = 0; i < 21; i++)
		{
			if (i < 16)
				printf("%s " TLBL "%s" TNRM "%s\n", slackware_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", slackware_logo[i]);
		}
	}
	else if (STREQ(data[1], "Red Hat Linux"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s " TRED "%s" TNRM "%s\n", redhat_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", redhat_logo[i]);
		}
	}
	else if (STREQ(data[1], "Frugalware"))
	{
		for (i = 0; i < 23; i++)
		{
			if (i < 16)
				printf("%s " TLCY "%s" TNRM "%s\n", frugalware_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", frugalware_logo[i]);
		}
	}
	else if (STREQ(data[1], "Peppermint"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s " TLRD "%s" TNRM "%s\n", peppermint_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", peppermint_logo[i]);
		}
	}
	else if (STREQ(data[1], "SolusOS"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s " TLGY "%s" TNRM "%s\n", solusos_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", solusos_logo[i]);
		}
	}
	else if (STREQ(data[1], "Mageia"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s " TLGY "%s" TNRM "%s\n", mageia_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", mageia_logo[i]);
		}
	}
	else if (STREQ(data[1], "ParabolaGNU/Linux-libre"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s " TLGY "%s" TLPR "%s\n",
					parabolagnu_linuxlibre_logo[i],	data_names[i], data[i]);
			else
				printf("%s\n", parabolagnu_linuxlibre_logo[i]);
		}
	}
	else if (STREQ(data[1], "Viperr"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s " TLGY "%s" TNRM "%s\n", viperr_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", viperr_logo[i]);
		}
	}
	else if (STREQ(data[1], "LinuxDeepin"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s " TLGN "%s" TNRM "%s\n", linuxdeepin_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", linuxdeepin_logo[i]);
		}
	}
	else if (STREQ(data[1], "Chakra"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s " TLBL "%s" TNRM "%s\n", chakra_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", chakra_logo[i]);
		}
	}
	else if (STREQ(data[1], "Fuduntu"))
	{
		for (i = 0; i < 21; i++)
		{
			if (i < 16)
				printf("%s " TLRD "%s" TNRM "%s\n", fuduntu_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", fuduntu_logo[i]);
		}
	}
	else if (STREQ(data[1], "Trisquel"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s " TLBL "%s" TNRM "%s\n", trisquel_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", trisquel_logo[i]);
		}
	}
	else if (STREQ(data[1], "Manjaro"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s %s%s\n", manjaro_logo[i], data_names[i], data[i]);
			else
				printf("%s\n", manjaro_logo[i]);
		}
	}
	else if (STREQ(data[1], "elementary OS"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s " TLGN "%s" TNRM "%s\n", elementaryos_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", elementaryos_logo[i]);
		}
	}
	else if (STREQ(data[1], "Scientific Linux"))
	{
		for (i = 0; i < 20; i++)
		{
			if (i < 16)
				printf("%s " TLRD "%s" TNRM "%s\n", scientificlinux_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", scientificlinux_logo[i]);
		}
	}
	else if (STREQ(data[1], "Backtrack Linux"))
	{
		for (i = 0; i < 21; i++)
		{
			if (i < 16)
				printf("%s " TLRD "%s" TNRM "%s\n", backtracklinux_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", backtracklinux_logo[i]);
		}
	}
	else if (STREQ(data[1], "Kali Linux"))
	{
		for (i = 0; i < 21; i++)
		{
			if (i < 16)
				printf("%s " TLBL "%s" TNRM "%s\n", kalilinux_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", backtracklinux_logo[i]);
		}
	}
	else if (STREQ(data[1], "Sabayon"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s " TLBL "%s" TNRM "%s\n", sabayon_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", sabayon_logo[i]);
		}
	}
	else if (STREQ(data[1], "Android"))
	{
		for (i = 0; i < 16; i++)
		{
			if (i < 12)
				printf("%s " TLGN "%s" TNRM "%s\n", android_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", android_logo[i]);
		}
	}
	else if (STREQ(data[1], "Angstrom"))
	{
		for (i = 0; i < 16; i++)
		{
			printf("%s %s%s\n", angstrom_logo[i], data_names[i], data[i]);
		}
	}
	else if (STREQ(data[1], "Linux"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s " TLGY "%s" TNRM "%s\n", linux_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", linux_logo[i]);
		}
	}
	else if (STREQ(data[1], "FreeBSD"))
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 16)
				printf("%s " TLRD "%s" TNRM "%s\n", freebsd_logo[i],
						data_names[i], data[i]);
			else
				printf("%s\n", freebsd_logo[i]);
		}
	}
	else if (STREQ(data[1], "OpenBSD"))
	{
		for (i = 0; i < 23; i++)
		{
			if (i < 16)
				printf("%s %s%s\n", openbsd_logo[i], data_names[i], data[i]);
			else
				printf("%s\n", openbsd_logo[i]);
		}
	}
	else if (STREQ(data[1], "NetBSD"))
	{
		for (i = 0; i < 23; i++)
		{
			if (i < 16)
				printf("%s %s%s\n", netbsd_logo[i], data_names[i], data[i]);
			else
				printf("%s\n", netbsd_logo[i]);
		}
	}
	else if (STREQ(data[1], "DragonFly BSD"))
	{
		for (i = 0; i < 23; i++)
		{
			if (i < 16)
				printf("%s %s%s\n", dragonflybsd_logo[i], data_names[i],
						data[i]);
			else
				printf("%s\n", dragonflybsd_logo[i]);
		}
	}
	else if (STREQ(data[1], "SunOS"))
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
		ERR_REPORT("Could not find a logo for the distro.");
	}

	return;
}

/*	main_text_output
	the secondary output for screenfetch-c - all info WITHOUT ASCII art
	arguments char *data[], char *data_names[]
*/
void main_text_output(char *data[], char *data_names[])
{
	int i;

	for (i = 0; i < 16; i++)
		printf("%s %s\n", data_names[i], data[i]);

	return;
}
