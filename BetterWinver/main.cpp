//BetterWinver 1.5.0
#include <windows.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <wingdi.h>
#include <shlwapi.h>

#include "infoGet.h"
#include "features.h"
#include "translation.h"

using namespace Gdiplus;

HFONT hFont, hFontBold;
COLORREF coloreSfondo = RGB(255, 255, 255);
COLORREF coloreTesto = RGB(0, 0, 0);
HBRUSH hBrushSfondo = NULL;

wchar_t NT[64];
wchar_t build[64];
wchar_t OSName[128];
wchar_t commercialVersion[64];
wchar_t user[128];
int compCheck;
bool isDarkModeEnabled;

Image* imgLogo = NULL;
HBITMAP hBmpRes = NULL;
int bmpWidth = 0, bmpHeight = 0;
ImageAttributes* pImgAttr = nullptr;
Bitmap* cachedLogo = nullptr;

FontFamily* ffSegoeV = nullptr;
Font* gdiFont = nullptr;
SolidBrush* pTextBrush = nullptr;

Pen* pLinePen = nullptr;

CustomButton btn = {};
GraphicsPath* buttonPath = nullptr;
float g_startX, g_startY, g_realW, g_lineY;
RectF g_layoutRectBody;
StringFormat* g_pStringFormatCenter = nullptr;
SolidBrush* pBtnFillNormal = nullptr;
SolidBrush* pBtnFillHover = nullptr;
SolidBrush* pBtnFillPressed = nullptr;
SolidBrush* pBtnTextBrushStatic = nullptr;
Pen* pBtnBorderPenStatic = nullptr;

LRESULT CALLBACK windowManager(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    int resourceNumber;
    if (compCheck >= 10240) {
        resourceNumber = 2123;
    } else { 
        resourceNumber = 2121;
    }

    switch (msg) {
        case WM_CREATE: {
            HMENU hSysMenu = GetSystemMenu(hwnd, FALSE);
            if (hSysMenu) {
                RemoveMenu(hSysMenu, SC_SIZE, MF_BYCOMMAND);
                RemoveMenu(hSysMenu, SC_MAXIMIZE, MF_BYCOMMAND);
            }

            UpdateTheme(hwnd, isDarkModeEnabled, coloreSfondo, coloreTesto, hBrushSfondo, gdiFont, ffSegoeV, btn);
            logoCreation(resourceNumber);

            InvalidateRect(hwnd, NULL, TRUE);   
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            HDC memDC = CreateCompatibleDC(hdc);
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            int width = clientRect.right;
            int height = clientRect.bottom;
            HBITMAP memBitmap = CreateCompatibleBitmap(hdc, width, height);
            HGDIOBJ oldBitmap = SelectObject(memDC, memBitmap);

            Graphics graphics(memDC);
            graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
            graphics.SetSmoothingMode(SmoothingModeHighQuality);
            graphics.SetPixelOffsetMode(PixelOffsetModeHalf);
            graphics.SetTextRenderingHint(isDarkModeEnabled ? 
                TextRenderingHintClearTypeGridFit : TextRenderingHintAntiAliasGridFit);

            if (compCheck < 22000) {
                Color bkg; bkg.SetFromCOLORREF(coloreSfondo);
                graphics.Clear(bkg);
            } else {
                graphics.Clear(Color(0, 0, 0, 0));
            }

            if (imgLogo && imgLogo->GetLastStatus() == Ok) {
                float aspectHeight = (g_realW / (float)imgLogo->GetWidth()) * (float)imgLogo->GetHeight();
                graphics.DrawImage(imgLogo, RectF(g_startX, g_startY, g_realW, aspectHeight));
            } 
            else if (cachedLogo && cachedLogo->GetLastStatus() == Ok) {
                float aspectHeight = (g_realW / (float)cachedLogo->GetWidth()) * (float)cachedLogo->GetHeight();
                graphics.DrawImage(cachedLogo, RectF(g_startX, g_startY, g_realW, aspectHeight), 0.0f, 0.0f, (REAL)cachedLogo->GetWidth(), (REAL)cachedLogo->GetHeight(), UnitPixel, pImgAttr);
            }

            graphics.DrawLine(pLinePen, g_startX, g_lineY, (REAL)width - g_startX, g_lineY);

            graphics.DrawString(string_4(), -1, gdiFont, g_layoutRectBody, NULL, pTextBrush);

            SolidBrush* currentFill = pBtnFillNormal;
            if (btn.pressed) currentFill = pBtnFillPressed;
            else if (btn.hover) currentFill = pBtnFillHover;

            graphics.FillPath(currentFill, buttonPath);
            graphics.DrawPath(pBtnBorderPenStatic, buttonPath);

            RectF rect((REAL)btn.rect.left, (REAL)btn.rect.top, (REAL)(btn.rect.right - btn.rect.left), (REAL)(btn.rect.bottom - btn.rect.top));

            graphics.DrawString(L"OK", -1, gdiFont, rect, g_pStringFormatCenter, pBtnTextBrushStatic);

            BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);

            SelectObject(memDC, oldBitmap);
            DeleteObject(memBitmap);
            DeleteDC(memDC);
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

        case WM_SETCURSOR: {
            SetCursor(LoadCursor(NULL, IDC_ARROW));
            return TRUE; 
        }
        
        case WM_MOUSEMOVE: {
            POINT pt = { LOWORD(lp), HIWORD(lp) };

            bool old = btn.hover;
            btn.hover = PtInRect(&btn.rect, pt);

            if (old != btn.hover)
                InvalidateRect(hwnd, &btn.rect, FALSE);

            TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 };
            TrackMouseEvent(&tme);

            return 0;
        }

        case WM_MOUSELEAVE: {
            btn.hover = false;
            InvalidateRect(hwnd, &btn.rect, FALSE);
            return 0;
        }

        case WM_LBUTTONDOWN: {
            POINT pt = { LOWORD(lp), HIWORD(lp) };

            if (PtInRect(&btn.rect, pt)) {
                btn.pressed = true;
                SetCapture(hwnd);
                InvalidateRect(hwnd, &btn.rect, FALSE);
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            POINT pt = { LOWORD(lp), HIWORD(lp) };

            if (btn.pressed) {
                btn.pressed = false;
                ReleaseCapture();

                if (PtInRect(&btn.rect, pt))
                    SendMessage(hwnd, WM_CLOSE, 0, 0);

                InvalidateRect(hwnd, &btn.rect, FALSE);
            }
            return 0;
        }

        case WM_KEYDOWN: {
            if (wp == VK_RETURN || wp == VK_ESCAPE) {
                SendMessage(hwnd, WM_CLOSE, 0, 0);
            }
            return 0;
        }

        case WM_SETTINGCHANGE: {
            UpdateTheme(hwnd, isDarkModeEnabled, coloreSfondo, coloreTesto, hBrushSfondo, gdiFont, ffSegoeV, btn);
            InvalidateRect(hwnd, NULL, TRUE);
            UpdateWindow(hwnd);
            return 0;
        }

        case WM_DPICHANGED: {
            LPRECT lprcNewScale = (LPRECT)lp;
            UINT newDpi = LOWORD(wp);
            UpdateTheme(hwnd, isDarkModeEnabled, coloreSfondo, coloreTesto, hBrushSfondo, gdiFont, ffSegoeV, btn);
            logoCreation(resourceNumber);

            SetWindowPos(hwnd, NULL, lprcNewScale->left, lprcNewScale->top, lprcNewScale->right - lprcNewScale->left, lprcNewScale->bottom - lprcNewScale->top, SWP_NOZORDER | SWP_NOACTIVATE);

            if (gdiFont) { delete gdiFont; gdiFont = nullptr; }
            if (ffSegoeV) { delete ffSegoeV; ffSegoeV = nullptr; }

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
            if (buttonPath) delete buttonPath;
            if (g_pStringFormatCenter) delete g_pStringFormatCenter;
            if (pBtnFillNormal) delete pBtnFillNormal;
            if (pBtnFillHover) delete pBtnFillHover;
            if (pBtnFillPressed) delete pBtnFillPressed;
            if (pBtnTextBrushStatic) delete pBtnTextBrushStatic;
            if (pBtnBorderPenStatic) delete pBtnBorderPenStatic;
            
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
    
    buildGet(build, 64);
    compCheck = _wtoi(build);

    if (compCheck < 9200) {
        MessageBoxW(NULL, string_1(), string_2(), MB_OK | MB_ICONWARNING | MB_SETFOREGROUND);
        return 0;
    }

    int clientW = ScaleValue(400, currentDpi);
    int clientH = ScaleValue(390, currentDpi);

    RECT rc = { 0, 0, clientW, clientH };
    AdjustWindowRectEx(&rc, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, FALSE, 0);

    int windowW = rc.right - rc.left;
    int windowH = rc.bottom - rc.top;

    OSGet(OSName, 128);
    ntGet(NT, 64);
    commercialVersionGet(commercialVersion, 64);
    userGet(user, 128);
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

    HWND hwnd = CreateWindowExW(WS_EX_DLGMODALFRAME, L"BetterWinver", string_3(), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, windowW, windowH, NULL, NULL, hInst, NULL);

    windowTheme(hwnd);
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
