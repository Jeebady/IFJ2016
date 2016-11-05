/*
 * Project: IFJ16 language interpret
 * File: scanner.h
 * Description: Lexical analyser interface
 * Author: xandri03
 * Last edit: 15/10/16
 */

#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>

/* Token types: standard types & error handlers. */
enum {
	BOOLEAN, BREAK, CLASS, CONTINUE, DO, DOUBLE, ELSE, FALSE, FOR, IF, INT, RETURN, STRING, STATIC, TRUE, VOID, WHILE,	// Reserved literals
	INTEGER_LITERAL, DOUBLE_LITERAL, STRING_LITERAL,			// Regular literals
	COMMA, SEMICOLON, LEFT_BRACKET, RIGHT_BRACKET, LEFT_BRACE, RIGHT_BRACE,	// Punctuation
	PLUS, MINUS, ASTERISK, SLASH,						// Arithmetic operators
	EQUAL, NOT_EQUAL, LESS, LESS_OR_EQUAL, GREATER, GREATER_OR_EQUAL,	// Relational operators
	IDENTIFIER, ASSIGNMENT,							// Miscellaneous
	INVALID_CHARACTER, INVALID_IDENTIFIER, INVALID_NUMBER, INVALID_STRING, UNCLOSED_STRING, UNCLOSED_COMMENT, ALLOCATION_FAILURE	// Error states
} token_type;

// Token structure:
// .type is from token_type + {EOF}
// .tag is a pointer to dynamically allocated memory containing tag when .type is
// from {IDENTIFIER, INTEGER_LITERAL, DOUBLE_LITERAL, STRING_LITERAL} and NULL otherwise
typedef struct token_t {
	int type;
	char *tag;
} Token;

// Read single token from {@code stream}.
// Type of token (.type) not only determines the type of recognised token, but
// also carries additional information about occured errors.
// @param stream Input stream of characters (!= NULL)
// @return (recognised) token
// NOTE: Token is returned (not a pointer to one) since there is no
// reason to allocate memory for 8-16B each time the function is called.
// NOTE: token tag (.tag) is (when non-NULL) a pointer to dynamically
// allocated memory, so make sure you deallocate it correctly.
Token get_token(FILE *stream);

#endif
