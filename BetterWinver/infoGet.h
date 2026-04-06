//BetterWinver 1.8.0

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
        SecureZeroMemory(buffer, sizeof(buffer));
        DWORD bSize = sizeof(buffer);

        if (compCheck < 22000) {
            if (RegQueryValueExW(hKey, L"ProductName", NULL, NULL, (LPBYTE)buffer, &bSize) == ERROR_SUCCESS) {
                StringCchCopyW(out, size, buffer);
            }
        } else {
            if (RegQueryValueExW(hKey, L"EditionID", NULL, NULL, (LPBYTE)buffer, &bSize) == ERROR_SUCCESS) {
                if (CompareStringOrdinal(buffer, -1, L"Professional", -1, TRUE) == CSTR_EQUAL) {
                    StringCchCopyW(out, size, L"Windows 11 Pro"); 
                } else if (CompareStringOrdinal(buffer, -1, L"Core", -1, TRUE) == CSTR_EQUAL) {
                    StringCchCopyW(out, size, L"Windows 11 Home"); 
                } else {
                    StringCchPrintfW(out, size, L"Windows 11 %s", buffer);
                }
            }
        }
        RegCloseKey(hKey);
    } else {
        StringCchCopyW(out, size, L"Windows");
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

    if (argv) {
        for (int i = 1; i < argc; i++) {
            if (CompareStringOrdinal(argv[i], -1, L"-customusername", -1, TRUE) == CSTR_EQUAL) {
                if (i + 1 < argc) {
                    StringCchCopyW(out, size, argv[i+1]);
                }
                return;
            }
        }
    }
    
    if (GetUserNameW(userName, &userName_len)) {
        StringCchCopyW(out, size, userName);
    } else {
        StringCchCopyW(out, size, L"Unknown");
    }
}
