//InfoGet 2.1.0
#include "Headers.hpp"

LPCWSTR GetResString(UINT id) {
    static wchar_t buffer[2048];

    int len = LoadStringW(GetModuleHandle(NULL), id, buffer, ARRAYSIZE(buffer));
    if (len > 0) {
        return buffer;
    }
    return L"";
}

void BuildGet(wchar_t* out, DWORD size) {
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

void ManualLanguageGet() {
    if (args != nullptr && argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (CompareStringOrdinal(args[i], -1, L"-lang", -1, TRUE) == CSTR_EQUAL) {
                if (i + 1 < argc) {
                    if (CompareStringOrdinal(args[i + 1], -1, L"it", -1, TRUE) == CSTR_EQUAL) {
                        SetThreadUILanguage(MAKELANGID(LANG_ITALIAN, SUBLANG_ITALIAN));
                    }
                    else if (CompareStringOrdinal(args[i + 1], -1, L"en", -1, TRUE) == CSTR_EQUAL) {
                        SetThreadUILanguage(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));
                    }
                }
            }
        }
    }
}

void NTGet(wchar_t* out, DWORD size) {
    HKEY hKey;
    DWORD major = 0, minor = 0;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD dSize = sizeof(DWORD);
        if (RegQueryValueExW(hKey, L"CurrentMajorVersionNumber", NULL, NULL, (LPBYTE)&major, &dSize) == ERROR_SUCCESS) {
            RegQueryValueExW(hKey, L"CurrentMinorVersionNumber", NULL, NULL, (LPBYTE)&minor, &dSize);
            StringCchPrintfW(out, size, L"%u.%u", major, minor);
        }
        else {
            DWORD bSize = size * sizeof(wchar_t);
            RegQueryValueExW(hKey, L"CurrentVersion", NULL, NULL, (LPBYTE)out, &bSize);
        }
        RegCloseKey(hKey);
    }
}

void OSGet(wchar_t* out, DWORD size) {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        wchar_t buffer[256];
        DWORD bSize = sizeof(buffer);

        if (RegQueryValueExW(hKey, L"ProductName", NULL, NULL, (LPBYTE)buffer, &bSize) == ERROR_SUCCESS) {

            if (build >= 21996) {
                wchar_t* pos = wcsstr(buffer, L"Windows 10");
                if (pos != NULL) {
                    wchar_t finalString[256];

                    StringCchCopyW(finalString, 256, L"Windows 11");

                    StringCchCatW(finalString, 256, pos + 10);

                    StringCchCopyW(out, size, finalString);
                }
                else {
                    StringCchCopyW(out, size, buffer);
                }
            }
            else {
                StringCchCopyW(out, size, buffer);
            }
        }
        RegCloseKey(hKey);
    }
}

void VersionGet(wchar_t* out, DWORD size) {
    out[0] = L'\0';
    if (build < 19042) return;

    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD bSize = size * sizeof(wchar_t);
        RegQueryValueExW(hKey, L"DisplayVersion", NULL, NULL, (LPBYTE)out, &bSize);
        RegCloseKey(hKey);
    }
}

void UserGet(wchar_t* out, DWORD size) {
    HKEY hKey;
    if (args != nullptr && argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (CompareStringOrdinal(args[i], -1, L"-customusername", -1, TRUE) == CSTR_EQUAL) {
                if (i + 1 < argc) {
                    StringCchCopyW(out, size, args[i + 1]);
                    return;
                }
            }
        }
    }

    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD bSize = size * sizeof(wchar_t);
        RegQueryValueExW(hKey, L"LastUsedUsername", NULL, NULL, (LPBYTE)out, &bSize);
        RegCloseKey(hKey);
    }
}

void DarkModeCheck() {
    if (args) {
        for (int i = 1; i < argc; i++) {
            if (CompareStringOrdinal(args[i], -1, L"-forcedarkmode", -1, TRUE) == CSTR_EQUAL) {
                darkMode = true;
                return;
            }
            else if (CompareStringOrdinal(args[i], -1, L"-forcelightmode", -1, TRUE) == CSTR_EQUAL) {
                darkMode = false;
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
    darkMode = (value == 0);
}