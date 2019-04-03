/*	bitmap.c
**	Author: djcj <djcj@gmx.de>
**	-------------
**
**	The screenshot functions used by screenfetch-c on Windows are implemented here.
**	Like the rest of screenfetch-c, this file is licensed under the MIT license.
*/

#include <windows.h>

int createBitmapFile(LPCTSTR lpszFileName, HBITMAP hBitmap, HDC hdc) {
  BITMAP bitmap;
  BITMAPFILEHEADER bitmapFileHeader;

  if (!GetObject(hBitmap, sizeof(BITMAP), &bitmap)) {
    return -1;
  }

  int infoHeaderSize = sizeof(BITMAPINFOHEADER);
  int rgbQuadSize = sizeof(RGBQUAD);
  int bits = bitmap.bmPlanes * bitmap.bmBitsPixel;
  int infoHeaderSizeImage = ((bitmap.bmWidth * bits + 31) & ~31) / 8 * bitmap.bmHeight;

  PBITMAPINFO pBitmapInfo =
      (PBITMAPINFO)LocalAlloc(LPTR, infoHeaderSize + rgbQuadSize * (1 << bits));

  pBitmapInfo->bmiHeader.biSize = infoHeaderSize;
  pBitmapInfo->bmiHeader.biBitCount = bitmap.bmBitsPixel;
  pBitmapInfo->bmiHeader.biClrImportant = 0;
  pBitmapInfo->bmiHeader.biClrUsed = 0;
  pBitmapInfo->bmiHeader.biCompression = BI_RGB;
  pBitmapInfo->bmiHeader.biHeight = bitmap.bmHeight;
  pBitmapInfo->bmiHeader.biWidth = bitmap.bmWidth;
  pBitmapInfo->bmiHeader.biPlanes = bitmap.bmPlanes;
  pBitmapInfo->bmiHeader.biSizeImage = infoHeaderSizeImage;

  PBITMAPINFOHEADER pBitmapInfoHeader = (PBITMAPINFOHEADER)pBitmapInfo;
  LPBYTE mem = GlobalAlloc(GMEM_FIXED, pBitmapInfoHeader->biSizeImage);

  if (!mem) {
    return -1;
  }

  if (!GetDIBits(hdc, hBitmap, 0, pBitmapInfoHeader->biHeight, mem, pBitmapInfo, DIB_RGB_COLORS)) {
    return -1;
  }

  int fileHeaderSize = sizeof(BITMAPFILEHEADER);
  int fileAndInfoHeaderSize =
      fileHeaderSize + infoHeaderSize + pBitmapInfoHeader->biClrUsed * rgbQuadSize;

  bitmapFileHeader.bfType = 'B' + ('M' << 8);
  bitmapFileHeader.bfOffBits = fileAndInfoHeaderSize;
  bitmapFileHeader.bfSize = fileAndInfoHeaderSize + pBitmapInfoHeader->biSizeImage;
  bitmapFileHeader.bfReserved1 = 0;
  bitmapFileHeader.bfReserved2 = 0;

  HANDLE handleFile =
      CreateFile(lpszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

  if (handleFile == INVALID_HANDLE_VALUE) {
    return -1;
  }

  if (!WriteFile(handleFile, &bitmapFileHeader, fileHeaderSize, NULL, NULL)) {
    return -1;
  }

  if (!WriteFile(handleFile, pBitmapInfoHeader,
                 infoHeaderSize + pBitmapInfoHeader->biClrUsed * rgbQuadSize, NULL, NULL)) {
    return -1;
  }

  if (!WriteFile(handleFile, mem, pBitmapInfoHeader->biSizeImage, NULL, NULL)) {
    return -1;
  }

  if (!CloseHandle(handleFile)) {
    return -1;
  }

  GlobalFree(mem);
  return 0;
}
