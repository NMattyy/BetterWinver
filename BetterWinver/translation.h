// BetterWinver 1.4.1
#ifndef TRANSLATION_H
#define TRANSLATION_H

#include <string>

#include "infoGet.h"

inline std::wstring GetLang() { 
    static std::wstring Language = currentLanguage(); 
    return Language; 
}
// L"0409" English_US
// L"0410" Italian

inline std::wstring string_1() {
    if (GetLang() == L"0410"){ 
        return L"BetterWinver attualmente supporta solo Windows 8 e successivi (build 9200 e successive)";
    }
    return L"Right now BetterWinver only supports Windows 8 and newer (build 9200 and newer)";
}

inline std::wstring string_2() {
    if (GetLang() == L"0410"){ 
        return L"Attenzione: versione di Windows non compatibile";
    }
    return L"Warning: this version of Windows is not compatible";
}

inline std::wstring string_3() {
    if (GetLang() == L"0410"){ 
        return L"Informazioni su Windows";
    }
    return L"About Windows";
}

inline std::wstring string_4() {
    if (GetLang() == L"0410") {
        return L"Microsoft " + OSName + L" " + commercialVersion + L"\n"
               L"Versione NT " + NT + L" (build SO " + build + L")\n" + 
               wchar_t(169) + L" Microsoft Corporation. Tutti i diritti riservati.\n\n"
               L"Il sistema operativo " + OSName + L" e la relativa interfaccia utente sono protetti da marchi e da altri diritti di proprietà intellettuale in corso di registrazione o registrati negli Stati Uniti e/o negli altri paesi o aree geografiche\n\n\n\n"
               L"Prodotto concesso in licenza a:\n" + user;
    }
    return L"Microsoft " + OSName + L" " + commercialVersion + L"\n"
            L"NT Version " + NT + L" (OS build " + build + L")\n" + 
            wchar_t(169) + L" Microsoft Corporation. All rights reserved.\n\n"
            L"The " + OSName + L" operating system and its user interface are protected by trademark and other pending or existing intellectual property rights in the United States and other countries/regions\n\n\n\n"
            L"Product licensed to:\n" + user;               
}

#endif
