/*	detect.h
	Author: William Woodruff
	-------------

	The detection functions used by screenfetch-c are prototyped here.
	Like the rest of screenfetch-c, this file is licensed under the MIT license.
	You should have received a copy of it with this code.
*/

#ifndef SCREENFETCH_C_DETECT_H
#define SCREENFETCH_C_DETECT_H

void detect_distro(char *str, bool error);
void detect_arch(char *str);
void detect_host(char *str);
void detect_kernel(char *str);
void detect_uptime(char *str);
void detect_pkgs(char *str, const char *distro_str, bool error);
void detect_cpu(char *str);
void detect_gpu(char *str, bool error);
void detect_disk(char *str, bool error);
void detect_mem(char *str);
void detect_shell(char *str, bool error);
void detect_res(char *str, bool error);
void detect_de(char *str);
void detect_wm(char *str, bool error);
void detect_wm_theme(char *str);
void detect_gtk(char *str);

#endif /* SCREENFETCH_C_DETECT_H */
