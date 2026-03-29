//BetterWinver 1.6.0
#ifndef FEATURES_H
#define FEATURES_H

#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <algorithm>

#include "infoGet.h"

extern int compCheck;
extern bool isDarkModeEnabled;
extern ID2D1Factory* pD2DFactory;
extern ID2D1DCRenderTarget* pRenderTarget;

extern ID2D1SolidColorBrush* pTextBrush;
extern ID2D1SolidColorBrush* pLinePenD2D;
extern ID2D1SolidColorBrush* pBtnBrush;

extern IDWriteFactory* pDWriteFactory;
extern IDWriteTextFormat* pTextFormatBody;
extern ID2D1Bitmap* pBitmapLogo;
extern IWICImagingFactory* pWICFactory;

struct CustomButton {
    RECT rect;
    bool hover;
    bool pressed;
};
extern CustomButton btn;

inline HRESULT CreateDeviceResources(HWND hwnd);
inline void DiscardDeviceResources();
inline void windowTheme(HWND hwnd);
inline void logoCreation(HWND hwnd, int resourceNumber);
inline HRESULT CreateTextFormats(HWND hwnd);
inline HRESULT LoadResourceBitmap(HWND hwnd);
inline void UpdateButtonState(HWND hwnd, LPARAM lp, bool isDown);

inline HRESULT CreateDeviceResources(HWND hwnd) {
    HRESULT hr = S_OK;
    if (!pRenderTarget) {
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT, 
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 
            0, 0, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE, D2D1_FEATURE_LEVEL_DEFAULT);

        hr = pD2DFactory->CreateDCRenderTarget(&props, &pRenderTarget);
        
        if (SUCCEEDED(hr)) {
            pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0,0,0), &pTextBrush);
            pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0,0,0), &pLinePenD2D);
            pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0,0,0,0), &pBtnBrush);
        }
    }

    if (SUCCEEDED(hr) && pTextBrush && pLinePenD2D) {
        D2D1_COLOR_F textColor = isDarkModeEnabled ? D2D1::ColorF(D2D1::ColorF::White) : D2D1::ColorF(D2D1::ColorF::Black);
        D2D1_COLOR_F lineColor = isDarkModeEnabled ? D2D1::ColorF(0.3f, 0.3f, 0.3f) : D2D1::ColorF(0.8f, 0.8f, 0.8f);
        
        pTextBrush->SetColor(textColor);
        pLinePenD2D->SetColor(lineColor);
        
        hr = CreateTextFormats(hwnd);
        LoadResourceBitmap(hwnd);
    }
    
    RECT rc;
    GetClientRect(hwnd, &rc);
    UINT dpi = GetDpiForWindow(hwnd);
    float margin = ScaleValueF(30.0f, dpi);
    int btnW = ScaleValue(75, dpi);
    int btnH = ScaleValue(24, dpi);
    
    btn.rect.right = (long)(rc.right - margin);
    btn.rect.left = (long)(btn.rect.right - btnW);
    btn.rect.bottom = (long)(rc.bottom - ScaleValue(20, dpi));
    btn.rect.top = (long)(btn.rect.bottom - btnH);

    return hr;
}

inline void DiscardDeviceResources() {
    SafeRelease(&pRenderTarget);
    SafeRelease(&pTextBrush);
    SafeRelease(&pLinePenD2D);
    SafeRelease(&pBtnBrush);
}

inline void windowTheme(HWND hwnd) {
    if (compCheck >= 22000) {
        BOOL dark = isDarkModeEnabled;
        DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));

        int backdropType = DWMSBT_MAINWINDOW; 
        DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdropType, sizeof(backdropType));
        
        MARGINS margins = {-1, -1, -1, -1};
        DwmExtendFrameIntoClientArea(hwnd, &margins);
    }

    DiscardDeviceResources();
    RECT rc;
    GetClientRect(hwnd, &rc);
    UINT dpi = GetDpiForWindow(hwnd);

    float margin = ScaleValueF(30.0f, dpi);
    int btnW = ScaleValue(75, dpi);
    int btnH = ScaleValue(24, dpi);

    btn.rect.right = (long)(rc.right - margin);
    btn.rect.left = (long)(btn.rect.right - btnW);
    btn.rect.bottom = (long)(rc.bottom - ScaleValue(20, dpi));
    btn.rect.top = (long)(btn.rect.bottom - btnH);

    SafeRelease(&pTextFormatBody);
    CreateTextFormats(hwnd); 
    SafeRelease(&pBitmapLogo);
}

inline void logoCreation(HWND hwnd, int resourceNumber) {
    SafeRelease(&pBitmapLogo);

    wchar_t systemPath[MAX_PATH], dllPath[MAX_PATH];
    GetSystemDirectoryW(systemPath, MAX_PATH);
    wsprintfW(dllPath, L"%s\\..\\Branding\\Basebrd\\basebrd.dll", systemPath);

    // Carichiamo la DLL
    HMODULE hLib = LoadLibraryExW(dllPath, NULL, LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    if (!hLib) return;

    IWICBitmapSource* pFinalSource = nullptr;

    if (compCheck >= 22000) {
        HRSRC hRsrc = FindResourceW(hLib, MAKEINTRESOURCEW(resourceNumber), L"IMAGE");
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
        HBITMAP hBmp = (HBITMAP)LoadImageW(hLib, MAKEINTRESOURCEW(resourceNumber), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
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
        pRenderTarget->CreateBitmapFromWicBitmap(pFinalSource, NULL, &pBitmapLogo);
        SafeRelease(&pFinalSource);
    }

    FreeLibrary(hLib);
}

inline HRESULT CreateTextFormats(HWND hwnd) {
    if (pTextFormatBody) return S_OK;

    UINT dpi = GetDpiForWindow(hwnd);
    float fontSize = ScaleValueF(12.0f, dpi);

    HRESULT hr = pDWriteFactory->CreateTextFormat(
        L"Segoe UI Variable Text",
        NULL,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        fontSize,
        L"", 
        &pTextFormatBody
    );

    if (FAILED(hr)) {
        hr = pDWriteFactory->CreateTextFormat(
            L"Segoe UI",
            NULL,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            fontSize,
            L"",
            &pTextFormatBody
        );
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

inline void UpdateButtonState(HWND hwnd, LPARAM lp, bool isDown = false) {
    POINT pt = { LOWORD(lp), HIWORD(lp) };
    bool wasHover = btn.hover;
    btn.hover = PtInRect(&btn.rect, pt);
    
    if (isDown && btn.hover) btn.pressed = true;
    if (!isDown) btn.pressed = false;

    if (wasHover != btn.hover || isDown) {
        InvalidateRect(hwnd, &btn.rect, FALSE);
    }
}
#endif
