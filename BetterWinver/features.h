//BetterWinver 1.4.0
#ifndef FEATURES_H
#define FEATURES_H

#include <windows.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <wingdi.h>
#include <shlwapi.h>

#include "infoGet.h"

using namespace Gdiplus;

extern int compCheck;
extern bool isDarkModeEnabled;
extern SolidBrush* pTextBrush;
extern Pen* pLinePen;
extern ImageAttributes* pImgAttr;

inline void UpdateTheme(HWND hwnd, bool& DarkMode, COLORREF& bgCol, COLORREF& txtCol, HBRUSH& hBrushBg, Font*& pGdiFont, FontFamily*& pFontFamily) {
    DarkModeCheck();
    DarkMode = isDarkModeEnabled;

    bgCol = DarkMode ? RGB(32, 32, 32) : RGB(255, 255, 255);
    txtCol = DarkMode ? RGB(255, 255, 255) : RGB(0, 0, 0);

    if (pTextBrush) { delete pTextBrush; pTextBrush = nullptr; }
    if (pLinePen) { delete pLinePen; pLinePen = nullptr; }
    if (pImgAttr) { delete pImgAttr; pImgAttr = nullptr; }

    Color gdiTxtCol;
    gdiTxtCol.SetFromCOLORREF(txtCol);
    pTextBrush = new SolidBrush(gdiTxtCol);

    Color lineCol = DarkMode ? Color(80, 80, 80) : Color(220, 220, 220);
    pLinePen = new Pen(lineCol, 1.0f);

    pImgAttr = new ImageAttributes();
    pImgAttr->SetWrapMode(WrapModeTileFlipXY);

    if (hBrushBg) DeleteObject(hBrushBg);
    hBrushBg = CreateSolidBrush(bgCol);

    BOOL useDarkMode = DarkMode;
    DwmSetWindowAttribute(hwnd, 20, &useDarkMode, sizeof(useDarkMode));

    UINT dpi = GetDpiForWindow(hwnd);

    if (pGdiFont) { delete pGdiFont; pGdiFont = nullptr; }
    if (pFontFamily) { delete pFontFamily; pFontFamily = nullptr; }

    pFontFamily = new FontFamily(L"Segoe UI Variable Text");
    if (pFontFamily->GetLastStatus() != Ok) {
        delete pFontFamily;
        pFontFamily = new FontFamily(L"Segoe UI");
    }

    float scaledFontSize = (12.0f * (float)dpi) / 96.0f;
    pGdiFont = new Font(pFontFamily, scaledFontSize, FontStyleRegular, UnitPixel);

    InvalidateRect(hwnd, NULL, TRUE);
}

inline int ScaleValue(int value, UINT dpi) {
    return MulDiv(value, dpi, 96);
}

inline IStream* CreateStreamOnResource(HMODULE hModule, LPCWSTR lpName, LPCWSTR lpType) {
    HRSRC hRsrc = FindResourceW(hModule, lpName, lpType);
    if (!hRsrc) return NULL;
    
    DWORD dwSize = SizeofResource(hModule, hRsrc);
    HGLOBAL hGlobal = LoadResource(hModule, hRsrc);
    if (!hGlobal) return NULL;

    void* pBuffer = LockResource(hGlobal);
    
    return SHCreateMemStream((BYTE*)pBuffer, dwSize);
}

extern Bitmap* cachedLogo;
inline void bitmapCache(HBITMAP hBmpRes, int bmpWidth, int bmpHeight, bool isDarkModeEnabled) {
    if (!hBmpRes) return;
    if (cachedLogo) { delete cachedLogo; cachedLogo = nullptr; }

    cachedLogo = new Bitmap(bmpWidth, bmpHeight, PixelFormat32bppARGB);
    Bitmap tempBmp(hBmpRes, NULL);
    Graphics g(cachedLogo);

    if (compCheck < 10240) {
        ImageAttributes imgAttr;
        
        Color lowGray(200, 200, 200);
        Color highGray(255, 255, 255);
        imgAttr.SetColorKey(lowGray, highGray, ColorAdjustTypeBitmap);

        g.DrawImage(&tempBmp, Rect(0, 0, bmpWidth, bmpHeight), 0, 0, bmpWidth, bmpHeight, UnitPixel, &imgAttr);
        return;
    }

    //Windows 10
    Color baseColor = isDarkModeEnabled ? Color(255, 255, 255) : Color(0, 0, 0);
    Color logoBlue(255, 0, 120, 215);

    for (int y = 0; y < bmpHeight; y++) {
        for (int x = 0; x < bmpWidth; x++) {
            Color c;
            tempBmp.GetPixel(x, y, &c);
            int r = c.GetR(), g = c.GetG(), b = c.GetB();
            int brightness = (r + g + b) / 3;
            int diff = std::max(r, std::max(g, b)) - std::min(r, std::min(g, b));

            if (diff > 25) { 
                float alphaFactor = std::min(1.0f, (float)diff / 25.0f);
                cachedLogo->SetPixel(x, y, Color((BYTE)(alphaFactor * 255.0f), logoBlue.GetR(), logoBlue.GetG(), logoBlue.GetB()));
            } else {
                float alpha = 0;
                if (brightness < 60) alpha = 255.0f;
                else if (brightness < 150) alpha = ((150.0f - (float)brightness) / (150.0f - 60.0f)) * 255.0f;
                cachedLogo->SetPixel(x, y, Color((BYTE)alpha, baseColor.GetR(), baseColor.GetG(), baseColor.GetB()));
            }
        }
    }
}

inline float ScaleValueF(float value, UINT dpi) {
    return (value * (float)dpi) / 96.0f;
}
#endif
