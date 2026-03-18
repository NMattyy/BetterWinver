//BetterWinver 1.4.1
#include <windows.h>
#include <dwmapi.h>
#include <string>
#include <gdiplus.h>
#include <wingdi.h>
#include <shlwapi.h>

#include "infoGet.h"
#include "features.h"
#include "translation.h"

using namespace Gdiplus;
using namespace std;

HFONT hFont, hFontBold;

wstring build;
wstring OSName;
wstring NT;
wstring commercialVersion;
wstring user;
int compCheck;
bool isDarkModeEnabled;

COLORREF coloreSfondo = RGB(255, 255, 255);
COLORREF coloreTesto = RGB(0, 0, 0);
HBRUSH hBrushSfondo = NULL;
bool mouseHiglight = false;
Image* imgLogo = NULL;
HBITMAP hBmpRes = NULL;
int bmpWidth = 0, bmpHeight = 0;
FontFamily* ffSegoeV = nullptr;
Font* gdiFont = nullptr;
SolidBrush* pTextBrush = nullptr;
Pen* pLinePen = nullptr;
ImageAttributes* pImgAttr = nullptr;
Bitmap* cachedLogo = nullptr;

LRESULT CALLBACK windowManager(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    int resourceNumber;
    if (compCheck >= 10240) {
        resourceNumber = 2123;
    } else { 
        resourceNumber = 2121;
    }

    switch (msg) {
        case WM_CREATE: {
            UpdateTheme(hwnd, isDarkModeEnabled, coloreSfondo, coloreTesto, hBrushSfondo, gdiFont, ffSegoeV);
            logoCreation(resourceNumber);
            InvalidateRect(hwnd, NULL, TRUE);   
            return 0;
        }

        case WM_PAINT: {
            UpdateTheme(hwnd, isDarkModeEnabled, coloreSfondo, coloreTesto, hBrushSfondo, gdiFont, ffSegoeV);
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            Graphics graphics(hdc);
            
            graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
            graphics.SetSmoothingMode(SmoothingModeHighQuality);
            graphics.SetPixelOffsetMode(PixelOffsetModeHalf);
            graphics.SetTextRenderingHint(isDarkModeEnabled ? 
                TextRenderingHintClearTypeGridFit : TextRenderingHintAntiAliasGridFit);

            if (compCheck < 22000) {
                Color backColor;
                backColor.SetFromCOLORREF(coloreSfondo);
                graphics.Clear(backColor);
            } else {
                graphics.Clear(Color(0, 0, 0, 0));
            }

            UINT dpi = GetDpiForWindow(hwnd);
            float startX = ScaleValueF(30.0f, dpi);
            float startY = ScaleValueF(5.0f, dpi);
            float realW = (float)ps.rcPaint.right - (startX * 2);

            if (imgLogo && imgLogo->GetLastStatus() == Ok) {
                float aspectHeight = (realW / (float)imgLogo->GetWidth()) * (float)imgLogo->GetHeight();
                graphics.DrawImage(imgLogo, RectF(startX, startY, realW, aspectHeight));
            } 
            else if (cachedLogo && cachedLogo->GetLastStatus() == Ok) {
                float aspectHeight = (realW / (float)cachedLogo->GetWidth()) * (float)cachedLogo->GetHeight();
                graphics.DrawImage(cachedLogo, RectF(startX, startY, realW, aspectHeight), 0.0f, 0.0f, (REAL)cachedLogo->GetWidth(), (REAL)cachedLogo->GetHeight(), UnitPixel, pImgAttr);
            }

            float lineY = ScaleValueF(75.0f, dpi);
            graphics.DrawLine(pLinePen, startX, lineY, (REAL)ps.rcPaint.right - startX, lineY);

            RectF layoutRectBody(startX, ScaleValueF(90.0f, dpi), realW, ScaleValueF(210.0f, dpi)); 
            graphics.DrawString(string_4().c_str(), -1, gdiFont, layoutRectBody, NULL, pTextBrush);

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_ERASEBKGND: {
            return 1; 
        }

        case WM_CTLCOLORSTATIC: {
            HDC hdcStatic = (HDC)wp;
            SetBkMode(hdcStatic, TRANSPARENT);
            SetTextColor(hdcStatic, coloreTesto);
            return (LRESULT)hBrushSfondo;
        }
        
        case WM_DRAWITEM: {
            LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lp;
            if (pDIS->CtlID == 1) { 
                Graphics graphics(pDIS->hDC);                
                graphics.SetSmoothingMode(SmoothingModeHighQuality);
                graphics.SetPixelOffsetMode(PixelOffsetModeHighQuality);

                Color winBackColor(isDarkModeEnabled ? Color(32, 32, 32) : Color(255, 255, 255));
                SolidBrush clearBrush(winBackColor);
                graphics.FillRectangle(&clearBrush, 0, 0, pDIS->rcItem.right, pDIS->rcItem.bottom);

                Color btnColor, borderColor, textColor;
                bool isHovered = mouseHiglight;
                bool isPressed = (pDIS->itemState & ODS_SELECTED);

                if (isDarkModeEnabled) {
                    graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
                    if (isPressed) btnColor = Color(100, 100, 100, 100);    
                    else if (isHovered) btnColor = Color(60, 255, 255, 255);                    
                    else btnColor = Color(30, 255, 255, 255);
                    
                    borderColor = Color(100, 100, 100);
                    textColor = Color(255, 255, 255);
                } else {
                    graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
                    if (isPressed) btnColor = Color(100, 0, 0, 0);
                    else if (isHovered) btnColor = Color(40, 0, 0, 0);
                    else btnColor = Color(20, 0, 0, 0);
                    
                    borderColor = Color(30, 0, 0, 0);
                    textColor = Color(0, 0, 0);
                }

                float r = 8.0f;
                float w = (float)pDIS->rcItem.right - 1.5f;
                float h = (float)pDIS->rcItem.bottom - 1.5f;
                float offset = 0.5f;

                GraphicsPath path;
                path.AddArc(offset, offset, r, r, 180, 90);
                path.AddArc(w - r, offset, r, r, 270, 90);
                path.AddArc(w - r, h - r, r, r, 0, 90);
                path.AddArc(offset, h - r, r, r, 90, 90);
                path.CloseFigure();

                SolidBrush brush(btnColor);
                Pen pen(borderColor, 1.0f);
                
                graphics.FillPath(&brush, &path);
                graphics.DrawPath(&pen, &path);

                StringFormat format;
                format.SetAlignment(StringAlignmentCenter);
                format.SetLineAlignment(StringAlignmentCenter);
                SolidBrush textBrush(textColor);

                UINT dpi = GetDpiForWindow(hwnd);
                const wchar_t* fontName = L"Segoe UI Variable Display";
                    if (FontFamily(fontName).GetLastStatus() != Ok) {
                        fontName = L"Segoe UI";
                    }

                    float scaledBtnFontSize = (9.0f * (float)dpi) / 72.0f;
                    FontFamily fontFamily(fontName);
                    Font btnFont(&fontFamily, scaledBtnFontSize, FontStyleRegular, UnitPixel);
                
                RectF rectText(0, 0, (REAL)pDIS->rcItem.right, (REAL)pDIS->rcItem.bottom);
                graphics.DrawString(L"OK", -1, &btnFont, rectText, &format, &textBrush);

                return TRUE;
            }
            break;
        }

        case WM_SETCURSOR: {
            if ((HWND)wp == GetDlgItem(hwnd, 1)) {
                if (!mouseHiglight) {
                    mouseHiglight = true;
                    InvalidateRect((HWND)wp, NULL, FALSE);
                }
                SetCursor(LoadCursor(NULL, IDC_ARROW));
                return TRUE; 
            } 
            else {
                if (mouseHiglight) {
                    mouseHiglight = false;
                    InvalidateRect(GetDlgItem(hwnd, 1), NULL, FALSE);
                }
                SetCursor(LoadCursor(NULL, IDC_ARROW));
                return TRUE;
            }
        }

        case WM_COMMAND: {
            if (LOWORD(wp) == 1) {
                SendMessage(hwnd, WM_CLOSE, 0, 0);
            }
            break;
        }

        case WM_KEYDOWN: {
            if (wp == VK_RETURN || wp == VK_ESCAPE) {
                SendMessage(hwnd, WM_CLOSE, 0, 0);
            }
            return 0;
        }
        
        case WM_SETTINGCHANGE: {
            UpdateTheme(hwnd, isDarkModeEnabled, coloreSfondo, coloreTesto, hBrushSfondo, gdiFont, ffSegoeV);
            return 0;
        }

        case WM_DPICHANGED: {
            LPRECT lprcNewScale = (LPRECT)lp;
            UINT newDpi = LOWORD(wp);
            UpdateTheme(hwnd, isDarkModeEnabled, coloreSfondo, coloreTesto, hBrushSfondo, gdiFont, ffSegoeV);
            logoCreation(resourceNumber);

            SetWindowPos(hwnd, NULL, lprcNewScale->left, lprcNewScale->top, lprcNewScale->right - lprcNewScale->left, lprcNewScale->bottom - lprcNewScale->top, SWP_NOZORDER | SWP_NOACTIVATE);

            if (gdiFont) { delete gdiFont; gdiFont = nullptr; }
            if (ffSegoeV) { delete ffSegoeV; ffSegoeV = nullptr; }

            HWND hBtn = GetDlgItem(hwnd, 1);
            int btnW = ScaleValue(68, newDpi);
            int btnH = ScaleValue(22, newDpi);
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            int btnX = clientRect.right - btnW - ScaleValue(30, newDpi);
            int btnY = clientRect.bottom - btnH - ScaleValue(15, newDpi);
            MoveWindow(hBtn, btnX, btnY, btnW, btnH, TRUE);

            InvalidateRect(hwnd, NULL, TRUE);
            UpdateWindow(hwnd);
            return 0;
        }

        case WM_DESTROY: {
            if (hBrushSfondo) DeleteObject(hBrushSfondo);
            if (hFont) DeleteObject(hFont);
            if (hBmpRes) DeleteObject(hBmpRes);
            if (imgLogo) delete imgLogo;
            if (ffSegoeV) delete ffSegoeV;
            if (gdiFont) delete gdiFont;
            if (pTextBrush) delete pTextBrush;
            if (pLinePen) delete pLinePen;
            if (pImgAttr) delete pImgAttr;
            if (cachedLogo) delete cachedLogo;
            
            PostQuitMessage(0);
            break;
        }
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow) {
    UINT currentDpi = GetSystemDPI();
    INITCOMMONCONTROLSEX icce;
    icce.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icce.dwICC = ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icce);
    
    build = buildGet();
    compCheck = stoi(build);
    if (compCheck < 9200) {
        MessageBoxW(NULL, string_1().c_str(), string_2().c_str(), MB_OK | MB_ICONWARNING | MB_SETFOREGROUND);
        return 0;
    }

    int clientW = ScaleValue(400, currentDpi);
    int clientH = ScaleValue(390, currentDpi);

    RECT rc = { 0, 0, clientW, clientH };
    AdjustWindowRectEx(&rc, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, FALSE, 0);

    int windowW = rc.right - rc.left;
    int windowH = rc.bottom - rc.top;

    OSName = OSGet();
    NT = ntGet();
    commercialVersion = commercialVersionGet();
    user = userGet();
    DarkModeCheck();

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASSW wc = {0};
    wc.lpfnWndProc = windowManager;
    wc.hInstance = hInst;
    wc.hbrBackground = NULL;
    wc.lpszClassName = L"BetterWinver";
    RegisterClassW(&wc);

    HWND hwnd = CreateWindowW(L"BetterWinver", string_3().c_str(), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, windowW, windowH, NULL, NULL, hInst, NULL);

    int btnW = ScaleValue(68, currentDpi);
    int btnH = ScaleValue(22, currentDpi);
    int btnX = clientW - btnW - ScaleValue(30, currentDpi); 
    int btnY = clientH - btnH - ScaleValue(15, currentDpi);

    if (compCheck >= 22000) {
        int backdropType = DWMSBT_MAINWINDOW;
        DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdropType, sizeof(backdropType));
        MARGINS margins = {-1, -1, -1, -1};
        DwmExtendFrameIntoClientArea(hwnd, &margins);
    }

    HWND hButton = CreateWindowW(L"BUTTON", L"OK", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, btnX, btnY, btnW, btnH, hwnd, (HMENU)1, hInst, NULL);
    SendMessage(hButton, WM_SETFONT, (WPARAM)hFont, TRUE);

    ShowWindow(hwnd, nShow);
    UpdateWindow(hwnd);

    PostMessage(hwnd, WM_NULL, 0, 0);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    GdiplusShutdown(gdiplusToken);
    return msg.wParam;
}
