/*	bitmap.h
**	Author: djcj <djcj@gmx.de>
**	-------------
**
**	Function prototypes for bitmap.c.
**	Like the rest of screenfetch-c, this file is licensed under the MIT license.
*/

#ifndef SCREENFETCH_C_BITMAP_H
#define SCREENFETCH_C_BITMAP_H

#include <windows.h>

int createBitmapFile(LPCTSTR lpszFileName, HBITMAP hBitmap, HDC hdc);

#endif /* SCREENFETCH_C_BITMAP_H */
