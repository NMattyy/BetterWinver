//BetterWinver 1.2.0
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
