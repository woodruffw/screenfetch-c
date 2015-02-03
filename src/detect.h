/*	detect.h
**	Author: William Woodruff
**	-------------
**
**	The detection functions used by screenfetch-c are prototyped here.
**	Like the rest of screenfetch-c, this file is licensed under the MIT license.
*/

#ifndef SCREENFETCH_C_DETECT_H
#define SCREENFETCH_C_DETECT_H

void detect_distro(void);
void detect_host(void);
void detect_kernel(char *str);
void detect_uptime(char *str);
void detect_pkgs(char *str, const char *distro_str);
void detect_cpu(char *str);
void detect_gpu(char *str);
void detect_disk(char *str);
void detect_mem(char *str);
void detect_shell(char *str);
void detect_res(char *str);
void detect_de(char *str);
void detect_wm(char *str);
void detect_wm_theme(char *str, const char *wm_str);
void detect_gtk(char *str1, char *str2, char *str3);

#endif /* SCREENFETCH_C_DETECT_H */
