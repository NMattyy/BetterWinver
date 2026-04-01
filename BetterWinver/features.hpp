//BetterWinver 1.7.1
#ifndef INFOGET_H
#define INFOGET_H

#include <windows.h>
#include <winreg.h>
#include <lmcons.h>

extern wchar_t NT[64];
extern wchar_t build[64];
extern wchar_t OSName[128];
extern wchar_t commercialVersion[64];
extern wchar_t user[128];

extern int compCheck;

extern bool isDarkModeEnabled;

extern int argc;
extern LPWSTR* argv;

//Informations
inline void GetRegString(HKEY hRoot, LPCWSTR subKey, LPCWSTR valueName, wchar_t* outBuffer, DWORD bufferSize) {
    HKEY hKey;
    outBuffer[0] = L'\0';
    if (RegOpenKeyExW(hRoot, subKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegQueryValueExW(hKey, valueName, NULL, NULL, (LPBYTE)outBuffer, &bufferSize);
        RegCloseKey(hKey);
    }
}

inline void ntGet(wchar_t* out, DWORD size) {
    HKEY hKey;
    DWORD major = 0, minor = 0;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD dSize = sizeof(DWORD);
        if (RegQueryValueExW(hKey, L"CurrentMajorVersionNumber", NULL, NULL, (LPBYTE)&major, &dSize) == ERROR_SUCCESS) {
            RegQueryValueExW(hKey, L"CurrentMinorVersionNumber", NULL, NULL, (LPBYTE)&minor, &dSize);
            wsprintfW(out, L"%d.%d", major, minor);
        } else {
            DWORD bSize = size * sizeof(wchar_t);
            RegQueryValueExW(hKey, L"CurrentVersion", NULL, NULL, (LPBYTE)out, &bSize);
        }
        RegCloseKey(hKey);
    }
}

inline void buildGet(wchar_t* out, DWORD size) {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD bSize = size * sizeof(wchar_t);
        RegQueryValueExW(hKey, L"CurrentBuild", NULL, NULL, (LPBYTE)out, &bSize);
        
        DWORD ubr = 0, uSize = sizeof(DWORD);
        if (RegQueryValueExW(hKey, L"UBR", NULL, NULL, (LPBYTE)&ubr, &uSize) == ERROR_SUCCESS) {
            wchar_t tmp[16];
            wsprintfW(tmp, L".%d", ubr);
            lstrcatW(out, tmp);
        }
        RegCloseKey(hKey);
    }
}

inline void OSGet(wchar_t* out, DWORD size) {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        wchar_t buffer[256] = {0};
        DWORD bSize = sizeof(buffer);

        if (compCheck < 22000) {
            if (RegQueryValueExW(hKey, L"ProductName", NULL, NULL, (LPBYTE)buffer, &bSize) == ERROR_SUCCESS) {
                lstrcpynW(out, buffer, size);
            }
        } else {
            if (RegQueryValueExW(hKey, L"EditionID", NULL, NULL, (LPBYTE)buffer, &bSize) == ERROR_SUCCESS) {
                if (lstrcmpiW(buffer, L"Professional") == 0) {
                    lstrcpynW(out, L"Windows 11 Pro", size);
                } else {
                    wsprintfW(out, L"Windows 11 %s", buffer);
                }
            }
        }
        RegCloseKey(hKey);
    } else {
        lstrcpynW(out, L"Windows", size);
    }
}

inline void commercialVersionGet(wchar_t* out, DWORD size) {
    out[0] = L'\0';
    if (compCheck < 19042) return;

    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD bSize = size * sizeof(wchar_t);
        RegQueryValueExW(hKey, L"DisplayVersion", NULL, NULL, (LPBYTE)out, &bSize);
        RegCloseKey(hKey);
    }
}

inline int userGet(wchar_t* out, DWORD size) {
    wchar_t userName[UNLEN + 1] = {0};
    DWORD userName_len = UNLEN + 1;

    if (argv) {
        for (int i = 1; i < argc; i++) {
            if (lstrcmpiW(argv[i], L"-customusername") == 0) {
                lstrcpynW(out, argv[i+1], size);
                return 0;
            }
        }
    }
    
    if (GetUserNameW(userName, &userName_len)) {
        lstrcpynW(out, userName, size);
    } else {
        lstrcpynW(out, L"Unknown", size);
    }
    return 0;
}

//Settings
inline UINT GetSystemDPI() {
    UINT dpi = 96;
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    
    typedef UINT (WINAPI* GetDpiForSystemProc)();
    GetDpiForSystemProc pGetDpiForSystem = (GetDpiForSystemProc)GetProcAddress(hUser32, "GetDpiForSystem");
    
    if (pGetDpiForSystem) {
        dpi = pGetDpiForSystem();
    } else {
        HDC hdc = GetDC(NULL);
        if (hdc) {
            dpi = GetDeviceCaps(hdc, LOGPIXELSX);
            ReleaseDC(NULL, hdc);
        }
    }
    return dpi;
}

inline UINT GetDpiForWindow(HWND hwnd) {
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        typedef UINT (WINAPI* GetDpiForWindowProc)(HWND);
        GetDpiForWindowProc pGetDpiForWindow = (GetDpiForWindowProc)GetProcAddress(hUser32, "GetDpiForWindow");
        
        if (pGetDpiForWindow) {
            return pGetDpiForWindow(hwnd);
        }
    }
    return GetSystemDPI(); 
}

inline int DarkModeCheck() {
    if (argv) {
        for (int i = 1; i < argc; i++) {
            if (lstrcmpiW(argv[i], L"-forcedarkmode") == 0) {
                isDarkModeEnabled = true;
                return 0;
            } else if (lstrcmpiW(argv[i], L"-forcelightmode") == 0) {
                isDarkModeEnabled = false;
                return 0;
            }
        }
    }

    HKEY hKey;
    DWORD value = 1; //1 Light, 0 Dark;
    DWORD valueSize = sizeof(value);

    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegQueryValueExW(hKey, L"AppsUseLightTheme", NULL, NULL, (LPBYTE)&value, &valueSize);
        RegCloseKey(hKey);
    }
    isDarkModeEnabled = (value == 0);
    return 0;
}

inline void currentLanguage(wchar_t* out, DWORD size) {
    lstrcpynW(out, L"0409", size); // Default English-US
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Nls\\Language", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD bSize = size * sizeof(wchar_t);
        RegQueryValueExW(hKey, L"InstallLanguage", NULL, NULL, (LPBYTE)out, &bSize);
        RegCloseKey(hKey);
    }
}

//Utility
inline int ScaleValue(int value, UINT dpi) {
    return MulDiv(value, dpi, 96);
}

inline float ScaleValueF(float value, UINT dpi) {
    return (value * (float)dpi) / 96.0f;
}

template <class T> void SafeRelease(T **ppT) {
    if (*ppT) {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

#endif
