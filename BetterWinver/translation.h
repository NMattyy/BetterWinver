//BetterWinver 1.3.0
#ifndef TRANSLATION_H
#define TRANSLATION_H

#include "infoGet.h"
#include <string>

using namespace std;

wstring ToWString(const string& s) {
    if (s.empty()) return wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &s[0], (int)s.size(), NULL, 0);
    wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &s[0], (int)s.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

string GetLang() { static string L = currentLanguage(); return L; }
//0409 English_US
//0410 Italian

string string_1(){
    if (GetLang() == "0410") return "BetterWinver attualmente supporta solo Windows 8 e successivi (build 9200 e successive)";
    return "Right now BetterWinver only supports Windows 8 and newer (build 9200 and newer)";
}

string string_2(){
    if (GetLang() == "0410") return "Attenzione: versione di Windows non compatibile";
    return "Warning: this version of Windows is not compatible";
}

string string_3(){
    if (GetLang() == "0410") return "Informazioni su Windows";
    return "About Windows";
}

wstring string_4(){
    wstring wOSName = ToWString(OSName);
    wstring wCommVer = ToWString(commercialVersion);
    wstring wNT = ToWString(NT);
    wstring wBuild = ToWString(build);

    if (GetLang() == "0410") {
        return L"Microsoft " + wOSName + L" " + wCommVer + L"\n"
               L"Versione NT " + wNT + L" (build SO " + wBuild + L")\n" + 
               wchar_t(169) + L" Microsoft Corporation. Tutti i diritti riservati.\n\n"
               L"Il sistema operativo " + wOSName + L" e la relativa interfaccia utente sono protetti da marchi e da altri diritti di propriet\x00E0 intellettuale in corso di registrazione o registrati negli Stati Uniti e/o negli altri paesi o aree geografiche";
    } else {
        return L"Microsoft " + wOSName + L" " + wCommVer + L"\n"
               L"NT Version " + wNT + L" (OS build " + wBuild + L")\n" + 
               wchar_t(169) + L" Microsoft Corporation. All rights reserved.\n\n"
               L"The " + wOSName + L" operating system and its user interface are protected by trademark and other pending or existing intellectual property rights in the United States and other countries/regions";
    }
}

wstring string_5(){
    wstring wUser = ToWString(user);
    if (GetLang() == "0410") {
        return L"Prodotto concesso in licenza a:\n" + wUser;
    } else {
        return L"Product licensed to:\n" + wUser;
    }
}

#endif
