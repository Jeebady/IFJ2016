/*
 * Project: IFJ16 language interpret
 * File: symbol_table.h
 * Description: Symbol table interface
 * Author: xvalek12
 * Last edit: 19/11/16
 */

#ifndef SYMBOL_TABLE_HEADER
#define SYMBOL_TABLE_HEADER

#include <stdbool.h>

// CUSTOM STRUCTURES AND ENUMERATION TYPES
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Data types
typedef enum data_types {
    VOID_DT,
    INT_DT,
    DOUBLE_DT,
    STRING_DT,
} Tdata_type;

// Table item
typedef struct table_item {
    struct table_item *left_branch;
    struct table_item *right_branch;
    
    char *key;
    Tdata_type type;
    bool is_defined;
} Ttable_item;

// Symbol table
typedef struct symbol_table {
    Ttable_item *first;
} Tsymbol_table;


// SYMBOL TABLE FUNCTIONS
// ~~~~~~~~~~~~~~~~~~~~~~~

// Symbol table memory allocation and initialization
Tsymbol_table * table_alloc_init(Tsymbol_table **table);

// Symbol table removal
void table_free(Tsymbol_table *table);

// Returns a pointer to an item with matching key, if there is any, otherwise returns NULL
Ttable_item * table_lookup(Tsymbol_table *table, char *key);

// Insert an item into a symbol table
void table_insert(Tsymbol_table **table, Ttable_item *item);

// Remove item with given key from the table
void table_remove_item(Tsymbol_table **table, char *key);

// Set item in the table as defined
void table_set_item_as_defined(Tsymbol_table **table, char *key);

// Creation of a new item that could be inserted into a symbol table
Ttable_item * create_table_item(char *key, Tdata_type type);


void print_t(Ttable_item *tree);
int _print_t(Ttable_item *tree, int is_left, int offset, int depth, char s[20][255]);

#endif
