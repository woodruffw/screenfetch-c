/* bitmap.h
**
** Copyright (c) 2016 Microsoft
** Source: https://msdn.microsoft.com/de-de/library/windows/desktop/dd145119(v=vs.85).aspx
**
** Function prototypes for bitmap.c.
**
** According to section 2b of the Microsoft Developer Services Agreement
** this code is released under the Microsoft Limited Public License.
*/

#ifndef SCREENFETCH_C_BITMAP_H
#define SCREENFETCH_C_BITMAP_H

#include <Windows.h>

PBITMAPINFO createBitmapInfoStruct(HBITMAP hBmp);
int createBmpFile(LPTSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC);

#endif /* SCREENFETCH_C_BITMAP_H */
