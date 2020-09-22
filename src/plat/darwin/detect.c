/*	detect.c
**	Author: William Woodruff
**	-------------
**
**	The detection functions used by screenfetch-c on OS X (Darwin) are implemented here.
**	Like the rest of screenfetch-c, this file is licensed under the MIT license.
*/

/* standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

/* OS X-specific includes */
#include <sys/types.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/utsname.h>
#include <sys/sysctl.h>
#include <time.h>
#include <glob.h>
#include <Availability.h>
#include <IOKit/IOKitlib.h>
#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>
#include <mach/mach_time.h>
#include <mach/vm_statistics.h>
#include <mach/mach_types.h>
#include <mach/mach_init.h>
#include <mach/mach_host.h>

/* program includes */
#include "../../arrays.h"
#include "../../colors.h"
#include "../../misc.h"
#include "../../disp.h"
#include "../../util.h"
#include "../../error_flag.h"

/*	detect_distro
  detects the computer's distribution (OS X release)
*/
void detect_distro(void) {
  char *codenames[] = {"Cheetah",       "Puma",        "Jaguar",       "Panther",
                       "Tiger",         "Leopard",     "Snow Leopard", "Lion",
                       "Mountain Lion", "Mavericks",   "Yosemite",     "El Capitan",
                       "Sierra",        "High Sierra", "Mojave",       "Catalina"};
  CFArrayRef split = CFStringCreateArrayBySeparatingStrings(
      NULL,
      CFPreferencesCopyAppValue(CFSTR("ProductVersion"),
                                CFSTR("/System/Library/CoreServices/SystemVersion")),
      CFSTR("."));
  unsigned maj = CFStringGetIntValue(CFArrayGetValueAtIndex(split, 0));
  unsigned min = CFStringGetIntValue(CFArrayGetValueAtIndex(split, 1));
  unsigned fix = 0;
  if (CFArrayGetCount(split) == 3) {
    fix = CFStringGetIntValue(CFArrayGetValueAtIndex(split, 2));
  }

  char build_ver[16];
  CFStringGetCString(CFPreferencesCopyAppValue(CFSTR("ProductBuildVersion"),
                                               CFSTR("/System/Library/CoreServices/SystemVersion")),
                     build_ver, 16, kCFStringEncodingUTF8);

  char *codename = "Mac OS";
  char buf[128];
  if (min < sizeof(codenames) / sizeof(*codenames)) {
    snprintf(buf, sizeof(buf), "%s %s", min < 8 ? "Mac OS X" : min < 12 ? "OS X" : "macOS",
             codenames[min]);
    codename = buf;
  } else {
    char *lookfor = "SOFTWARE LICENSE AGREEMENT FOR ";
    FILE *fp = fopen("/System/Library/CoreServices/Setup "
                     "Assistant.app/Contents/Resources/en.lproj/OSXSoftwareLicense.rtf",
                     "r");
    if (fp != NULL) {
      for (int i = 0; i < 50 && fgets(buf, sizeof(buf), fp); ++i) {
        char *p = strstr(buf, lookfor);
        if (p) {
          codename = p + strlen(lookfor);
          codename[strlen(p) - strlen(lookfor) - 1] = '\0';
          break;
        }
      }
      fclose(fp);
    }
  }

  snprintf(distro_str, MAX_STRLEN, "%s %d.%d.%d (%s)", codename, maj, min, fix, build_ver);
  safe_strncpy(host_color, TLBL, MAX_STRLEN);

  return;
}

/*	detect_host
  detects the computer's hostname and active user and formats them
*/
void detect_host(void) {
  char *given_user = "Unknown";
  char given_host[MAX_STRLEN] = "Unknown";
  struct utsname host_info;

  given_user = getlogin();
  uname(&host_info);
  safe_strncpy(given_host, host_info.nodename, MAX_STRLEN);

  snprintf(host_str, MAX_STRLEN, "%s%s%s%s@%s%s%s%s", host_color, given_user, TNRM, TWHT, TNRM,
           host_color, given_host, TNRM);

  return;
}

/*	detect_kernel
  detects the computer's kernel
*/
void detect_kernel(void) {
  struct utsname kern_info;

  uname(&kern_info);
  snprintf(kernel_str, MAX_STRLEN, "%s %s %s", kern_info.sysname, kern_info.release,
           kern_info.machine);

  return;
}

/*	detect_uptime
  detects the computer's uptime
*/
void detect_uptime(void) {
  long long uptime = 0;
  unsigned int secs = 0;
  unsigned int mins = 0;
  unsigned int hrs = 0;
  unsigned int days = 0;

  /* three cheers for undocumented functions and structs */
  static mach_timebase_info_data_t timebase_info;

  if (timebase_info.denom == 0) {
    (void)mach_timebase_info(&timebase_info);
  }

  uptime = (long long)((mach_absolute_time() * timebase_info.numer) /
                       (1000 * 1000 * timebase_info.denom));
  uptime /= 1000;

  split_uptime(uptime, &secs, &mins, &hrs, &days);

  if (days > 0)
    snprintf(uptime_str, MAX_STRLEN, "%dd %dh %dm %ds", days, hrs, mins, secs);
  else
    snprintf(uptime_str, MAX_STRLEN, "%dh %dm %ds", hrs, mins, secs);

  return;
}

/*	detect_pkgs
  detects the number of packages installed on the computer
*/
void detect_pkgs(void) {
  int packages = 0;
  glob_t gl;

  if (glob("/usr/local/Cellar/*", GLOB_NOSORT, NULL, &gl) == 0) {
    packages = gl.gl_pathc;
  } else if (error) {
    ERR_REPORT("Failure while globbing packages.");
  }

  globfree(&gl);

  snprintf(pkgs_str, MAX_STRLEN, "%d", packages);

  return;
}

/*	detect_cpu
  detects the computer's CPU brand/name-string
*/
void detect_cpu(void) {
  size_t size = MAX_STRLEN;
  sysctlbyname("machdep.cpu.brand_string", cpu_str, &size, NULL, 0);
  return;
}

/*	detect_gpu
  detects the computer's GPU brand/name-string
*/
void detect_gpu(void) {
  CFMutableDictionaryRef matchDict = IOServiceMatching("IOPCIDevice");
  io_iterator_t iterator;
  if (IOServiceGetMatchingServices(kIOMasterPortDefault, matchDict, &iterator) ==
      kIOReturnSuccess) {
    io_registry_entry_t regEntry;
    while ((regEntry = IOIteratorNext(iterator))) {
      CFMutableDictionaryRef serviceDictionary;
      if (IORegistryEntryCreateCFProperties(regEntry, &serviceDictionary, kCFAllocatorDefault,
                                            kNilOptions) != kIOReturnSuccess) {
        IOObjectRelease(regEntry);
        continue;
      }
      const void *GPUModel = CFDictionaryGetValue(serviceDictionary, CFSTR("model"));
      if (GPUModel && CFGetTypeID(GPUModel) == CFDataGetTypeID())
        safe_strncpy(gpu_str, (char *)CFDataGetBytePtr((CFDataRef)GPUModel), MAX_STRLEN);
      CFRelease(serviceDictionary);
      IOObjectRelease(regEntry);
    }
    IOObjectRelease(iterator);
  }

  return;
}

/*	detect_disk
  detects the computer's total disk capacity and usage
*/
void detect_disk(void) {
  struct statfs disk_info;
  long disk_total = 0, disk_used = 0, disk_percentage = 0;

  if (!(statfs(getenv("HOME"), &disk_info))) {
    disk_total = ((disk_info.f_blocks * disk_info.f_bsize) / GB);
    disk_used = (((disk_info.f_blocks - disk_info.f_bfree) * disk_info.f_bsize) / GB);
    disk_percentage = (((float)disk_used / disk_total) * 100);
    snprintf(disk_str, MAX_STRLEN, "%ldG / %ldG (%ld%%)", disk_used, disk_total, disk_percentage);
  } else if (error) {
    ERR_REPORT("Could not stat $HOME for filesystem statistics.");
  }

  return;
}

/*	detect_mem
  detects the computer's total and used RAM
*/
void detect_mem(void) {
  int64_t used_mem = 0, total_mem = 0;
  vm_size_t page_size;
  mach_port_t mach_port;
  mach_msg_type_number_t count;
  vm_statistics64_data_t vm_stats;
  mach_port = mach_host_self();
  count = sizeof(vm_stats) / sizeof(natural_t);
  if (KERN_SUCCESS == host_page_size(mach_port, &page_size) &&
      KERN_SUCCESS ==
          host_statistics64(mach_port, HOST_VM_INFO, (host_info64_t)&vm_stats, &count)) {
    used_mem = ((int64_t)vm_stats.active_count + (int64_t)vm_stats.wire_count) * (int64_t)page_size;
  }

  size_t len = sizeof(total_mem);
  sysctlbyname("hw.memsize", &total_mem, &len, NULL, 0);

  snprintf(mem_str, MAX_STRLEN, "%lld%s / %lld%s", used_mem / MB, "MB", total_mem / MB, "MB");

  return;
}

/*	detect_shell
  detects the shell currently running on the computer
  --
  CAVEAT: shell version detection relies on the standard versioning format for
  each shell. If any shell's older (or newer versions) suddenly begin to use a
  new	scheme, the version may be displayed incorrectly.
  --
*/
void detect_shell(void) {
  FILE *shell_file;
  char *shell_name;
  char vers_str[MAX_STRLEN];

  shell_name = getenv("SHELL");

  if (shell_name == NULL) {
    if (error)
      ERR_REPORT("Could not detect a shell.");

    return;
  }

  if (STREQ(shell_name, "/bin/sh")) {
    safe_strncpy(shell_str, "POSIX sh", MAX_STRLEN);
  } else if (strstr(shell_name, "bash")) {
    shell_file = popen("bash --version | head -1", "r");
    fgets(vers_str, MAX_STRLEN, shell_file);
    snprintf(shell_str, MAX_STRLEN, "bash %.*s", 17, vers_str + 10);
    pclose(shell_file);
  } else if (strstr(shell_name, "zsh")) {
    shell_file = popen("zsh --version", "r");
    fgets(vers_str, MAX_STRLEN, shell_file);
    snprintf(shell_str, MAX_STRLEN, "zsh %.*s", 5, vers_str + 4);
    pclose(shell_file);
  } else if (strstr(shell_name, "csh")) {
    shell_file = popen("csh --version | head -1", "r");
    fgets(vers_str, MAX_STRLEN, shell_file);
    snprintf(shell_str, MAX_STRLEN, "csh %.*s", 7, vers_str + 5);
    pclose(shell_file);
  } else if (strstr(shell_name, "fish")) {
    shell_file = popen("fish --version", "r");
    fgets(vers_str, MAX_STRLEN, shell_file);
    snprintf(shell_str, MAX_STRLEN, "fish %.*s", 13, vers_str + 6);
    pclose(shell_file);
  } else if (strstr(shell_name, "dash") || strstr(shell_name, "ash") || strstr(shell_name, "ksh")) {
    /* i don't have a version detection system for these, yet */
    safe_strncpy(shell_str, shell_name, MAX_STRLEN);
  }

  return;
}

/*	detect_res
  detects the combined resolution of all monitors attached to the computer
*/
void detect_res(void) {
  uint32_t count = 0, chars = 0;
  CGGetOnlineDisplayList(UINT32_MAX, NULL, &count);
  CGDirectDisplayID displays[count];
  CGGetOnlineDisplayList(count, displays, &count);
  chars += snprintf(res_str, MAX_STRLEN, "%zu x %zu", CGDisplayPixelsWide(*displays),
                    CGDisplayPixelsHigh(*displays));
  for (int i = 1; i < count; ++i) {
    chars += snprintf(res_str + chars, MAX_STRLEN, ", %zu x %zu", CGDisplayPixelsWide(displays[i]),
                      CGDisplayPixelsHigh(displays[i]));
  }
  return;
}

/*	detect_de
  detects the desktop environment currently running on top of the OS.
  On OS X, this will always be Aqua.
*/
void detect_de(void) {
  safe_strncpy(de_str, "Aqua", MAX_STRLEN);

  return;
}

/*	detect_wm
  detects the window manager currently running on top of the OS.
  On OS X, this will always be the Quartz Compositor.
*/
void detect_wm(void) {
  safe_strncpy(wm_str, "Quartz Compositor", MAX_STRLEN);

  return;
}

/*	detect_wm_theme
  detects the theme associated with the WM detected in detect_wm().
  On OS X, there are dark and light theme, and various accent colours.
*/
void detect_wm_theme(void) {
  char *accents[] = {"Graphite", "Red", "Orange", "Yellow", "Green", "", "Purple", "Pink", "Blue"};
  char *color;
  Boolean accentExists;
  CFStringRef def = CFSTR(".GlobalPreferences");
  CFIndex accentColor =
      CFPreferencesGetAppIntegerValue(CFSTR("AppleAccentColor"), def, &accentExists);
  CFIndex aquaColorVariant =
      CFPreferencesGetAppIntegerValue(CFSTR("AppleAquaColorVariant"), def, NULL);
  CFPropertyListRef style = CFPreferencesCopyAppValue(CFSTR("AppleInterfaceStyle"), def);
  if (accentExists) {
    color = accents[accentColor + 1];
  } else if (aquaColorVariant == 6) {
    color = "Graphite";
  } else {
    color = "Blue";
  }
  snprintf(wm_theme_str, MAX_STRLEN, "%s %s", style ? "Dark" : "Light", color);
  return;
}

/*	detect_gtk
  OS X doesn't use GTK, so this function fills str with "Not Applicable"
*/
void detect_gtk(void) {
  safe_strncpy(gtk_str, "Not Applicable", MAX_STRLEN);

  return;
}
