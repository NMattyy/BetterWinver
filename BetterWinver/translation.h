//BetterWinver 1.7.0
#ifndef TRANSLATION_H
#define TRANSLATION_H

#include <windows.h>
#include <shlwapi.h>

#include "infoGet.hpp"

static wchar_t formattedString[2048];

inline LPCWSTR GetLang() { 
    static wchar_t Language[8] = {0};
    if (Language[0] == L'\0') {
        currentLanguage(Language, 8);
    }
    return Language; 
}

inline LPCWSTR string_1() {
    if (lstrcmpW(GetLang(), L"0410") == 0) 
        return L"BetterWinver attualmente supporta solo Windows 8 e successivi (build 9200 e successive)";
    return L"Right now BetterWinver only supports Windows 8 and newer (build 9200 and newer)";
}

inline LPCWSTR string_2() {
    if (lstrcmpW(GetLang(), L"0410") == 0)
        return L"Attenzione: versione di Windows non compatibile";
    return L"Warning: this version of Windows is not compatible";
}

inline LPCWSTR string_3() {
    if (lstrcmpW(GetLang(), L"0410") == 0)
        return L"Si è verificato un errore fatale durante l'avvio di un componente critico del programma";
    return L"An error has occured during the starting of a critical component of the program";
}

inline LPCWSTR string_4() {
    if (lstrcmpW(GetLang(), L"0410") == 0)
        return L"Errore Fatale";
    return L"Fatal Error";
}

inline LPCWSTR string_5() {
    if (lstrcmpW(GetLang(), L"0410") == 0)
        return L" Informazioni su Windows";
    return L" About Windows";
}

inline LPCWSTR string_6() {
    formattedString[0] = L'\0';

    if (lstrcmpW(GetLang(), L"0410") == 0) {
        wnsprintf(formattedString, 2048,
            L"Microsoft %s %s\n"
            L"Versione NT %s (build SO %s)\n"
            L"%c Microsoft Corporation. Tutti i diritti riservati.\n\n"
            L"Il sistema operativo %s e la relativa interfaccia utente sono protetti da marchi e da altri diritti di proprietà intellettuale in corso di registrazione o registrati negli Stati Uniti e/o negli altri paesi o aree geografiche\n\n\n\n"
            L"Prodotto concesso in licenza a:\n%s",
            OSName, commercialVersion, NT, build, (wchar_t)169, OSName, user);
    } else {
        wnsprintf(formattedString, 2048,
            L"Microsoft %s %s\n"
            L"NT Version %s (OS build %s)\n"
            L"%c Microsoft Corporation. All rights reserved.\n\n"
            L"The %s operating system and its user interface are protected by trademark and other pending or existing intellectual property rights in the United States and other countries/regions\n\n\n\n"
            L"Product licensed to:\n%s",
            OSName, commercialVersion, NT, build, (wchar_t)169, OSName, user);
    }

    return formattedString;
}

#endif
