//BetterWinver 1.3.1
#ifndef INFOGET_H
#define INFOGET_H

#include <windows.h>
#include <winreg.h>
#include <lmcons.h>
#include <string>

using namespace std;

extern string NT;
extern string build;
extern string OSName;
extern string commercialVersion;
extern string user;
extern int compCheck;
extern bool isDarkModeEnabled;

//Placeholders

string ntGet() {
    HKEY hKey;
    DWORD major = 0;
    DWORD minor = 0;
    
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD size = sizeof(DWORD);

        if (RegQueryValueExA(hKey, "CurrentMajorVersionNumber", NULL, NULL, (LPBYTE)&major, &size) == ERROR_SUCCESS) {
            RegQueryValueExA(hKey, "CurrentMinorVersionNumber", NULL, NULL, (LPBYTE)&minor, &size);
        }

        if (major == 0) {
            char buildBuffer[256];
            DWORD bufferSize = sizeof(buildBuffer);

            if (RegQueryValueExA(hKey, "CurrentVersion", NULL, NULL, (LPBYTE)buildBuffer, &bufferSize) == ERROR_SUCCESS) {
                RegCloseKey(hKey);
                return string(buildBuffer);
            }
        }   
        RegCloseKey(hKey);  
    }
    return to_string(major) + "." + to_string(minor);
}

string buildGet() {
    HKEY hKey;
    string buildNumber = "00000";

    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        
        char buildBuffer[256];
        DWORD bSize = sizeof(buildBuffer);

        if (RegQueryValueExA(hKey, "CurrentBuild", NULL, NULL, (LPBYTE)buildBuffer, &bSize) == ERROR_SUCCESS) {
            buildNumber = string(buildBuffer);

            if (ntGet() == "10.0"){
                DWORD ubrValue = 0;
                DWORD uSize = sizeof(DWORD);

                if (RegQueryValueExA(hKey, "UBR", NULL, NULL, (LPBYTE)&ubrValue, &uSize) == ERROR_SUCCESS) {
                    buildNumber += "." + to_string(ubrValue);
                }
            }
            RegCloseKey(hKey);
        }
    } 
    return buildNumber;
}

string OSGet() {
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS){
        char buildBuffer[256];
        DWORD bufferSize = sizeof(buildBuffer);

        if (compCheck < 22000){
            if (RegQueryValueExA(hKey, "ProductName", NULL, NULL, (LPBYTE)buildBuffer, &bufferSize) == ERROR_SUCCESS) {
                RegCloseKey(hKey);
                return string(buildBuffer);
            }
        } else {
            if (RegQueryValueExA(hKey, "EditionID", NULL, NULL, (LPBYTE)buildBuffer, &bufferSize) == ERROR_SUCCESS) {
                RegCloseKey(hKey);
                string name = string(buildBuffer);
                if (name == "Professional"){
                    return "Windows 11 Pro";
                }
                return "Windows 11 " + name;
            }
        }
        RegCloseKey(hKey);
    }
    return "Windows";
}

string commercialVersionGet() {
    if (compCheck < 19042){
        return "";
    }

    HKEY hKey;

    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        
        char buildBuffer[256];
        DWORD bufferSize = sizeof(buildBuffer);

        if (RegQueryValueExA(hKey, "DisplayVersion", NULL, NULL, (LPBYTE)buildBuffer, &bufferSize) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return string(buildBuffer);
        }
        RegCloseKey(hKey);
    }

    return "";
}

string userGet(){
    wchar_t userName[UNLEN + 1];
    DWORD userName_len = UNLEN + 1;
    if (GetUserNameW(userName, &userName_len)) {
        char buffer[UNLEN * 3];
        WideCharToMultiByte(CP_UTF8, 0, userName, -1, buffer, sizeof(buffer), NULL, NULL);
        return string(buffer);
    }
    return "Unknown";
}

//Settings
UINT GetSystemDPI() {
    UINT dpi = 96;
    HMODULE hUser32 = GetModuleHandleA("user32.dll");
    
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

void DarkModeCheck() {
    HKEY hKey;
    DWORD value = 1; //1 Light, 0 Dark;
    DWORD valueSize = sizeof(value);

    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegQueryValueExA(hKey, "AppsUseLightTheme", NULL, NULL, (LPBYTE)&value, &valueSize);
        RegCloseKey(hKey);
    }
    isDarkModeEnabled = (value == 0);
}

string currentLanguage(){
    HKEY hKey;

    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Nls\\Language", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        
        char buildBuffer[256];
        DWORD bufferSize = sizeof(buildBuffer);

        if (RegQueryValueExA(hKey, "InstallLanguage", NULL, NULL, (LPBYTE)buildBuffer, &bufferSize) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return string(buildBuffer);
        }
        RegCloseKey(hKey);
    }

    return "0409"; //English-US
}

#endif
