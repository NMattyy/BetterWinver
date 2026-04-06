//BetterWinver 1.8.0
#ifndef HEADERS_H
#define HEADERS_H

#include <windows.h>
#include <dwmapi.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <winreg.h>
#include <lmcons.h>
#include <strsafe.h>

extern ID2D1Factory* pD2DFactory;
extern IDWriteFactory* pDWriteFactory;
extern IWICImagingFactory* pWICFactory;

extern ID2D1HwndRenderTarget* pRenderTarget;
extern ID2D1HwndRenderTarget* pAboutRenderTarget;

extern ID2D1SolidColorBrush* pTextBrush;
extern ID2D1SolidColorBrush* pLinePenD2D;
extern ID2D1SolidColorBrush* pBtnBrush;
extern ID2D1SolidColorBrush* pAboutTextBrush;
extern ID2D1SolidColorBrush* pAboutLineBrush;

extern ID2D1Bitmap* pBitmapLogo;

extern IDWriteTextFormat* pTextFormatBody;
extern IDWriteTextFormat* pTextFormatAbout;

struct CustomButton {
    RECT rect;
    bool hover;
    bool pressed;
};
extern CustomButton btn;

extern int argc;
extern LPWSTR* argv;

extern wchar_t NT[64];
extern wchar_t build[64];
extern wchar_t OSName[128];
extern wchar_t commercialVersion[64];
extern wchar_t user[128];

extern int compCheck;
extern bool isDarkModeEnabled;

template <class T> void SafeRelease(T **ppT) {
    if (*ppT) {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

//Windows
LRESULT CALLBACK windowManager(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow);
LRESULT CALLBACK WindowManagerAbout(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
void WinAbout(HWND hParent, HINSTANCE hInst);

//Info Get
inline void ntGet(wchar_t* out, DWORD size);
inline void buildGet(wchar_t* out, DWORD size);
inline void OSGet(wchar_t* out, DWORD size);
inline void commercialVersionGet(wchar_t* out, DWORD size);
inline void userGet(wchar_t* out, DWORD size);

//User Settings
inline UINT GetSystemDPI();
inline UINT GetDpiForWindow(HWND hwnd);
inline void DarkModeCheck();

//Resource Handler
inline HRESULT CreateDeviceResources(HWND hwnd);
inline HRESULT CreateDeviceResourcesAboutWindow(HWND hwnd);
inline void DiscardDeviceResources();
inline void DiscardDeviceResourcesAboutWindow();
inline void logoCreation(HWND hwnd);
inline HRESULT CreateTextFormats(HWND hwnd);
inline HRESULT LoadResourceBitmap(HWND hwnd);

//Window Composition
inline void windowTheme(HWND hwnd);
inline void UpdateButtonState(HWND hwnd, LPARAM lp, bool isDown = false);
inline void clearBackground(ID2D1HwndRenderTarget* pWindowRenderTarget);

//Utility
inline void GetRegString(HKEY hRoot, LPCWSTR subKey, LPCWSTR valueName, wchar_t* outBuffer, DWORD bufferSize);
inline int ScaleValue(int value, UINT dpi);
inline float ScaleValueF(float value, UINT dpi);
inline LPCWSTR GetResString(UINT id);

#include "infoGet.h"
#include "userSettings.h"
#include "resourceHandler.h"
#include "windowComposition.h"
#include "utility.h"

#endif