//BetterWinver 1.3.0
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
Bitmap* cachedLogo = nullptr;

string build;
string OSName;
string NT;
string commercialVersion;
string user;
int compCheck;
bool isDarkModeEnabled;

LRESULT CALLBACK windowManager(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
static COLORREF coloreSfondo = RGB(255, 255, 255);
    static COLORREF coloreTesto = RGB(0, 0, 0);
    static HBRUSH hBrushSfondo = NULL;
    static bool mouseHiglight = false;
    static Image* imgLogo = NULL;
    static HBITMAP hBmpRes = NULL;
    static int bmpWidth = 0, bmpHeight = 0;
    static FontFamily* ffSegoeV = nullptr;
    static Font* gdiFont = nullptr;

    int resourceNumber;
    if (compCheck >= 10240) {
        resourceNumber = 2123;
    } else { 
        resourceNumber = 2121;
    }

    switch (msg) {
        case WM_CREATE: {
            DarkModeCheck();
            BOOL useDarkMode = isDarkModeEnabled;
            coloreSfondo = isDarkModeEnabled ? RGB(32, 32, 32) : RGB(255, 255, 255);
            coloreTesto = isDarkModeEnabled ? RGB(255, 255, 255) : RGB(0, 0, 0);
            
            if (hBrushSfondo) DeleteObject(hBrushSfondo);
            hBrushSfondo = CreateSolidBrush(coloreSfondo);
            DwmSetWindowAttribute(hwnd, 20, &useDarkMode, sizeof(useDarkMode));

            char systemPath[MAX_PATH];
            GetSystemDirectory(systemPath, MAX_PATH);
            string dllPath = string(systemPath) + "\\..\\Branding\\Basebrd\\basebrd.dll";

            HMODULE hLib = LoadLibraryEx(dllPath.c_str(), NULL, LOAD_LIBRARY_AS_DATAFILE);

            if (hLib) {
                if (compCheck >= 22000) {
                    IStream* pStream = CreateStreamOnResource(hLib, MAKEINTRESOURCE(resourceNumber), "IMAGE");
                    if (pStream) {
                        imgLogo = new Image(pStream);
                        pStream->Release();
                    }
                } else {
                    hBmpRes = (HBITMAP)LoadImage(hLib, MAKEINTRESOURCE(resourceNumber), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
                    if (hBmpRes) {
                        BITMAP bmp;
                        GetObject(hBmpRes, sizeof(BITMAP), &bmp);
                        bmpWidth = bmp.bmWidth;
                        bmpHeight = bmp.bmHeight;
                        
                        bitmapCache(hBmpRes, bmpWidth, bmpHeight, isDarkModeEnabled);
                    }
                }
            }
            InvalidateRect(hwnd, NULL, TRUE);   
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            Graphics graphics(hdc);
            
            if (compCheck >= 22000) {
                graphics.Clear(Color(0, 0, 0, 0));
            } else {
                Color backColor;
                backColor.SetFromCOLORREF(coloreSfondo);
                graphics.Clear(backColor);
            }

            float aspectWidth = 420.0f;

            if (compCheck >= 22000 && imgLogo && imgLogo->GetLastStatus() == Ok) {
                graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
                float aspectHeight = (aspectWidth / (float)imgLogo->GetWidth()) * (float)imgLogo->GetHeight();
                graphics.DrawImage(imgLogo, RectF(30.0f, 15.0f, aspectWidth, aspectHeight));
            } 
            else if (cachedLogo && cachedLogo->GetLastStatus() == Ok) {
                graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
                graphics.SetSmoothingMode(SmoothingModeAntiAlias);
                graphics.SetPixelOffsetMode(PixelOffsetModeHighQuality);

                float aspectHeight = (aspectWidth / (float)cachedLogo->GetWidth()) * (float)cachedLogo->GetHeight();
                ImageAttributes imgAttr;
                imgAttr.SetWrapMode(WrapModeTileFlipXY);

                graphics.DrawImage(cachedLogo, 
                    RectF(30.0f, 15.0f, aspectWidth, aspectHeight),
                    0, 0, (REAL)cachedLogo->GetWidth(), (REAL)cachedLogo->GetHeight(), 
                    UnitPixel, &imgAttr);
            }

            Graphics graphicsLine(hdc);
            RECT rc; GetClientRect(hwnd, &rc);
            Pen pen(isDarkModeEnabled ? Color(80, 80, 80) : Color(220, 220, 220), 1.0f);
            graphicsLine.DrawLine(&pen, 30.0f, 95.5f, (REAL)rc.right - 30.0f, 95.5f);

            if (isDarkModeEnabled) {
                graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
            } else {
                graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
            }

            if (!ffSegoeV) {
                ffSegoeV = new FontFamily(L"Segoe UI Variable Text");
                if (ffSegoeV->GetLastStatus() != Ok) {
                    delete ffSegoeV;
                    ffSegoeV = new FontFamily(L"Segoe UI");
                }
                gdiFont = new Font(ffSegoeV, 9, FontStyleRegular, UnitPoint);
            }

            SolidBrush textBrush(isDarkModeEnabled ? Color(255, 255, 255) : Color(0, 0, 0));

            RectF layoutRectBody(30.0f, 115.0f, 420.0f, 210.0f); 
            graphics.DrawString(string_4().c_str(), -1, gdiFont, layoutRectBody, NULL, &textBrush);

            RectF layoutRectUser(30.0f, 340.0f, 420.0f, 80.0f);
            graphics.DrawString(string_5().c_str(), -1, gdiFont, layoutRectUser, NULL, &textBrush);

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

                const wchar_t* fontName = L"Segoe UI Variable Display";
                    if (FontFamily(fontName).GetLastStatus() != Ok) {
                        fontName = L"Segoe UI";
                    }

                    FontFamily fontFamily(fontName);
                    Font btnFont(&fontFamily, 9, FontStyleRegular, UnitPoint);
                
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
            DarkModeCheck();
            BOOL useDarkMode = isDarkModeEnabled;
            coloreSfondo = isDarkModeEnabled ? RGB(32, 32, 32) : RGB(255, 255, 255);
            coloreTesto = isDarkModeEnabled ? RGB(255, 255, 255) : RGB(0, 0, 0);
            if (hBrushSfondo) DeleteObject(hBrushSfondo);
            hBrushSfondo = CreateSolidBrush(coloreSfondo);
            DwmSetWindowAttribute(hwnd, 20, &useDarkMode, sizeof(useDarkMode));
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }

        case WM_DESTROY: {
            if (hBrushSfondo) DeleteObject(hBrushSfondo);
            if (hFont) DeleteObject(hFont);
            if (hBmpRes) DeleteObject(hBmpRes);
            if (imgLogo) delete imgLogo;
            if (ffSegoeV) delete ffSegoeV;
            if (gdiFont) delete gdiFont;
            PostQuitMessage(0);
            break;
        }
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow) {
    SetProcessDPIAware();

    build = buildGet();
    compCheck = stoi(build);
    if (compCheck < 9200) {
        MessageBox(NULL, string_1().c_str(), string_2().c_str(), MB_OK | MB_ICONWARNING | MB_SETFOREGROUND);
        return 0;
    }

    OSName = OSGet();
    NT = ntGet();
    commercialVersion = commercialVersionGet();
    user = userGet();
    DarkModeCheck();

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASS wc = {0};
    wc.lpfnWndProc = windowManager;
    wc.hInstance = hInst;
    wc.hbrBackground = NULL;
    wc.lpszClassName = "BetterWinver";
    RegisterClass(&wc);

    HWND hwnd = CreateWindow("BetterWinver", string_3().c_str(), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 480, 520, NULL, NULL, hInst, NULL);

    if (compCheck >= 22000) {
        int backdropType = DWMSBT_MAINWINDOW;
        DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdropType, sizeof(backdropType));
        MARGINS margins = {-1, -1, -1, -1};
        DwmExtendFrameIntoClientArea(hwnd, &margins);
    }

    HWND hButton = CreateWindowW(L"BUTTON", L"OK", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 370, 440, 80, 25, hwnd, (HMENU)1, hInst, NULL);
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
