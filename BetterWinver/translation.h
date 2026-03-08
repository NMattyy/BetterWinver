//BetterWinver 1.2.0
#ifndef TRANSLATION_H
#define TRANSLATION_H

#include "infoGet.h"

inline string GetLang() { static string L = currentLanguage(); return L; }
//0409 English_US
//0410 Italian

string string_1(){
    if (GetLang() == "0409"){
        return "Right now BetterWinver only supports Windows 8 and newer (build 9200 and newer)";
    } else if (GetLang() == "0410") {
        return "BetterWinver attualmente supporta solo Windows 8 e successivi (build 9200 e successive)";
    } else {
    return "Right now BetterWinver only supports Windows 8 and newer (build 9200 and newer)";
    }
}

string string_2(){
    if (GetLang() == "0409"){
        return "Warning: this version of Windows is not compatible";
    } else if (GetLang() == "0410") {
        return "Attenzione: versione di Windows non compatibile";
    } else {
    return "Warning: this version of Windows is not compatible";
    }
}

string string_3(){
    if (GetLang() == "0409"){
        return "About Windows";
    } else if (GetLang() == "0410") {
        return "Informazioni su Windows";
    } else {
    return "About Windows";
    }
}

string string_4(){
    if (GetLang() == "0409"){
        return string("Microsoft ") + OSName + " " + commercialVersion + "\n"
                    "NT Version " + NT + " (OS build " + build + ")\n" + 
                    (char)169 + " Microsoft Corporation. All rights reserved.\n\n" + 
                    "The " + OSName + " operating system and its user interface are protected by trademark and other pending or existing intellectual property rights in the United States and other countries/regions";
    } else if (GetLang() == "0410") {
        return string("Microsoft ") + OSName + " " + commercialVersion + "\n"
                    "Versione NT " + NT + " (build SO " + build + ")\n" + 
                    (char)169 + " Microsoft Corporation. Tutti i diritti riservati.\n\n" + 
                    "Il sistema operativo " + OSName + " e la relativa interfaccia utente sono protetti da marchi e da altri diritti di propriet\xE0 intelletuale in corso di registrazione o registrati negli Stati Uniti e/o negli altri paesi o aree geografiche";
    } else {
    return string("Microsoft ") + OSName + " " + commercialVersion + "\n"
                    "NT Version " + NT + " (OS build " + build + ")\n" + 
                    (char)169 + " Microsoft Corporation. All rights reserved.\n\n" + 
                    "The " + OSName + " operating system and its user interface are protected by trademark and other pending or existing intellectual property rights in the United States and other countries/regions";
    }
}

string string_5(){
    if (GetLang() == "0409"){
        return "Product licensed to:\n" + user;
    } else if (GetLang() == "0410") {
        return "Prodotto concesso in licenza a:\n" + user;
    } else {
    return "Product licensed to:\n" + user;
    }
}

#endif
