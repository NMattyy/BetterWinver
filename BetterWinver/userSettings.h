//BetterWinver 1.8.0

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
    if (argv) {
        for (int i = 1; i < argc; i++) {
            if (CompareStringOrdinal(argv[i], -1, L"-forcedarkmode", -1, TRUE) == CSTR_EQUAL) {
                isDarkModeEnabled = true;
                return;
            } else if (CompareStringOrdinal(argv[i], -1, L"-forcelightmode", -1, TRUE) == CSTR_EQUAL) {
                isDarkModeEnabled = false;
                return;
            }
        }
    }

    HKEY hKey;
    DWORD value = 1; // 1 Light, 0 Dark;
    DWORD valueSize = sizeof(value);

    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegQueryValueExW(hKey, L"AppsUseLightTheme", NULL, NULL, (LPBYTE)&value, &valueSize);
        RegCloseKey(hKey);
    }
    isDarkModeEnabled = (value == 0);
}