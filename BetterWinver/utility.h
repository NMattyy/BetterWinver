//BetterWinver 1.8.0

inline void GetRegString(HKEY hRoot, LPCWSTR subKey, LPCWSTR valueName, wchar_t* outBuffer, DWORD bufferSize) {
    HKEY hKey;
    outBuffer[0] = L'\0';
    if (RegOpenKeyExW(hRoot, subKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegQueryValueExW(hKey, valueName, NULL, NULL, (LPBYTE)outBuffer, &bufferSize);
        RegCloseKey(hKey);
    }
}

inline LPCWSTR GetResString(UINT id) {
    static wchar_t buffer[2048]; 
    
    int len = LoadStringW(GetModuleHandle(NULL), id, buffer, ARRAYSIZE(buffer));
    if (len > 0) {
        return buffer; 
    }
    return L"";
}

inline int ScaleValue(int value, UINT dpi) {
    return MulDiv(value, dpi, 96);
}

inline float ScaleValueF(float value, UINT dpi) {
    return (value * (float)dpi) / 96.0f;
}