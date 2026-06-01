//Headers 2.3.1
#pragma once
#pragma comment (lib, "dwmapi.lib")
#pragma comment (lib, "UxTheme.lib")
#pragma comment (lib, "d2d1.lib")
#pragma comment (lib, "windowscodecs.lib")
#pragma comment (lib, "dwrite.lib")

#include <windows.h>
#include <wrl/client.h>
#include <dwmapi.h>
#include <d2d1_3.h>
#include <wincodec.h>
#include <strsafe.h>
#include <dwrite.h>
#include "Placeholders.hpp"
#include "WindowCompositionHelper.hpp"

extern HWND hwnd;

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

extern float btnW;
extern float btnH;
extern float btnMargin;
extern bool btnHovered;
extern bool btnPressed;

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
void ApplyOldAcrylic();
void WindowScale();
void WindowTheme();

HRESULT MainWindowComposition();
void ClearBackground();
void DrawWindowsLogo();
void DrawLine();
void DrawWindowsText();
void DrawButton();
void MainWindowDestroy();