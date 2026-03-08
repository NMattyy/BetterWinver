//BetterWinver 1.2.0
#include <windows.h>
#include <dwmapi.h>
#include <string>
#include <gdiplus.h>
#include <wingdi.h>
#include <shlwapi.h>

#include "infoGet.h"
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

IStream* CreateStreamOnResource(HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType) {
    HRSRC hRsrc = FindResource(hModule, lpName, lpType);
    if (!hRsrc) return NULL;
    
    DWORD dwSize = SizeofResource(hModule, hRsrc);
    HGLOBAL hGlobal = LoadResource(hModule, hRsrc);
    if (!hGlobal) return NULL;

    void* pBuffer = LockResource(hGlobal);
    
    return SHCreateMemStream((BYTE*)pBuffer, dwSize);
}

void bitmapCache(HBITMAP hBmpRes, int bmpWidth, int bmpHeight, bool isDarkModeEnabled) {
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
            int diff = max(r, max(g, b)) - min(r, min(g, b));

            if (diff > 25) { 
                float alphaFactor = min(1.0f, (float)diff / 25.0f);
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

LRESULT CALLBACK windowManager(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    static COLORREF coloreSfondo = RGB(255, 255, 255);
    static COLORREF coloreTesto = RGB(0, 0, 0);
    static HBRUSH hBrushSfondo = NULL;

    static bool mouseHiglight = false;

    static Image* imgLogo = NULL;
    static HBITMAP hBmpRes = NULL;
    static int bmpWidth = 0, bmpHeight = 0;

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

            HMODULE hLib = GetModuleHandle("basebrd.dll");
            if (!hLib) hLib = LoadLibraryEx("C:\\Windows\\Branding\\Basebrd\\basebrd.dll", NULL, LOAD_LIBRARY_AS_DATAFILE);

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
            return 0;
        }

        case WM_CTLCOLORSTATIC: {
            HDC hdcStatic = (HDC)wp;
            SetBkMode(hdcStatic, TRANSPARENT);
            SetTextColor(hdcStatic, coloreTesto);
            return (LRESULT)hBrushSfondo;
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

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            Graphics graphics(hdc);
            
            Color backColor;
            backColor.SetFromCOLORREF(coloreSfondo);
            graphics.Clear(backColor);

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
            
            EndPaint(hwnd, &ps);
            return 0;
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

                if (isDarkModeEnabled) {
                    if (pDIS->itemState & ODS_SELECTED) btnColor = Color(80, 80, 80);      
                    else if (isHovered) btnColor = Color(70, 70, 70);                    
                    else btnColor = Color(60, 60, 60);
                    
                    borderColor = Color(100, 100, 100);
                    textColor = Color(255, 255, 255);
                } else {
                    if (pDIS->itemState & ODS_SELECTED) btnColor = Color(200, 200, 200);
                    else if (isHovered) btnColor = Color(215, 215, 215);
                    else btnColor = Color(230, 230, 230);
                    
                    borderColor = Color(180, 180, 180);
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
                Font btnFont(L"Segoe UI", 9);
                
                RectF rectText(0, 0, (REAL)pDIS->rcItem.right, (REAL)pDIS->rcItem.bottom);
                graphics.DrawString(L"OK", -1, &btnFont, rectText, &format, &textBrush);

                return TRUE;
            }
            break;
        }

        case WM_ERASEBKGND: {
            return 1; 
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

        case WM_KEYDOWN: {
            if (wp == VK_RETURN || wp == VK_ESCAPE) {
                SendMessage(hwnd, WM_CLOSE, 0, 0);
            }
            return 0;
        }

        case WM_COMMAND: {
            if (LOWORD(wp) == 1) {
                SendMessage(hwnd, WM_CLOSE, 0, 0);
            }
            break;
        }

        case WM_DESTROY: {
            if (hBrushSfondo) DeleteObject(hBrushSfondo);
            if (imgLogo) delete imgLogo;
            if (cachedLogo) delete cachedLogo;
            if (hBmpRes) DeleteObject(hBmpRes);
            if (hFont) DeleteObject(hFont);
            PostQuitMessage(0);
            break;
        }
    
        default:
            return DefWindowProc(hwnd, msg, wp, lp);
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

    hFont = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH, "Segoe UI");
    
    HWND hBody = CreateWindow("STATIC", string_4().c_str(), WS_VISIBLE | WS_CHILD, 30, 115, 420, 150, hwnd, NULL, hInst, NULL);
    SendMessage(hBody, WM_SETFONT, (WPARAM)hFont, TRUE);

    HWND hUser = CreateWindow("STATIC", string_5().c_str(), WS_VISIBLE | WS_CHILD, 30, 330, 420, 80, hwnd, NULL, hInst, NULL);
    SendMessage(hUser, WM_SETFONT, (WPARAM)hFont, TRUE);

    HWND hButton = CreateWindow("BUTTON", "OK", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 370, 440, 80, 25, hwnd, (HMENU)1, hInst, NULL);
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
