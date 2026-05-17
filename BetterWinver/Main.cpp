//BetterWinver 2.1.1
#include "Headers.hpp"

const int MIN_REQUIRED_BUILD = 17763;

LPWSTR* args = nullptr;
wchar_t buildString[64];
int argc = 0;
int build;

UINT dpi;
int width;
int height;

BOOL darkMode;

Microsoft::WRL::ComPtr<ID2D1Factory3> pD2DFactory = nullptr;
Microsoft::WRL::ComPtr<ID2D1DeviceContext> pMainContext = nullptr;
Microsoft::WRL::ComPtr<ID2D1Bitmap1> pWindowsLogo = nullptr;
Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pMLineBrush = nullptr;
Microsoft::WRL::ComPtr<IDWriteFactory> pDWriteFont = nullptr;
Microsoft::WRL::ComPtr<IDWriteTextFormat> pMTextFormat = nullptr;
Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pMBrush = nullptr;
wchar_t bodyText[2048];

bool buttonHovered = false;
bool buttonPressed = false;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE : {
            HMENU hSysMenu = GetSystemMenu(hwnd, FALSE);
            if (hSysMenu) {
                RemoveMenu(hSysMenu, SC_RESTORE, MF_BYCOMMAND);
                RemoveMenu(hSysMenu, SC_SIZE, MF_BYCOMMAND);
                RemoveMenu(hSysMenu, SC_MINIMIZE, MF_BYCOMMAND);
                RemoveMenu(hSysMenu, SC_MAXIMIZE, MF_BYCOMMAND);

                InsertMenuW(hSysMenu, SC_CLOSE, MF_BYCOMMAND | MF_STRING, SC_ABOUT, GetResString(BETTERWINVER_ABOUT));

                LPCWSTR rawTemplate = GetResString(BETTERWINVER_VERSION);
                wchar_t formattedText[256];
                StringCchPrintfW(formattedText, ARRAYSIZE(formattedText), rawTemplate, VERSION_STRING);
                InsertMenuW(hSysMenu, SC_CLOSE, MF_BYCOMMAND | MF_STRING, SC_VERSION, formattedText);

                InsertMenuW(hSysMenu, SC_CLOSE, MF_BYCOMMAND | MF_STRING, SC_CREATOR, GetResString(BETTERWINVER_CREATOR));

                InsertMenuW(hSysMenu, SC_CLOSE, MF_BYCOMMAND | MF_SEPARATOR, 0, NULL);
            }

            DarkModeCheck();
            return 0;
        }

        case WM_SYSCOMMAND : {
            switch (wParam & 0xFFF0) {
                case SC_ABOUT:
                    ShellExecuteW(NULL, L"open", L"https://github.com/NMattyy/BetterWinver", NULL, NULL, SW_SHOWNORMAL);
                    return 0;

                case SC_VERSION:
                    ShellExecuteW(NULL, L"open", L"https://github.com/NMattyy/BetterWinver/releases", NULL, NULL, SW_SHOWNORMAL);
                    return 0;

                case SC_CREATOR:
                    ShellExecuteW(NULL, L"open", L"https://github.com/NMattyy", NULL, NULL, SW_SHOWNORMAL);
                    return 0;
            }

            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }

        case WM_SETCURSOR : {
            SetCursor(LoadCursor(NULL, IDC_ARROW));
            return TRUE;
        }

        case WM_ERASEBKGND : {
            return 1;
        }

        case WM_PAINT : {
            MainWindowComposition(hwnd);

            if (pMainContext) {
                pMainContext->BeginDraw();

                pMainContext->Clear(D2D1::ColorF(0, 0, 0, 0.0f));
                pMainContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

                DrawWindowsLogo();
                DrawLine();
                DrawWindowsText(hwnd);
                DrawButton(hwnd);

                HRESULT hr = pMainContext->EndDraw();
                if (hr == D2DERR_RECREATE_TARGET) {
                    MainWindowDestroy();
                }
            }

            ValidateRect(hwnd, NULL);
            return 0;
        }

        case WM_MOUSEMOVE: {
            if (!pMainContext) return 0;

            float dpi = GetDpiForWindow(hwnd);
            float scale = dpi / 96.0f;

            float mouseX = (float)LOWORD(lParam) / scale;
            float mouseY = (float)HIWORD(lParam) / scale;

            D2D1_SIZE_F rtSize = pMainContext->GetSize();

            float btnW = 75.0f;
            float btnH = 24.0f;
            float margin = 30.0f;

            float btnX = rtSize.width - btnW - margin;
            float btnY = rtSize.height - btnH - margin;

            bool hoveredNow = (mouseX >= btnX && mouseX <= btnX + btnW && mouseY >= btnY && mouseY <= btnY + btnH);

            if (hoveredNow != buttonHovered) {
                buttonHovered = hoveredNow;
                InvalidateRect(hwnd, NULL, FALSE); 

                TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 };
                TrackMouseEvent(&tme);
            }
            return 0;
        }

        case WM_MOUSELEAVE: {
            buttonHovered = false;
            buttonPressed = false;
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }

        case WM_LBUTTONDOWN: {
            if (buttonHovered) {
                buttonPressed = true;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            if (buttonPressed && buttonHovered) {
                SendMessage(hwnd, WM_CLOSE, 0, 0);
            }
            buttonPressed = false;
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }

        case WM_ENTERSIZEMOVE: {
            if (build >= 17763 && build < 21996) {
                ApplyAcrylic(hwnd, 1);
            }
            break;
        }

        case WM_EXITSIZEMOVE: {
            if (build >= 17763 && build < 21996) {
                ApplyAcrylic(hwnd, 4);
            }
            break;
        }

        case WM_SETTINGCHANGE : {
            MainWindowDestroy();
            DarkModeCheck();
            WindowTheme(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }

        case WM_DPICHANGED : {
            MainWindowDestroy();
            WindowScale(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }

        case WM_KEYDOWN : {
            if (wParam == VK_RETURN || wParam == VK_ESCAPE) {
                SendMessage(hwnd, WM_CLOSE, 0, 0);
            }
            return 0;
        }

        case WM_DESTROY : {
            MainWindowDestroy();
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    args = CommandLineToArgvW(GetCommandLineW(), &argc);
    BuildGet(buildString, 64);
    build = _wtoi(buildString);
    ManualLanguageGet();

    if (build < MIN_REQUIRED_BUILD) {
        MessageBoxW(NULL, GetResString(UNSUPPORTED_VERSION), L"BetterWinver", MB_OK | MB_ICONERROR);
        return 0;
    }

    const wchar_t CLASS_NAME[] = L"BetterWinver";

    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = NULL;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    HWND hwnd = CreateWindowExW(WS_EX_DLGMODALFRAME, CLASS_NAME, GetResString(APP_TITLE), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, NULL, NULL, hInstance, NULL);

    if (!hwnd) return 0;

    WindowScale(hwnd);

    if (SUCCEEDED(MainWindowComposition(hwnd))) {
        ValidateRect(hwnd, NULL);
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    WindowTheme(hwnd);

    SendMessage(hwnd, WM_NCACTIVATE, FALSE, 0);
    SendMessage(hwnd, WM_NCACTIVATE, TRUE, 0);

    SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CoUninitialize();
    LocalFree(args);
    return 0;
}