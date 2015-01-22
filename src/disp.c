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

/*	process_logo_only
	output handling for output_logo_only
	argument char *distro: the output logo
	argument unsigned short int num: the length of the logo in lines
*/
void process_logo_only(char *distro[], unsigned short int num)
{
	unsigned short int x = 0;

	for (x = 0; x < num; x++)
		printf("%s\n", distro[x]);

	return;
}

/*	output_logo_only
	outputs an ASCII logo based upon the distro name passed to it
	argument char *distro: the name of the distro to output
*/
void output_logo_only(char *distro)
{
	if (STREQ(distro, "Windows"))
	{
		process_logo_only(windows_logo, 16);
	}
	else if (strstr(distro, "OS X"))
	{
		process_logo_only(macosx_logo, 16);
	}
	else if (STREQ(distro, "Arch Linux - Old"))
	{
		process_logo_only(oldarch_logo, 18);
	}
	else if (STREQ(distro, "Arch Linux"))
	{
		process_logo_only(arch_logo, 19);
	}
	else if (STREQ(distro, "LinuxMint"))
	{
		process_logo_only(mint_logo, 18);
	}
	else if (STREQ(distro, "LMDE"))
	{
		process_logo_only(lmde_logo, 18);
	}
	else if (STREQ(distro, "Ubuntu") || STREQ(distro, "Lubuntu")
			|| STREQ(distro, "Xubuntu"))
	{
		process_logo_only(ubuntu_logo, 18);
	}
	else if (STREQ(distro, "Debian"))
	{
		process_logo_only(debian_logo, 18);
	}
	else if (STREQ(distro, "CrunchBang"))
	{
		process_logo_only(crunchbang_logo, 18);
	}
	else if (STREQ(distro, "Gentoo"))
	{
		process_logo_only(gentoo_logo, 18);
	}
	else if (STREQ(distro, "Funtoo"))
	{
		process_logo_only(funtoo_logo, 18);
	}
	else if (STREQ(distro, "Fedora"))
	{
		process_logo_only(fedora_logo, 18);
	}
	else if (STREQ(distro, "Mandriva") || STREQ(distro, "Mandrake"))
	{
		process_logo_only(mandriva_mandrake_logo, 18);
	}
	else if (STREQ(distro, "OpenSUSE"))
	{
		process_logo_only(opensuse_logo, 18);
	}
	else if (STREQ(distro, "Slackware"))
	{
		process_logo_only(slackware_logo, 21);
	}
	else if (STREQ(distro, "Red Hat Linux"))
	{
		process_logo_only(redhat_logo, 18);
	}
	else if (STREQ(distro, "Frugalware"))
	{
		process_logo_only(frugalware_logo, 23);
	}
	else if (STREQ(distro, "Peppermint"))
	{
		process_logo_only(peppermint_logo, 19);
	}
	else if (STREQ(distro, "SolusOS"))
	{
		process_logo_only(solusos_logo, 18);
	}
	else if (STREQ(distro, "Mageia"))
	{
		process_logo_only(mageia_logo, 18);
	}
	else if (STREQ(distro, "ParabolaGNU/Linux-libre"))
	{
		process_logo_only(parabolagnu_linuxlibre_logo, 18);
	}
	else if (STREQ(distro, "Viperr"))
	{
		process_logo_only(viperr_logo, 18);
	}
	else if (STREQ(distro, "LinuxDeepin"))
	{
		process_logo_only(linuxdeepin_logo, 18);
	}
	else if (STREQ(distro, "Chakra"))
	{
		process_logo_only(chakra_logo, 18);
	}
	else if (STREQ(distro, "Fuduntu"))
	{
		process_logo_only(fuduntu_logo, 21);
	}
	else if (STREQ(distro, "Trisquel"))
	{
		process_logo_only(trisquel_logo, 18);
	}
	else if (STREQ(distro, "Manjaro"))
	{
		process_logo_only(manjaro_logo, 18);
	}
	else if (STREQ(distro, "elementary OS"))
	{
		process_logo_only(elementaryos_logo, 18);
	}
	else if (STREQ(distro, "Scientific Linux"))
	{
		process_logo_only(scientificlinux_logo, 20);
	}
	else if (STREQ(distro, "Backtrack Linux"))
	{
		process_logo_only(backtracklinux_logo, 21);
	}
	else if (STREQ(distro, "Kali Linux"))
	{
		process_logo_only(kalilinux_logo, 21);
	}
	else if (STREQ(distro, "Sabayon"))
	{
		process_logo_only(sabayon_logo, 18);
	}
	else if (STREQ(distro, "FreeBSD"))
	{
		process_logo_only(freebsd_logo, 18);
	}
	else if (STREQ(distro, "OpenBSD"))
	{
		process_logo_only(openbsd_logo, 23);
	}
	else if (STREQ(distro, "NetBSD"))
	{
		process_logo_only(netbsd_logo, 20);
	}
	else if (STREQ(distro, "DragonFly BSD"))
	{
		process_logo_only(dragonflybsd_logo, 18);
	}
	else if (STREQ(distro, "Android"))
	{
		process_logo_only(android_logo, 16);
	}
	else if (STREQ(distro, "Solaris"))
	{
		process_logo_only(solaris_logo, 17);
	}
	else if (STREQ(distro, "Angstrom"))
	{
		process_logo_only(angstrom_logo, 16);
	}
	else /* if (STREQ(distro_str, "Linux")) */
	{
		process_logo_only(linux_logo, 16);
	}

	return;
}

/* process_data
   output handling main_ascii_output
   argument char *data[]: the data being output
   argument char *data_names[]: the name associated with each datum
   argument char *logo[]: the logo associated with the distro
   argument unsigned short int num1, num2: indices for the data
   argument char *col1..3: colors for the output
*/
void process_data(char *data[], char *data_names[], char *logo[], unsigned short int num1, unsigned short int num2, char *col1, char *col2, char *col3)
{
	unsigned short int x = 0;

	if (0 == num2)
	{
		for (x = 0; x < num1; x++)
		{
			printf("%s %s%s%s%s%s%s\n", logo[x], col1, col2, col3,
						data_names[x], TNRM, data[x]);
		}
	}
	else
	{
		for (x = 0; x < num1; x++)
		{
			if (x < num2)
			{
				printf("%s %s%s%s%s%s%s\n", logo[x], col1, col2, col3,
						data_names[x], TNRM, data[x]);
			}
			else
			{
				printf("%s\n", logo[x]);
			}
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
	if (strstr(data[1], "Microsoft"))
	{
		process_data(data, data_names, windows_logo, 16, 0, TRED, TWHT, TRED);
	}
	else if (strstr(data[1], "OS X"))
	{
		process_data(data, data_names, macosx_logo, 16, 0, TLBL, TNRM, TLBL);
	}
	else if (STREQ(data[1], "Arch Linux - Old"))
	{
		process_data(data, data_names, oldarch_logo, 18, 16, TLBL, TNRM, TLBL);
	}
	else if (STREQ(data[1], "Arch Linux"))
	{
		process_data(data, data_names, arch_logo, 19, 17, TLCY, TNRM, TLCY);
	}
	else if (STREQ(data[1], "LinuxMint"))
	{
		process_data(data, data_names, mint_logo, 18, 16, TLGN, TNRM, TLGN);
	}
	else if (STREQ(data[1], "LMDE"))
	{
		process_data(data, data_names, lmde_logo, 18, 16, TLGN, TNRM, TLGN);
	}
	else if (STREQ(data[1], "Ubuntu") || STREQ(data[1], "Lubuntu")
			|| STREQ(data[1], "Xubuntu"))
	{
		process_data(data, data_names, ubuntu_logo, 18, 16, TLRD, TNRM, TLRD);
	}
	else if (STREQ(data[1], "Debian"))
	{
		process_data(data, data_names, debian_logo, 18, 16, TLRD, TNRM, TLRD);
	}
	else if (STREQ(data[1], "CrunchBang"))
	{
		process_data(data, data_names, crunchbang_logo, 18, 16, TDGY, TNRM, TDGY);
	}
	else if (STREQ(data[1], "Gentoo"))
	{
		process_data(data, data_names, gentoo_logo, 18, 16, TLPR, TNRM, TLPR);
	}
	else if (STREQ(data[1], "Funtoo"))
	{
		process_data(data, data_names, funtoo_logo, 18, 16, TLPR, TNRM, TLPR);
	}
	else if (STREQ(data[1], "Fedora"))
	{
		process_data(data, data_names, fedora_logo, 18, 16, TLBL, TNRM, TLBL);
	}
	else if (STREQ(data[1], "Mandriva") || STREQ(data[1], "Mandrake"))
	{
		process_data(data, data_names, mandriva_mandrake_logo, 18, 16, TLBL, TNRM, TLBL);
	}
	else if (STREQ(data[1], "OpenSUSE"))
	{
		process_data(data, data_names, opensuse_logo, 18, 16, TLGN, TNRM, TLGN);
	}
	else if (STREQ(data[1], "Slackware"))
	{
		process_data(data, data_names, slackware_logo, 21, 16, TLBL, TNRM, TLBL);
	}
	else if (STREQ(data[1], "Red Hat Linux"))
	{
		process_data(data, data_names, redhat_logo, 18, 16, TRED, TNRM, TRED);
	}
	else if (STREQ(data[1], "Frugalware"))
	{
		process_data(data, data_names, frugalware_logo, 23, 16, TLCY, TNRM, TLCY);
	}
	else if (STREQ(data[1], "Peppermint"))
	{
		process_data(data, data_names, peppermint_logo, 18, 16, TLRD, TNRM, TLRD);
	}
	else if (STREQ(data[1], "SolusOS"))
	{
		process_data(data, data_names, solusos_logo, 18, 16, TLGY, TNRM, TLGY);
	}
	else if (STREQ(data[1], "Mageia"))
	{
		process_data(data, data_names, mageia_logo, 18, 16, TLGY, TNRM, TLGY);
	}
	else if (STREQ(data[1], "ParabolaGNU/Linux-libre"))
	{
		process_data(data, data_names, parabolagnu_linuxlibre_logo, 18, 16, TLGY, TLPR, TLGY);
	}
	else if (STREQ(data[1], "Viperr"))
	{
		process_data(data, data_names, viperr_logo, 18, 16, TLGY, TNRM, TLGY);
	}
	else if (STREQ(data[1], "LinuxDeepin"))
	{
		process_data(data, data_names, linuxdeepin_logo, 18, 16, TLGN, TNRM, TLGN);
	}
	else if (STREQ(data[1], "Chakra"))
	{
		process_data(data, data_names, chakra_logo, 18, 16, TLBL, TNRM, TLBL);
	}
	else if (STREQ(data[1], "Fuduntu"))
	{
		process_data(data, data_names, fuduntu_logo, 21, 16, TLRD, TNRM, TLRD);
	}
	else if (STREQ(data[1], "Trisquel"))
	{
		process_data(data, data_names, trisquel_logo, 18, 16, TLBL, TNRM, TLBL);
	}
	else if (STREQ(data[1], "Manjaro"))
	{
		process_data(data, data_names, manjaro_logo, 18, 16, "", "", "");
	}
	else if (STREQ(data[1], "elementary OS"))
	{
		process_data(data, data_names, elementaryos_logo, 18, 16, TLGN, TNRM, TLGN);
	}
	else if (STREQ(data[1], "Scientific Linux"))
	{
		process_data(data, data_names, scientificlinux_logo, 20, 16, TLRD, TNRM, TLRD);
	}
	else if (STREQ(data[1], "Backtrack Linux"))
	{
		process_data(data, data_names, backtracklinux_logo, 21, 16, TLRD, TNRM, TLRD);
	}
	else if (STREQ(data[1], "Kali Linux"))
	{
		process_data(data, data_names, backtracklinux_logo, 21, 16, TLBL, TNRM, TLBL);
	}
	else if (STREQ(data[1], "Sabayon"))
	{
		process_data(data, data_names, sabayon_logo, 18, 16, TLBL, TNRM, TLBL);
	}
	else if (STREQ(data[1], "Android"))
	{
		process_data(data, data_names, android_logo, 16, 12, TLGN, TNRM, TLGN);
	}
	else if (STREQ(data[1], "Angstrom"))
	{
		process_data(data, data_names, angstrom_logo, 16, 0, "", "", "");
	}
	else if (STREQ(data[1], "Linux"))
	{
		process_data(data, data_names, linux_logo, 18, 16, TLGY, TNRM, TLGY);
	}
	else if (STREQ(data[1], "FreeBSD"))
	{
		process_data(data, data_names, freebsd_logo, 18, 16, TLRD, TNRM, TLRD);
	}
	else if (STREQ(data[1], "OpenBSD"))
	{
		process_data(data, data_names, openbsd_logo, 23, 16, "", "", "");
	}
	else if (STREQ(data[1], "NetBSD"))
	{
		process_data(data, data_names, netbsd_logo, 23, 16, "", "", "");
	}
	else if (STREQ(data[1], "DragonFly BSD"))
	{
		process_data(data, data_names, dragonflybsd_logo, 23, 16, "", "", "");
	}
	else if (STREQ(data[1], "SunOS"))
	{
		process_data(data, data_names, solaris_logo, 17, 16, "", "", "");
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
