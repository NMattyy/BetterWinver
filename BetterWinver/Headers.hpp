//Headers 2.2.0
#pragma once
#include <windows.h>
#include <wrl/client.h>
#include <d2d1_3.h>
#include <dwrite.h>
#include <dwmapi.h>
#include <wincodec.h>
#include <strsafe.h>
#include "Placeholders.hpp"
#include "WindowCompositionHelper.hpp"

extern LPWSTR* args;
extern int argc;
extern wchar_t buildString[64];
extern int build;

extern UINT dpi;
extern int width;
extern int height;

extern BOOL darkMode;
extern BOOL trasparency;

extern Microsoft::WRL::ComPtr<ID2D1Factory3> pD2DFactory;
extern Microsoft::WRL::ComPtr<ID2D1DeviceContext> pMainContext;
extern Microsoft::WRL::ComPtr<ID2D1Bitmap1> pWindowsLogo;
extern Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pMLineBrush;
extern Microsoft::WRL::ComPtr<IDWriteFactory> pDWriteFont;
extern Microsoft::WRL::ComPtr<IDWriteTextFormat> pMTextFormat;
extern Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pMBrush;
extern wchar_t bodyText[2048];

extern bool buttonHovered;
extern bool buttonPressed;

//InfoGet
LPCWSTR GetResString(UINT id);
void BuildGet(wchar_t* out, DWORD size);
void ManualLanguageGet();
void NTGet(wchar_t* out, DWORD size);
void OSGet(wchar_t* out, DWORD size);
void VersionGet(wchar_t* out, DWORD size);
void UserGet(wchar_t* out, DWORD size);
void OrganizationGet(wchar_t* out, DWORD size);
void DarkModeCheck();
void TrasparencyCheck();

//WindowComposition
void ApplyAcrylic(HWND hwnd);
void WindowScale(HWND hwnd);
void WindowTheme(HWND hwnd);

HRESULT MainWindowComposition(HWND hwnd);
void clearBackground();
void DrawWindowsLogo();
void DrawLine();
void DrawWindowsText(HWND hwnd);
void DrawButton(HWND hwnd);
void MainWindowDestroy();