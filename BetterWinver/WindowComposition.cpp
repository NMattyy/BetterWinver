//WindowCompositionHelper 2.1.0
#include "Headers.hpp"

void ApplyAcrylic(HWND hwnd, int setting) {
    HMODULE hUser = GetModuleHandleW(L"user32.dll");
    if (hUser) {
        auto SetWindowCompositionAttribute = (pSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
        if (SetWindowCompositionAttribute) {
            ACCENT_STATE state;
            DWORD color;

            if (setting == 1) {
                state = ACCENT_ENABLE_GRADIENT;
                color = darkMode ? 0xFF202020 : 0xFFF3F3F3;
            }
            else {
                state = ACCENT_ENABLE_ACRYLICBLURBEHIND;
                color = darkMode ? 0xA6202020 : 0xB3F3F3F3;
            }

            ACCENT_POLICY policy = { state, 2, color, 0 };
            WINDOWCOMPOSITIONATTRIBDATA data = { 19, &policy, sizeof(ACCENT_POLICY) };
            SetWindowCompositionAttribute(hwnd, &data);
        }
    }
}

void WindowScale(HWND hwnd) {
    dpi = GetDpiForWindow(hwnd);
    width = MulDiv(400, dpi, 96);
    height = MulDiv(410, dpi, 96);

    RECT rc = { 0, 0, width, height };
    AdjustWindowRectExForDpi(&rc, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, FALSE, WS_EX_DLGMODALFRAME, dpi);
    SetWindowPos(hwnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

void WindowTheme(HWND hwnd) {
    DarkModeCheck();
    HMODULE hUxTheme = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hUxTheme) {
        typedef void (WINAPI* PfnSetPreferredAppMode)(int appMode);
        typedef void (WINAPI* PfnFlushMenuThemes)(void);

        PfnSetPreferredAppMode SetPreferredAppMode =
            (PfnSetPreferredAppMode)GetProcAddress(hUxTheme, MAKEINTRESOURCEA(135));
        PfnFlushMenuThemes FlushMenuThemes =
            (PfnFlushMenuThemes)GetProcAddress(hUxTheme, MAKEINTRESOURCEA(136));

        if (SetPreferredAppMode) {
            SetPreferredAppMode(darkMode ? 1 : 0);
        }

        if (FlushMenuThemes) {
            FlushMenuThemes();
        }

        FreeLibrary(hUxTheme);
    }

    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &darkMode, sizeof(darkMode));

    if (build >= 22000) {
        int backdropType = DWMSBT_MAINWINDOW;
        DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdropType, sizeof(backdropType));

        MARGINS margins = { -1, -1, -1, -1 };
        DwmExtendFrameIntoClientArea(hwnd, &margins);
    } else {
        ApplyAcrylic(hwnd, 4);
    }
}

void LoadWindowsLogo(HWND hwnd) {
    wchar_t systemPath[MAX_PATH], dllPath[MAX_PATH];
    if (GetSystemDirectoryW(systemPath, MAX_PATH) == 0) return;
    StringCchPrintfW(dllPath, ARRAYSIZE(dllPath), L"%s\\..\\Branding\\Basebrd\\basebrd.dll", systemPath);

    HMODULE hBaseBrd = LoadLibraryExW(dllPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
    if (!hBaseBrd) return;

    if (build >= 22000) {
        if (hBaseBrd) {
            HRSRC hRes = FindResourceW(hBaseBrd, MAKEINTRESOURCEW(2123), L"IMAGE");
            if (hRes) {
                HGLOBAL hResData = LoadResource(hBaseBrd, hRes);
                if (hResData) {
                    LPVOID resData = LockResource(hResData);
                    DWORD resSize = SizeofResource(hBaseBrd, hRes);

                    IWICImagingFactory* pWICFactory = nullptr;
                    IWICStream* pStream = nullptr;
                    IWICBitmapDecoder* pDecoder = nullptr;
                    IWICBitmapFrameDecode* pFrame = nullptr;
                    IWICFormatConverter* pConverter = nullptr;

                    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pWICFactory));
                    if (SUCCEEDED(hr)) {
                        pWICFactory->CreateStream(&pStream);
                        pStream->InitializeFromMemory((BYTE*)resData, resSize);
                        pWICFactory->CreateDecoderFromStream(pStream, NULL, WICDecodeMetadataCacheOnLoad, &pDecoder);
                        pDecoder->GetFrame(0, &pFrame);
                        pWICFactory->CreateFormatConverter(&pConverter);
                        pConverter->Initialize(pFrame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut);
                        pMainContext->CreateBitmapFromWicBitmap(pConverter, NULL, &pWindowsLogo);
                    }
                }
            }
        }
    } else {
        HBITMAP hBitmap = (HBITMAP)LoadImageW(hBaseBrd, MAKEINTRESOURCEW(2123), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

        if (hBitmap) {
            IWICImagingFactory* pWICFactory = nullptr;
            IWICBitmap* pWICBitmap = nullptr;
            IWICFormatConverter* pConverter = nullptr;

            HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pWICFactory));
            pWICFactory->CreateBitmapFromHBITMAP(hBitmap, NULL, WICBitmapUseAlpha, &pWICBitmap);

            if (SUCCEEDED(hr)) {
                hr = pWICFactory->CreateFormatConverter(&pConverter);
                pConverter->Initialize(pWICBitmap, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut);
                hr = pMainContext->CreateBitmapFromWicBitmap(pConverter, NULL, &pWindowsLogo);
            }

            DeleteObject(hBitmap);
        }
    }

    FreeLibrary(hBaseBrd);
    return;
}

//MainWindow
HRESULT MainWindowComposition(HWND hwnd) {
    if (pD2DFactory == nullptr) {
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), (void**)&pD2DFactory);
    }

    if (pD2DFactory && pMainContext == nullptr) {
        RECT rc;
        GetClientRect(hwnd, &rc);
        D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));

        ID2D1HwndRenderTarget* pHwndRT = nullptr;

        HRESULT hr = pD2DFactory->CreateHwndRenderTarget(rtProps, D2D1::HwndRenderTargetProperties(hwnd, size), &pHwndRT);

        if (SUCCEEDED(hr) && pHwndRT) {
            pHwndRT->QueryInterface(__uuidof(ID2D1DeviceContext), (void**)&pMainContext);

            IDXGISurface* pSurface = nullptr;
            if (SUCCEEDED(pHwndRT->QueryInterface(__uuidof(IDXGISurface), (void**)&pSurface))) {
                ID2D1Bitmap1* pTargetBitmap = nullptr;
                pMainContext->CreateBitmapFromDxgiSurface(pSurface, nullptr, &pTargetBitmap);
                pMainContext->SetTarget(pTargetBitmap);
            }
            pHwndRT->Release();
        }

        if (pMainContext) {
            float dpi = (float)GetDpiForWindow(hwnd);
            pMainContext->SetDpi(dpi, dpi);

            if (!pWindowsLogo) {
                LoadWindowsLogo(hwnd);
            }

            if (pMLineBrush == nullptr) {
                pMainContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gray, 0.4f), &pMLineBrush);
            }

            if (pDWriteFont == nullptr) {
                DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&pDWriteFont);
            }

            if (pDWriteFont && pMTextFormat == nullptr) {
                pDWriteFont->CreateTextFormat(L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f, L"en-us", &pMTextFormat);

                wchar_t osName[256], version[256], ntVer[64], userName[256];
                OSGet(osName, 256);
                VersionGet(version, 256);
                NTGet(ntVer, 64);
                UserGet(userName, 256);

                LPCWSTR rawTemplate = GetResString(TEXT_BODY);

                StringCchPrintfW(bodyText, ARRAYSIZE(bodyText), rawTemplate, osName, version, ntVer, buildString, L'\u00A9', osName, userName);
            }

            if (pMBrush == nullptr) {
                D2D1::ColorF color = darkMode ? D2D1::ColorF(D2D1::ColorF::White) : D2D1::ColorF(D2D1::ColorF::Black);
                pMainContext->CreateSolidColorBrush(color, &pMBrush);
            }
            return S_OK;
        }
        return E_FAIL;
    }
}

void DrawWindowsLogo() {
    if (pWindowsLogo == nullptr) {
        return;
    }

    D2D1_SIZE_F rtSize = pMainContext->GetSize();
    D2D1_SIZE_F bmpSize = pWindowsLogo->GetSize();

    float scale = 0.55f; 
    float scaledWidth = bmpSize.width * scale;
    float scaledHeight = bmpSize.height * scale;

    float x = (float)(int)((rtSize.width - scaledWidth) / 2.0f);
    float y = 10.0f;

    D2D1_RECT_F rect = D2D1::RectF(x, y, x + scaledWidth, y + scaledHeight);

    pMainContext->DrawBitmap(pWindowsLogo.Get(), rect, 1.0f, D2D1_INTERPOLATION_MODE_MULTI_SAMPLE_LINEAR);
}

void DrawLine() {
    if (pMainContext == nullptr || pMBrush == nullptr) return;

    D2D1_SIZE_F rtSize = pMainContext->GetSize();

    float lineY = 85.0f;

    D2D1_POINT_2F startPoint = D2D1::Point2F(30.0f, lineY);
    D2D1_POINT_2F endPoint = D2D1::Point2F(rtSize.width - 30.0f, lineY);

    pMainContext->DrawLine(startPoint, endPoint, pMLineBrush.Get(), 0.8f);
}

void DrawWindowsText(HWND hwnd) {
    if (!pMainContext || !pMTextFormat || !pMBrush) return;

    D2D1_SIZE_F rtSize = pMainContext->GetSize();
    float lineY = 100.0f;
    float startPoint = 30.0f;
    float endPoint = rtSize.width - 30.0f;

    D2D1_RECT_F textRect = D2D1::RectF(startPoint, lineY, endPoint, rtSize.height);

    pMainContext->DrawText(bodyText, (UINT32)wcslen(bodyText), pMTextFormat.Get(), textRect, pMBrush.Get());
}

void DrawButton(HWND hwnd) {
    if (!pMainContext || !pMBrush || !pMTextFormat) return;

    D2D1_SIZE_F rtSize = pMainContext->GetSize();

    float width = 75.0f;
    float height = 24.0f;
    float margin = 30.0f;

    float x = rtSize.width - width - margin;
    float y = rtSize.height - height - margin;
    D2D1_RECT_F rect = D2D1::RectF(x, y, x + 75.0f, y + 24.0f);
    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(rect, 4.0f, 4.0f);

    D2D1_COLOR_F bgColor = darkMode ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.1f) : D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.05f);
    if (buttonPressed) bgColor.a = 0.3f;
    else if (buttonHovered) bgColor.a = 0.2f;

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pBtnBrush;
    pMainContext->CreateSolidColorBrush(bgColor, &pBtnBrush);
    
    pMainContext->FillRoundedRectangle(roundedRect, pBtnBrush.Get());
    pMainContext->DrawRoundedRectangle(roundedRect, pMLineBrush.Get(), 1.0f);

    pMTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    pMTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    pMainContext->DrawText(L"OK", 2, pMTextFormat.Get(), rect, pMBrush.Get());

    pMTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    pMTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
}

void MainWindowDestroy() {
    pMBrush.Reset();
    pWindowsLogo.Reset();
    pMLineBrush.Reset();
    pDWriteFont.Reset();
    pMTextFormat.Reset();
    pMainContext.Reset();
}