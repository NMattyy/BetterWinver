//BetterWinver 1.7.1
#include <windows.h>
#include <dwmapi.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <wincodec.h>

#include "infoGet.hpp"
#include "features.hpp"
#include "translation.hpp"

ID2D1Factory* pD2DFactory = nullptr;
IDWriteFactory* pDWriteFactory = nullptr;
IWICImagingFactory* pWICFactory = nullptr;

ID2D1HwndRenderTarget* pRenderTarget = nullptr;

ID2D1SolidColorBrush* pBrushSfondo = nullptr;
ID2D1SolidColorBrush* pTextBrush = nullptr;
ID2D1SolidColorBrush* pLinePenD2D = nullptr;
ID2D1SolidColorBrush* pBtnBrush = nullptr;

ID2D1Bitmap* pBitmapLogo = nullptr;

IDWriteTextFormat* pTextFormatTitle = nullptr; 
IDWriteTextFormat* pTextFormatBody = nullptr;

wchar_t NT[64];
wchar_t build[64];
wchar_t OSName[128];
wchar_t commercialVersion[64];
wchar_t user[128];
int compCheck;
bool isDarkModeEnabled;

int argc = 0;
LPWSTR* argv = nullptr;

CustomButton btn = {};

LRESULT CALLBACK windowManager(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow);
void AboutWindow(HWND hParent, HINSTANCE hInst);
LRESULT CALLBACK aboutWindowManager(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

LRESULT CALLBACK windowManager(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE: {
            HMENU hSysMenu = GetSystemMenu(hwnd, FALSE);
            if (hSysMenu) {
                RemoveMenu(hSysMenu, SC_SIZE, MF_BYCOMMAND);
                RemoveMenu(hSysMenu, SC_MAXIMIZE, MF_BYCOMMAND);
            }
            windowTheme(hwnd);
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);

            if (SUCCEEDED(CreateDeviceResources(hwnd))) {
                RECT rc;
                GetClientRect(hwnd, &rc);

                pRenderTarget->BeginDraw();
                pTextFormatBody->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                pTextFormatBody->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

                pRenderTarget->Clear(D2D1::ColorF(0, 0, 0, 0.0f));

                clearBackground(pRenderTarget);

                UINT dpi = GetDpiForWindow(hwnd);
                float margin = ScaleValueF(30.0f, dpi);

                pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

                //Logo
                if (pBitmapLogo) {
                    D2D1_SIZE_F size = pBitmapLogo->GetSize();
                    int destW = (int)((float)rc.right - (margin * 2));
                    int destH = (int)(( (float)destW / size.width ) * size.height);
                    D2D1_RECT_F logoRect = D2D1::RectF((float)margin, (float)ScaleValue(5, dpi), (float)(margin + destW), (float)(ScaleValue(5, dpi) + destH));
                    pRenderTarget->DrawBitmap(pBitmapLogo, logoRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);
                }

                pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

                //Line
                float lineY = ScaleValueF(75.0f, dpi);
                pRenderTarget->DrawLine(D2D1::Point2F(margin, lineY), D2D1::Point2F((float)rc.right - margin, lineY), pLinePenD2D, 1.0f);

                //Text
                D2D1_RECT_F layoutRect = D2D1::RectF(margin, ScaleValueF(90.0f, dpi), (float)rc.right - margin, (float)rc.bottom - ScaleValueF(70.0f, dpi));
                pRenderTarget->DrawText(string_6(), (UINT32)wcslen(string_6()), pTextFormatBody, layoutRect, pTextBrush);

                //OK Button
                D2D1_RECT_F bRect = D2D1::RectF((float)btn.rect.left, (float)btn.rect.top, (float)btn.rect.right, (float)btn.rect.bottom);
                D2D1_COLOR_F btnCol = isDarkModeEnabled ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.1f) : D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.05f);
                if (btn.pressed) btnCol.a = 0.3f;
                else if (btn.hover) btnCol.a = 0.2f;

                pBtnBrush->SetColor(btnCol);
                pRenderTarget->FillRoundedRectangle(D2D1::RoundedRect(bRect, 4.0f, 4.0f), pBtnBrush);
                pRenderTarget->DrawRoundedRectangle(D2D1::RoundedRect(bRect, 4.0f, 4.0f), pLinePenD2D, 1.0f);
                
                pTextFormatBody->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                pTextFormatBody->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                pRenderTarget->DrawText(L"OK", 2, pTextFormatBody, bRect, pTextBrush);

                if (pRenderTarget->EndDraw() == D2DERR_RECREATE_TARGET) {
                    DiscardDeviceResources();
                }
            }
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_SETCURSOR: {
            SetCursor(LoadCursor(NULL, IDC_ARROW));
            return TRUE; 
        }

        case WM_MOUSEMOVE: {
            TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 };
            TrackMouseEvent(&tme);
            UpdateButtonState(hwnd, lp);
            return 0;
        }

        case WM_LBUTTONUP: { 
            POINT pt = { LOWORD(lp), HIWORD(lp) };
            if (btn.pressed && PtInRect(&btn.rect, pt)) {
                SendMessage(hwnd, WM_CLOSE, 0, 0);
            }

            btn.pressed = false;
            ReleaseCapture();
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

        case WM_MOUSELEAVE: {
            btn.hover = false;
            InvalidateRect(hwnd, &btn.rect, FALSE);
            return 0;
        }

        case WM_KEYDOWN: {
            if (wp == VK_RETURN || wp == VK_ESCAPE) {
                SendMessage(hwnd, WM_CLOSE, 0, 0);
            }
            return 0;
        }

        case WM_ERASEBKGND: {
            return 1;
        }

        case WM_SETTINGCHANGE: {
            DarkModeCheck(); 
            windowTheme(hwnd);

            InvalidateRect(hwnd, NULL, TRUE);
            UpdateWindow(hwnd);
            return 0;
        }
        
        case WM_DPICHANGED: {
            LPRECT lprcNewScale = (LPRECT)lp;

            DiscardDeviceResources();
            
            SafeRelease(&pTextFormatBody); 
            SafeRelease(&pBitmapLogo); 

            DarkModeCheck(); 
            windowTheme(hwnd);

            SetWindowPos(hwnd, NULL, lprcNewScale->left, lprcNewScale->top, lprcNewScale->right - lprcNewScale->left, lprcNewScale->bottom - lprcNewScale->top, SWP_NOZORDER | SWP_NOACTIVATE);

            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }

        case WM_DESTROY: {
            SafeRelease(&pTextFormatBody);
            DiscardDeviceResources();
            
            PostQuitMessage(0);
            break;
        }
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow) {
    argv = CommandLineToArgvW(GetCommandLineW(), &argc);

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

    OSGet(OSName, 128);
    ntGet(NT, 64);
    commercialVersionGet(commercialVersion, 64);
    userGet(user, 128);
    DarkModeCheck();

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) return 0;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);
    if (SUCCEEDED(hr)) {
        hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(&pDWriteFactory)
        );
    }

    if (SUCCEEDED(hr)) {
        hr = CoCreateInstance(
            CLSID_WICImagingFactory,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&pWICFactory)
        );
    }

    if (FAILED(hr)) {
        MessageBoxW(NULL, string_3(), string_4(), MB_OK | MB_ICONWARNING | MB_SETFOREGROUND);
        return 0;
    }

    WNDCLASSW wc = {0};
    wc.lpfnWndProc = windowManager;
    wc.hInstance = hInst;
    wc.hbrBackground = NULL;
    wc.lpszClassName = L"BetterWinver";
    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(WS_EX_DLGMODALFRAME, L"BetterWinver", string_5(), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 100, 100, NULL, NULL, hInst, NULL);

    if (hwnd) {
        windowTheme(hwnd);
        UINT dpi = GetDpiForWindow(hwnd);
        RECT rc = { 0, 0, ScaleValue(400, dpi), ScaleValue(390, dpi) };
        AdjustWindowRectEx(&rc, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, FALSE, WS_EX_DLGMODALFRAME);
        
        SetWindowPos(hwnd, NULL, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        
        if (SUCCEEDED(CreateDeviceResources(hwnd))) {
            ValidateRect(hwnd, NULL); 
        }

        ShowWindow(hwnd, nShow);
        UpdateWindow(hwnd);
    }

    PostMessage(hwnd, WM_NULL, 0, 0);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    SafeRelease(&pD2DFactory);
    SafeRelease(&pDWriteFactory);
    SafeRelease(&pWICFactory);
    SafeRelease(&pBitmapLogo);
    CoUninitialize();

    LocalFree(argv);
    return msg.wParam;
}
