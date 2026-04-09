//BetterWinver 1.8.1

inline void ntGet(wchar_t* out, DWORD size) {
    HKEY hKey;
    DWORD major = 0, minor = 0;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD dSize = sizeof(DWORD);
        if (RegQueryValueExW(hKey, L"CurrentMajorVersionNumber", NULL, NULL, (LPBYTE)&major, &dSize) == ERROR_SUCCESS) {
            RegQueryValueExW(hKey, L"CurrentMinorVersionNumber", NULL, NULL, (LPBYTE)&minor, &dSize);
            StringCchPrintfW(out, size, L"%u.%u", major, minor);
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
            StringCchPrintfW(tmp, ARRAYSIZE(tmp), L".%u", ubr);
            StringCchCatW(out, size, tmp);
        }
        RegCloseKey(hKey);
    }
}

inline void OSGet(wchar_t* out, DWORD size) {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        wchar_t buffer[256];
        DWORD bSize = sizeof(buffer);

        if (RegQueryValueExW(hKey, L"ProductName", NULL, NULL, (LPBYTE)buffer, &bSize) == ERROR_SUCCESS) {
            
            if (compCheck >= 22000) {
                wchar_t* pos = wcsstr(buffer, L"Windows 10");
                if (pos != NULL) {
                    wchar_t finalString[256];
                    
                    StringCchCopyW(finalString, 256, L"Windows 11");
                    
                    StringCchCatW(finalString, 256, pos + 10);
                    
                    StringCchCopyW(out, size, finalString);
                } else {
                    StringCchCopyW(out, size, buffer);
                }
            } else {
                StringCchCopyW(out, size, buffer);
            }
        }
        RegCloseKey(hKey);
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

inline void userGet(wchar_t* out, DWORD size) {
    wchar_t userName[UNLEN + 1] = {0};
    DWORD userName_len = UNLEN + 1;

    if (argv != nullptr && argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (CompareStringOrdinal(argv[i], -1, L"-customusername", -1, TRUE) == CSTR_EQUAL) {
                if (i + 1 < argc) {
                    StringCchCopyW(out, size, argv[i + 1]);
                    return;
                }
            }
        }
    }
    
    if (GetUserNameW(userName, &userName_len)) {
        StringCchCopyW(out, size, userName);
    } else {
        StringCchCopyW(out, size, L"Unknown");
    }
}