//BetterWinver 1.8.0

inline HRESULT CreateDeviceResources(HWND hwnd) {
    HRESULT hr = S_OK;
    UINT dpi = GetDpiForWindow(hwnd);

    if (!pRenderTarget) {
        RECT rc;
        GetClientRect(hwnd, &rc);
        D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_HARDWARE, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.0f, 96.0f);

        D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps = D2D1::HwndRenderTargetProperties(hwnd, size);
        hwndProps.presentOptions = D2D1_PRESENT_OPTIONS_RETAIN_CONTENTS;

        hr = pD2DFactory->CreateHwndRenderTarget(props, hwndProps, &pRenderTarget);
        
        if (SUCCEEDED(hr) && !pTextBrush) {
            (pRenderTarget)->CreateSolidColorBrush(D2D1::ColorF(0,0,0), &pTextBrush);
            (pRenderTarget)->CreateSolidColorBrush(D2D1::ColorF(0,0,0), &pLinePenD2D);
            (pRenderTarget)->CreateSolidColorBrush(D2D1::ColorF(0,0,0,0), &pBtnBrush);
        }
    }

    if (SUCCEEDED(hr) && pTextBrush && pLinePenD2D) {
        D2D1_COLOR_F textColor = isDarkModeEnabled ? D2D1::ColorF(D2D1::ColorF::White) : D2D1::ColorF(D2D1::ColorF::Black);
        D2D1_COLOR_F lineColor = isDarkModeEnabled ? D2D1::ColorF(0.3f, 0.3f, 0.3f) : D2D1::ColorF(0.8f, 0.8f, 0.8f);
        
        pTextBrush->SetColor(textColor);
        pLinePenD2D->SetColor(lineColor);

        logoCreation(hwnd);
        hr = CreateTextFormats(hwnd);
        LoadResourceBitmap(hwnd);
    }
    
    RECT rc;
    GetClientRect(hwnd, &rc);
    float margin = ScaleValueF(30.0f, dpi);
    int btnW = ScaleValue(75, dpi);
    int btnH = ScaleValue(24, dpi);
    
    btn.rect.right = (long)(rc.right - margin);
    btn.rect.left = (long)(btn.rect.right - btnW);
    btn.rect.bottom = (long)(rc.bottom - ScaleValue(20, dpi));
    btn.rect.top = (long)(btn.rect.bottom - btnH);

    return hr;
}

inline HRESULT CreateDeviceResourcesAboutWindow(HWND hwnd) {
    HRESULT hr = S_OK;

    if (!pAboutRenderTarget) {
        RECT rc;
        GetClientRect(hwnd, &rc);
        D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_HARDWARE, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.0f, 96.0f);

        D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps = D2D1::HwndRenderTargetProperties(hwnd, size);
        
        hr = pD2DFactory->CreateHwndRenderTarget(props, hwndProps, &pAboutRenderTarget);

        if (SUCCEEDED(hr)) {
            pAboutRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0,0,0), &pAboutTextBrush);
            pAboutRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0,0,0), &pAboutLineBrush);
        }
    }

    if (SUCCEEDED(hr)) {
        D2D1_COLOR_F textColor = isDarkModeEnabled ? D2D1::ColorF(D2D1::ColorF::White) : D2D1::ColorF(D2D1::ColorF::Black);
        D2D1_COLOR_F lineColor = isDarkModeEnabled ? D2D1::ColorF(0.4f, 0.4f, 0.4f) : D2D1::ColorF(0.8f, 0.8f, 0.8f);
        
        pAboutTextBrush->SetColor(textColor);
        pAboutLineBrush->SetColor(lineColor);

        if (!pTextFormatAbout) {
            UINT dpi = GetDpiForWindow(hwnd);
            pDWriteFactory->CreateTextFormat(
                L"Segoe UI Variable Text", NULL, DWRITE_FONT_WEIGHT_NORMAL,
                DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
                ScaleValueF(12.0f, dpi), L"", &pTextFormatAbout
            );
        }
    }
    return hr;
}

inline void DiscardDeviceResources() {
    SafeRelease(&pRenderTarget);
    SafeRelease(&pTextBrush);
    SafeRelease(&pLinePenD2D);
    SafeRelease(&pBtnBrush);
    SafeRelease(&pBitmapLogo); 
    SafeRelease(&pTextFormatBody);
}

inline void DiscardDeviceResourcesAboutWindow() {
    SafeRelease(&pAboutRenderTarget);
    SafeRelease(&pAboutTextBrush);
    SafeRelease(&pAboutLineBrush);
}

inline void logoCreation(HWND hwnd) {
    if (pBitmapLogo || !pRenderTarget) return;

    wchar_t systemPath[MAX_PATH], dllPath[MAX_PATH];
    if (GetSystemDirectoryW(systemPath, MAX_PATH) == 0) return;
    StringCchPrintfW(dllPath, ARRAYSIZE(dllPath), L"%s\\..\\Branding\\Basebrd\\basebrd.dll", systemPath);

    HMODULE hLib = LoadLibraryExW(dllPath, NULL, LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    if (!hLib) return;

    IWICBitmapSource* pFinalSource = nullptr;

    if (compCheck >= 22000) {
        HRSRC hRsrc = FindResourceW(hLib, MAKEINTRESOURCEW(2123), L"IMAGE");
        if (hRsrc) {
            HGLOBAL hResData = LoadResource(hLib, hRsrc);
            void* pBuffer = LockResource(hResData);
            DWORD dwSize = SizeofResource(hLib, hRsrc);

            IWICStream* pStream = nullptr;
            if (SUCCEEDED(pWICFactory->CreateStream(&pStream))) {
                pStream->InitializeFromMemory((BYTE*)pBuffer, dwSize);
                IWICBitmapDecoder* pDecoder = nullptr;
                if (SUCCEEDED(pWICFactory->CreateDecoderFromStream(pStream, NULL, WICDecodeMetadataCacheOnDemand, &pDecoder))) {
                    IWICBitmapFrameDecode* pFrame = nullptr;
                    if (SUCCEEDED(pDecoder->GetFrame(0, &pFrame))) {
                        IWICFormatConverter* pConverter = nullptr;
                        if (SUCCEEDED(pWICFactory->CreateFormatConverter(&pConverter))) {
                            pConverter->Initialize(pFrame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);
                            pFinalSource = pConverter;
                        }
                        SafeRelease(&pFrame);
                    }
                    SafeRelease(&pDecoder);
                }
                SafeRelease(&pStream);
            }
        }
    } else {
        HBITMAP hBmp = (HBITMAP)LoadImageW(hLib, MAKEINTRESOURCEW(2123), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
        if (hBmp) {
            IWICBitmap* pWICBitmap = nullptr;
            if (SUCCEEDED(pWICFactory->CreateBitmapFromHBITMAP(hBmp, NULL, WICBitmapAlphaChannelOption::WICBitmapUseAlpha, &pWICBitmap))) {
                IWICFormatConverter* pConverter = nullptr;
                if (SUCCEEDED(pWICFactory->CreateFormatConverter(&pConverter))) {
                    pConverter->Initialize(pWICBitmap, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);
                    pFinalSource = pConverter;
                }
                SafeRelease(&pWICBitmap);
            }
            DeleteObject(hBmp);
        }
    }

    if (pFinalSource) {
        float dpiX, dpiY;
        pRenderTarget->GetDpi(&dpiX, &dpiY);

        D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(pRenderTarget->GetPixelFormat(), dpiX, dpiY);

        pRenderTarget->CreateBitmapFromWicBitmap(pFinalSource, &props, &pBitmapLogo);
        SafeRelease(&pFinalSource);
    }

    FreeLibrary(hLib);
}

inline HRESULT CreateTextFormats(HWND hwnd) {
    if (pTextFormatBody) return S_OK;

    UINT dpi = GetDpiForWindow(hwnd);
    float fontSize = ScaleValueF(12.0f, dpi);

    HRESULT hr = pDWriteFactory->CreateTextFormat(L"Segoe UI Variable Text", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"",  &pTextFormatBody);

    if (FAILED(hr)) {
        hr = pDWriteFactory->CreateTextFormat(L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"", &pTextFormatBody);
    }
    return hr;
}

inline HRESULT LoadResourceBitmap(HWND hwnd) {
    if (pBitmapLogo) return S_OK;
    HRESULT hr = S_FALSE;

    HICON hIcon = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(101), IMAGE_ICON, 128, 128, LR_DEFAULTCOLOR);
    
    if (hIcon) {
        IWICBitmap* pWICBitmap = NULL;
        hr = pWICFactory->CreateBitmapFromHICON(hIcon, &pWICBitmap);
        if (SUCCEEDED(hr)) {
            hr = pRenderTarget->CreateBitmapFromWicBitmap(pWICBitmap, NULL, &pBitmapLogo);
        }
        SafeRelease(&pWICBitmap);
        DestroyIcon(hIcon);
    }
    return hr;
}
