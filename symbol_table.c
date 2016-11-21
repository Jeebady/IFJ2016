/*
 * Project: IFJ16 language interpret
 * File: symbol_table.c
 * Description: Symbol table implementation
 * Author: xvalek12
 * Last edit: 19/11/16
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "symbol_table.h"
#include "error.h"

// Symbol table initialization
Tsymbol_table * table_alloc_init(Tsymbol_table **table) {
    *table = (Tsymbol_table *)malloc(sizeof(Tsymbol_table));
    if (!(*table)) {
        exit(ALLOCATION_ERROR);
    }
    
    (*table)->first = NULL;
    
    return *table;
}

Ttable_item * find_minimum(Ttable_item *item) {
    if (!(item->left_branch)) {
        return item;
    } else {
        return find_minimum(item->left_branch);
    }
}

Ttable_item * remove_item_and_return_new_root(Ttable_item *item, char *key_of_item_being_removed) {
    if (item == NULL) {
        return NULL;
    }
    
    // If the key to be deleted is smaller than the root's key,
    // then it lies in left subtree
    if (strcmp(item->key, key_of_item_being_removed) > 0) {
        item->left_branch = remove_item_and_return_new_root(item->left_branch, key_of_item_being_removed);
    }
    // If the key to be deleted is greater than the root's key,
    // then it lies in right subtree
    else if (strcmp(item->key, key_of_item_being_removed) < 0) {
        item->right_branch = remove_item_and_return_new_root(item->right_branch, key_of_item_being_removed);
    }
    // If key is same as root's key, then This is the node
    // to be deleted
    else {
        // Node with only one child or no child
        if (!item->left_branch) {
            Ttable_item *temp = item->right_branch;
            free(item->key);
            free(item);
            item = NULL;
            return temp;
        } else if (!item->right_branch) {
            Ttable_item *temp = item->left_branch;
            free(item->key);
            free(item);
            item = NULL;
            return temp;
        }
        // Node with two children: Get the inorder successor (smallest in the right subtree)
        Ttable_item *temp = find_minimum(item->right_branch);
        // Copy the inorder successor's content to this node
        strcpy(item->key, temp->key);
        
        item->right_branch = remove_item_and_return_new_root(item->right_branch, temp->key);
    }
    return item;
}

void table_remove_item(Tsymbol_table **table, char *key) {
    if (!(*table) || !key) {
        return;
    }
    
    (*table)->first = remove_item_and_return_new_root((*table)->first, key);
}

 void table_items_free(Ttable_item *item) {
     if (item) {
         table_items_free(item->left_branch);
         table_items_free(item->right_branch);
         free(item->key);
         free(item);
     }
 }
 
 void table_free(Tsymbol_table *table) {
     table_items_free(table->first);
 }
 

// If the key is definned in symbol table, function returns pointer to it
// otherwise it returns NULL
Ttable_item * table_items_lookup(Ttable_item *item, char *key) {
    if (item) {
        // If keys are equal
        if (strcmp(key, item->key) == 0) {
            return item;
        } else if (strcmp(key, item->key) < 0) {
            return table_items_lookup(item->left_branch, key);
        } else {
            return table_items_lookup(item->right_branch, key);
        }
    }
    return NULL;
}

Ttable_item * table_lookup(Tsymbol_table *table, char *key) {
    // If tree or key is empty
    if (!(table->first) || !key) {
        return NULL;
    }
    
    return table_items_lookup(table->first, key);
}

void table_item_insert(Ttable_item **item, Ttable_item *item_to_be_inserted) {
    if (!(*item)) {
        *item = item_to_be_inserted;
    } else {
        if (strcmp((*item)->key, item_to_be_inserted->key) > 0) {
            table_item_insert(&(*item)->left_branch, item_to_be_inserted);
        } else if (strcmp((*item)->key, item_to_be_inserted->key) < 0) {
            table_item_insert(&(*item)->right_branch, item_to_be_inserted);
        } else {
            exit(UNEXPECTED_ERROR_IN_TABLE_ITEM_INSERT);
        }
    }
}

void table_insert(Tsymbol_table **table, Ttable_item *item) {
    Ttable_item *found_item = table_lookup(*table, item->key);
    
    // If the item is already in the table
    if (found_item) {
        exit(ITEM_WITH_SAME_KEY);
    }
    
    table_item_insert(&(*table)->first, item);
}

Ttable_item * create_table_item(char *key, Tdata_type type) {
    Ttable_item *item = (Ttable_item *)malloc(sizeof(Ttable_item));
    if (!item) {
        exit(ALLOCATION_ERROR);
    }
    
    item->key = malloc(strlen(key)+1);
    strcpy(item->key, key);
    
    item->left_branch = NULL;
    item->right_branch = NULL;
    item->type = type;
    item->is_defined = false;
    
    return item;
}

void table_set_item_as_defined(Tsymbol_table **table, char *key) {
    Ttable_item *item = table_lookup((*table), key);
    if (item) {
        item->is_defined = true;
    }
}
int _print_t(Ttable_item *tree, int is_left, int offset, int depth, char s[20][255])
{
    char b[20];
    int width = 5;
    
    if (!tree) return 0;
    
    sprintf(b, "(%s)", tree->key);
    
    int left  = _print_t(tree->left_branch,  1, offset,                depth + 1, s);
    int right = _print_t(tree->right_branch, 0, offset + left + width, depth + 1, s);
    
#ifdef COMPACT
    for (int i = 0; i < width; i++)
        s[depth][offset + left + i] = b[i];
    
    if (depth && is_left) {
        
        for (int i = 0; i < width + right; i++)
            s[depth - 1][offset + left + width/2 + i] = '-';
        
        s[depth - 1][offset + left + width/2] = '.';
        
    } else if (depth && !is_left) {
        
        for (int i = 0; i < left + width; i++)
            s[depth - 1][offset - width/2 + i] = '-';
        
        s[depth - 1][offset + left + width/2] = '.';
    }
#else
    for (int i = 0; i < width; i++)
        s[2 * depth][offset + left + i] = b[i];
    
    if (depth && is_left) {
        
        for (int i = 0; i < width + right; i++)
            s[2 * depth - 1][offset + left + width/2 + i] = '-';
        
        s[2 * depth - 1][offset + left + width/2] = '+';
        s[2 * depth - 1][offset + left + width + right + width/2] = '+';
        
    } else if (depth && !is_left) {
        
        for (int i = 0; i < left + width; i++)
            s[2 * depth - 1][offset - width/2 + i] = '-';
        
        s[2 * depth - 1][offset + left + width/2] = '+';
        s[2 * depth - 1][offset - width/2 - 1] = '+';
    }
#endif
    
    return left + width + right;
}

void print_t(Ttable_item *tree)
{
    char s[20][255];
    for (int i = 0; i < 20; i++)
        sprintf(s[i], "%80s", " ");
    
    _print_t(tree, 0, 0, 0, s);
    
    for (int i = 0; i < 20; i++)
        printf("%s\n", s[i]);
}
