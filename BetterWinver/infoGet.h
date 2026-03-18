//BetterWinver 1.4.1
#ifndef INFOGET_H
#define INFOGET_H

#include <windows.h>
#include <winreg.h>
#include <lmcons.h>
#include <string>

extern std::wstring NT;
extern std::wstring build;
extern std::wstring OSName;
extern std::wstring commercialVersion;
extern std::wstring user;
extern int compCheck;
extern bool isDarkModeEnabled;

//Information
inline std::wstring ntGet() {
    HKEY hKey;
    DWORD major = 0;
    DWORD minor = 0;
    
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD size = sizeof(DWORD);

        if (RegQueryValueExW(hKey, L"CurrentMajorVersionNumber", NULL, NULL, (LPBYTE)&major, &size) == ERROR_SUCCESS) {
            RegQueryValueExW(hKey, L"CurrentMinorVersionNumber", NULL, NULL, (LPBYTE)&minor, &size);
        }

        if (major == 0) {
            wchar_t buildBuffer[256] = {0};
            DWORD bufferSize = sizeof(buildBuffer);

            if (RegQueryValueExW(hKey, L"CurrentVersion", NULL, NULL, (LPBYTE)buildBuffer, &bufferSize) == ERROR_SUCCESS) {
                buildBuffer[255] = L'\0';
                RegCloseKey(hKey);
                return std::wstring(buildBuffer);
            }
        }   
        RegCloseKey(hKey);  
    }
    return std::to_wstring(major) + L"." + std::to_wstring(minor);
}

inline std::wstring buildGet() {
    HKEY hKey;
    std::wstring buildNumber = L"00000";

    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        
        wchar_t buildBuffer[256] = {0};
        DWORD bSize = sizeof(buildBuffer);

        if (RegQueryValueExW(hKey, L"CurrentBuild", NULL, NULL, (LPBYTE)buildBuffer, &bSize) == ERROR_SUCCESS) {
            buildBuffer[255] = L'\0';
            buildNumber = std::wstring(buildBuffer);

            if (ntGet() == L"10.0"){
                DWORD ubrValue = 0;
                DWORD uSize = sizeof(DWORD);

                if (RegQueryValueExW(hKey, L"UBR", NULL, NULL, (LPBYTE)&ubrValue, &uSize) == ERROR_SUCCESS) {
                    buildNumber += L"." + std::to_wstring(ubrValue);
                }
            }
            RegCloseKey(hKey);
        }
    } 
    return buildNumber;
}

inline std::wstring OSGet() {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS){
        wchar_t buildBuffer[256] = {0};
        DWORD bufferSize = sizeof(buildBuffer);

        if (compCheck < 22000){
            if (RegQueryValueExW(hKey, L"ProductName", NULL, NULL, (LPBYTE)buildBuffer, &bufferSize) == ERROR_SUCCESS) {
                buildBuffer[255] = L'\0';
                RegCloseKey(hKey);
                return std::wstring(buildBuffer);
            }
        } else {
            if (RegQueryValueExW(hKey, L"EditionID", NULL, NULL, (LPBYTE)buildBuffer, &bufferSize) == ERROR_SUCCESS) {
                buildBuffer[255] = L'\0';
                RegCloseKey(hKey);
                std::wstring name = std::wstring(buildBuffer);
                if (name == L"Professional"){
                    return L"Windows 11 Pro";
                }
                return L"Windows 11 " + name;
            }
        }
        RegCloseKey(hKey);
    }
    return L"Windows";
}

inline std::wstring commercialVersionGet() {
    if (compCheck < 19042){
        return L"";
    }

    HKEY hKey;

    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        
        wchar_t buildBuffer[256] = {0};
        DWORD bufferSize = sizeof(buildBuffer);

        if (RegQueryValueExW(hKey, L"DisplayVersion", NULL, NULL, (LPBYTE)buildBuffer, &bufferSize) == ERROR_SUCCESS) {
            buildBuffer[255] = L'\0';
            RegCloseKey(hKey);
            return std::wstring(buildBuffer);
        }
        RegCloseKey(hKey);
    }

    return L"";
}

inline std::wstring userGet(){
    wchar_t userName[UNLEN + 1] = {0};
    DWORD userName_len = UNLEN + 1;
    if (GetUserNameW(userName, &userName_len)) {
        return std::wstring(userName);
    }
    return L"Unknown";
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

inline void DarkModeCheck() {
    HKEY hKey;
    DWORD value = 1; //1 Light, 0 Dark;
    DWORD valueSize = sizeof(value);

    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegQueryValueExW(hKey, L"AppsUseLightTheme", NULL, NULL, (LPBYTE)&value, &valueSize);
        RegCloseKey(hKey);
    }
    isDarkModeEnabled = (value == 0);
}

inline std::wstring currentLanguage(){
    HKEY hKey;

    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Nls\\Language", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        
        wchar_t buildBuffer[256] = {0};
        DWORD bufferSize = sizeof(buildBuffer);

        if (RegQueryValueExW(hKey, L"InstallLanguage", NULL, NULL, (LPBYTE)buildBuffer, &bufferSize) == ERROR_SUCCESS) {
            buildBuffer[255] = L'\0';
            RegCloseKey(hKey);
            return std::wstring(buildBuffer);
        }
        RegCloseKey(hKey);
    }

    return L"0409"; //English-US
}

#endif
