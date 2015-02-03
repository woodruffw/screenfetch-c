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
void detect_kernel(void);
void detect_uptime(void);
void detect_pkgs(void);
void detect_cpu(void);
void detect_gpu(void);
void detect_disk(void);
void detect_mem(void);
void detect_shell(void);
void detect_res(char *str);
void detect_de(char *str);
void detect_wm(char *str);
void detect_wm_theme(char *str, const char *wm_str);
void detect_gtk(char *str1, char *str2, char *str3);

#endif /* SCREENFETCH_C_DETECT_H */
