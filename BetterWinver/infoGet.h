#include <windows.h>
#include <winreg.h>
#include <lmcons.h>
#include <string>

using namespace std;

//Placeholders

string ntGet(){
    OSVERSIONINFOEX info;
    ZeroMemory(&info, sizeof(OSVERSIONINFOEX));
    info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    GetVersionEx((LPOSVERSIONINFO)&info);

    return to_string(info.dwMajorVersion) + "." + to_string(info.dwMinorVersion);
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
int build = stoi(buildGet());

string OSGet() {
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS){
        char buildBuffer[256];
        DWORD bufferSize = sizeof(buildBuffer);

        if (build < 22000){
            if (RegQueryValueExA(hKey, "ProductName", NULL, NULL, (LPBYTE)buildBuffer, &bufferSize) == ERROR_SUCCESS) {
                RegCloseKey(hKey);
                return string(buildBuffer);
            }
        } else {
            if (RegQueryValueExA(hKey, "EditionID", NULL, NULL, (LPBYTE)buildBuffer, &bufferSize) == ERROR_SUCCESS) {
                RegCloseKey(hKey);
                return "Windows 11 " + string(buildBuffer);
            }
        }
        RegCloseKey(hKey);
    }
    return "Windows";
}

string commercialVersionGet() {
    if (build < 19042){
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

    } else {
        return "";
    }
}

string userGet(){
    TCHAR userName[UNLEN + 1];
    DWORD userName_len = UNLEN + 1;
    if (GetUserName(userName, &userName_len)) {
        
        #ifdef UNICODE
            wstring ws(userName);
            return string(ws.begin(), ws.end());
            
        #else
            return string(userName);
        #endif
    }
    return "Unknown";
}

//Settings

bool isDarkModeEnabled() {
    HKEY hKey;
    DWORD value = 1; //1 Light, 0 Dark;
    DWORD valueSize = sizeof(value);

    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegQueryValueExA(hKey, "AppsUseLightTheme", NULL, NULL, (LPBYTE)&value, &valueSize);
        RegCloseKey(hKey);
    }
    
    return (value == 0);
}