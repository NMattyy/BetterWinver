//BetterWinver 1.7.2
#ifndef TRANSLATION_H
#define TRANSLATION_H

#include <windows.h>
#include <strsafe.h>

#include "infoGet.hpp"

const LPCWSTR string_version = L"1.7.2";

inline LPCWSTR GetLang() { 
    static wchar_t Language[8] = { L'\0' };
    if (Language[0] == L'\0') {
        currentLanguage(Language, ARRAYSIZE(Language));
    }
    return Language; 
}

inline LPCWSTR string_1() {
    if (CompareStringOrdinal(GetLang(), -1, L"0410", -1, TRUE) == CSTR_EQUAL) 
        return L"BetterWinver attualmente supporta solo Windows 8 e successivi (build 9200 e successive)";
    return L"Right now BetterWinver only supports Windows 8 and newer (build 9200 and newer)";
}

inline LPCWSTR string_2() {
    if (CompareStringOrdinal(GetLang(), -1, L"0410", -1, TRUE) == CSTR_EQUAL)
        return L"Attenzione: versione di Windows non compatibile";
    return L"Warning: this version of Windows is not compatible";
}

inline LPCWSTR string_3() {
    if (CompareStringOrdinal(GetLang(), -1, L"0410", -1, TRUE) == CSTR_EQUAL)
        return L"Si è verificato un errore fatale durante l'avvio di un componente critico del programma";
    return L"An error has occured during the starting of a critical component of the program";
}

inline LPCWSTR string_4() {
    if (CompareStringOrdinal(GetLang(), -1, L"0410", -1, TRUE) == CSTR_EQUAL)
        return L"Errore Fatale";
    return L"Fatal Error";
}

inline LPCWSTR string_5() {
    if (CompareStringOrdinal(GetLang(), -1, L"0410", -1, TRUE) == CSTR_EQUAL)
        return L" Informazioni su Windows";
    return L" About Windows";
}

inline LPCWSTR string_6() {
    static wchar_t formattedString[2048];

    if (CompareStringOrdinal(GetLang(), -1, L"0410", -1, TRUE) == CSTR_EQUAL) {
        StringCchPrintfW(formattedString, ARRAYSIZE(formattedString),
            L"Microsoft %s %s\n"
            L"Versione NT %s (build SO %s)\n"
            L"%c Microsoft Corporation. Tutti i diritti riservati.\n\n"
            L"Il sistema operativo %s e la relativa interfaccia utente sono protetti da marchi e da altri diritti di proprietà intellettuale in corso di registrazione o registrati negli Stati Uniti e/o negli altri paesi o aree geografiche\n\n\n\n"
            L"Prodotto concesso in licenza a:\n%s",
            OSName, commercialVersion, NT, build, (wchar_t)169, OSName, user);
    } else {
        StringCchPrintfW(formattedString, ARRAYSIZE(formattedString),
            L"Microsoft %s %s\n"
            L"NT Version %s (OS build %s)\n"
            L"%c Microsoft Corporation. All rights reserved.\n\n"
            L"The %s operating system and its user interface are protected by trademark and other pending or existing intellectual property rights in the United States and other countries/regions\n\n\n\n"
            L"Product licensed to:\n%s",
            OSName, commercialVersion, NT, build, (wchar_t)169, OSName, user);
    }

    return formattedString;
}

inline LPCWSTR string_7() {
    if (CompareStringOrdinal(GetLang(), -1, L"0410", -1, TRUE) == CSTR_EQUAL)
        return L" Informazioni su BetterWinver";
    return L" About BetterWinver";
}

inline LPCWSTR string_8() {
    static wchar_t formattedStringAbout[512];

    if (CompareStringOrdinal(GetLang(), -1, L"0410", -1, TRUE) == CSTR_EQUAL) {
        StringCchPrintfW(formattedStringAbout, ARRAYSIZE(formattedStringAbout),
            L"BetterWinver\n"
            L"Versione %s\n"
            L"Creato da NMattyy",
            string_version);
    } else {
        StringCchPrintfW(formattedStringAbout, ARRAYSIZE(formattedStringAbout),
            L"BetterWinver\n"
            L"Version %s\n"
            L"Created by NMattyy",
            string_version);
    }

    return formattedStringAbout;
}

#endif
