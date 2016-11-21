//
//  error.h
//  SymbolTable
//
//  Created by Adam Valek on 15/11/2016.
//  Copyright © 2016 Adam Valek. All rights reserved.
//

#ifndef error_h
#define error_h

// 1 - chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexému).
// 2 - chyba v programu v rámci syntaktické analýzy (chybná syntaxe programu).
// 3 - sémantická chyba v programu – nedefinovaná trída/funkce/promenná, pokus o redefinici trídy/funkce/promenné, atd.
// 4 - sémantická chyba typové kompatibility v aritmetických, retezcových a relacních výrazech, príp. špatný pocet ci typ parametru u volání funkce.
// 6 - ostatní sémantické chyby.
// 7 - behová chyba pri nacítání císelné hodnoty ze vstupu.
// 8 - behová chyba pri práci s neinicializovanou promennou.
// 9 - behová chyba delení nulou.
// 10 - ostatní behové chyby. ˇ
// 99 - interní chyba interpretu tj. neovlivnená vstupním programem (napr. chyba alokace pameti, chyba pri otvírání souboru srídicím programem, špatné parametry príkazové rádky atd.).

typedef int make_iso_compilers_happy;

#define ALLOCATION_ERROR 1
#define ITEM_WITH_SAME_KEY 3
#define UNEXPECTED_ERROR_IN_TABLE_ITEM_INSERT 100

#endif
