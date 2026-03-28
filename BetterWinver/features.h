//BetterWinver 1.5.0
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
extern Font* gdiFont;
extern FontFamily* ffSegoeV;
extern StringFormat* pStringFormatCenter;

extern ImageAttributes* pImgAttr;
extern Image* imgLogo;       
extern Bitmap* cachedLogo;   
extern HBITMAP hBmpRes;     
extern int bmpWidth;        
extern int bmpHeight;

extern Pen* pLinePen;

extern GraphicsPath* buttonPath;
extern float g_startX, g_startY, g_realW, g_lineY;
extern RectF g_layoutRectBody;
extern StringFormat* g_pStringFormatCenter;
extern SolidBrush *pBtnFillNormal, *pBtnFillHover, *pBtnFillPressed, *pBtnTextBrushStatic;
extern Pen* pBtnBorderPenStatic;
struct CustomButton {
    RECT rect;
    bool hover;
    bool pressed;
};

inline int ScaleValue(int value, UINT dpi) {
    return MulDiv(value, dpi, 96);
}

inline float ScaleValueF(float value, UINT dpi) {
    return (value * (float)dpi) / 96.0f;
}

inline void windowTheme(HWND hwnd) {
    if (compCheck >= 22000) {
        int backdropType = DWMSBT_MAINWINDOW;
        DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdropType, sizeof(backdropType));
        MARGINS margins = {-1, -1, -1, -1};
        DwmExtendFrameIntoClientArea(hwnd, &margins);
    }
}

inline void UpdateTheme(HWND hwnd, bool& DarkMode, COLORREF& bgCol, COLORREF& txtCol, HBRUSH& hBrushBg, Font*& pGdiFont, FontFamily*& pFontFamily, CustomButton& btn) {
    DarkModeCheck();
    DarkMode = isDarkModeEnabled;

    bgCol = DarkMode ? RGB(32, 32, 32) : RGB(255, 255, 255);
    txtCol = DarkMode ? RGB(255, 255, 255) : RGB(0, 0, 0);

    if (pTextBrush) delete pTextBrush;
    if (pLinePen) delete pLinePen;
    if (pImgAttr) delete pImgAttr;
    if (pBtnFillNormal) delete pBtnFillNormal;
    if (pBtnFillHover) delete pBtnFillHover;
    if (pBtnFillPressed) delete pBtnFillPressed;
    if (pBtnTextBrushStatic) delete pBtnTextBrushStatic;
    if (pBtnBorderPenStatic) delete pBtnBorderPenStatic;

    Color gdiTxtCol; gdiTxtCol.SetFromCOLORREF(txtCol);
    pTextBrush = new SolidBrush(gdiTxtCol);
    
    Color lineCol = DarkMode ? Color(80, 80, 80) : Color(220, 220, 220);
    pLinePen = new Pen(lineCol, 1.0f);

    if (DarkMode) {
        pBtnFillNormal = new SolidBrush(Color(30, 255, 255, 255));
        pBtnFillHover = new SolidBrush(Color(60, 255, 255, 255));
        pBtnFillPressed = new SolidBrush(Color(100, 255, 255, 255));
        pBtnTextBrushStatic = new SolidBrush(Color(255, 255, 255));
        pBtnBorderPenStatic = new Pen(Color(80, 255, 255, 255), 0.1f);
    } else {
        pBtnFillNormal = new SolidBrush(Color(20, 0, 0, 0));
        pBtnFillHover = new SolidBrush(Color(40, 0, 0, 0));
        pBtnFillPressed = new SolidBrush(Color(100, 0, 0, 0));
        pBtnTextBrushStatic = new SolidBrush(Color(0, 0, 0));
        pBtnBorderPenStatic = new Pen(Color(50, 0, 0, 0), 0.1f);
    }

    if (!g_pStringFormatCenter) {
        g_pStringFormatCenter = new StringFormat();
        g_pStringFormatCenter->SetAlignment(StringAlignmentCenter);
        g_pStringFormatCenter->SetLineAlignment(StringAlignmentCenter);
    }

    UINT dpi = GetDpiForWindow(hwnd);
    g_startX = ScaleValueF(30.0f, dpi);
    g_startY = ScaleValueF(5.0f, dpi);
    g_lineY = ScaleValueF(75.0f, dpi);

    RECT rc; GetClientRect(hwnd, &rc);
    int width = rc.right;
    int height = rc.bottom;
    g_realW = (float)width - (g_startX * 2);

    g_layoutRectBody = RectF(g_startX, ScaleValueF(90.0f, dpi), g_realW, ScaleValueF(210.0f, dpi));

    if (pGdiFont) delete pGdiFont;
    if (pFontFamily) delete pFontFamily;
    pFontFamily = new FontFamily(L"Segoe UI Variable Text");
    if (pFontFamily->GetLastStatus() != Ok) {
        delete pFontFamily;
        pFontFamily = new FontFamily(L"Segoe UI");
    }
    pGdiFont = new Font(pFontFamily, ScaleValueF(12.0f, dpi), FontStyleRegular, UnitPixel);

    int btnw = ScaleValue(75, dpi);
    int btnh = ScaleValue(22, dpi);
    btn.rect = { width - btnw - ScaleValue(30, dpi), height - btnh - ScaleValue(15, dpi), width - ScaleValue(30, dpi), height - ScaleValue(15, dpi) };

    if (buttonPath) delete buttonPath;
    buttonPath = new GraphicsPath();
    float r = 8.0f;
    RectF rf((REAL)btn.rect.left, (REAL)btn.rect.top, (REAL)btnw, (REAL)btnh);
    buttonPath->AddArc(rf.X, rf.Y, r, r, 180, 90);
    buttonPath->AddArc(rf.X + rf.Width - r, rf.Y, r, r, 270, 90);
    buttonPath->AddArc(rf.X + rf.Width - r, rf.Y + rf.Height - r, r, r, 0, 90);
    buttonPath->AddArc(rf.X, rf.Y + rf.Height - r, r, r, 90, 90);
    buttonPath->CloseFigure();

    if (hBrushBg) DeleteObject(hBrushBg);
    hBrushBg = CreateSolidBrush(bgCol);
    BOOL useDarkMode = DarkMode;
    DwmSetWindowAttribute(hwnd, 20, &useDarkMode, sizeof(useDarkMode));
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

    Rect rect(0, 0, bmpWidth, bmpHeight);
    BitmapData srcData, dstData;

    tempBmp.LockBits(&rect, ImageLockModeRead, PixelFormat32bppARGB, &srcData);
    cachedLogo->LockBits(&rect, ImageLockModeWrite, PixelFormat32bppARGB, &dstData);

    DWORD* srcPixels = (DWORD*)srcData.Scan0;
    DWORD* dstPixels = (DWORD*)dstData.Scan0;

    DWORD baseR = isDarkModeEnabled ? 255 : 0;
    DWORD baseG = isDarkModeEnabled ? 255 : 0;
    DWORD baseB = isDarkModeEnabled ? 255 : 0;
    DWORD blueR = 0, blueG = 120, blueB = 215;

    for (int i = 0; i < bmpWidth * bmpHeight; i++) {
        DWORD pixel = srcPixels[i];
        
        BYTE b = (pixel & 0x000000FF);
        BYTE g = (pixel & 0x0000FF00) >> 8;
        BYTE r = (pixel & 0x00FF0000) >> 16;

        int brightness = (r + g + b) / 3;
        int maxVal = std::max(r, std::max(g, b));
        int minVal = std::min(r, std::min(g, b));
        int diff = maxVal - minVal;

        BYTE outA, outR, outG, outB;

        if (diff > 25) { 
            float alphaFactor = std::min(1.0f, (float)diff / 25.0f);
            outA = (BYTE)(alphaFactor * 255.0f);
            outR = blueR; outG = blueG; outB = blueB;
        } else {
            float alpha = 0;
            if (brightness < 60) alpha = 255.0f;
            else if (brightness < 150) alpha = ((150.0f - (float)brightness) / (150.0f - 60.0f)) * 255.0f;
            
            outA = (BYTE)alpha;
            outR = baseR; outG = baseG; outB = baseB;
        }

        dstPixels[i] = (outA << 24) | (outR << 16) | (outG << 8) | outB;
    }

    tempBmp.UnlockBits(&srcData);
    cachedLogo->UnlockBits(&dstData);
}

inline void logoCreation(int resourceNumber){
    if (imgLogo) { delete imgLogo; imgLogo = nullptr; }
    if (cachedLogo) { delete cachedLogo; cachedLogo = nullptr; }
    if (hBmpRes) { DeleteObject(hBmpRes); hBmpRes = NULL; }

    wchar_t systemPath[MAX_PATH];
    wchar_t dllPath[MAX_PATH];
    
    GetSystemDirectoryW(systemPath, MAX_PATH);
    wsprintfW(dllPath, L"%s\\..\\Branding\\Basebrd\\basebrd.dll", systemPath);

    HMODULE hLib = LoadLibraryExW(dllPath, NULL, LOAD_LIBRARY_AS_DATAFILE);

    if (hLib) {
        if (compCheck >= 22000) {
            IStream* pStream = CreateStreamOnResource(hLib, MAKEINTRESOURCEW(resourceNumber), L"IMAGE");
            if (pStream) {
                imgLogo = new Image(pStream);
                pStream->Release();
            }
        } else {
            hBmpRes = (HBITMAP)LoadImageW(hLib, MAKEINTRESOURCEW(resourceNumber), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
            if (hBmpRes) {
                BITMAP bmp;
                GetObject(hBmpRes, sizeof(BITMAP), &bmp);
                bmpWidth = bmp.bmWidth;
                bmpHeight = bmp.bmHeight;
                bitmapCache(hBmpRes, bmpWidth, bmpHeight, isDarkModeEnabled);
            }
        }
        FreeLibrary(hLib);
    }
}

#endif
