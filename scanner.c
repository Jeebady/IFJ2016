/*
 * Project: IFJ16 language interpret
 * File: scanner.c
 * Description: Lexical analyser implementation
 * Author: xandri03
 * Last edit: 15/10/16
 */

#include "scanner.h"

#include <assert.h>
#include <stdio.h>	// FILE *, fgetc(), ungetc()
#include <ctype.h>	// isspace(), isalpha(), isalnum(), isdigit()
#include <stdlib.h>	// malloc(), realloc(), free()
#include <stdbool.h>	
#include <string.h>	// strcmp()

#define append(arr, a) if(arr_append(arr, a)) { return ALLOCATION_FAILURE; }

/****************
 * DECLARATIONS *
 ****************/

/*
 * Dynamic array operations
 */

/* Dynamic array structure: capacity of array, it's current size and array itself. */
typedef struct array_t {
	unsigned cap;
	unsigned size;
	char *str;
} Array;

// Allocate structure array_t and a string of INITIAL_CAPACITY size.
// @return NULL on failed allocation and a valid pointer to allocated structure otherwise
static Array *arr_ctor(void);

// Deallocate all memory within {@code arr}.
// @param arr Dynamic array to deallocate (!= NULL)
static void arr_dtor(Array *arr);

// Append character {@code c} to the end of dynamic array {@code arr}.
// If the array is full, double it's capacity.
// @param arr Pointer to dynamic array (!= NULL)
// @param c Character to append
// @return non-zero value and deallocate all memory on failed reallocation, 0 otherwise
static int arr_append(Array *arr, char c);

/*
 * Input string handlers
 */

// Skip all characters before nearest "*/".
// @param stream Input stream of characters (!= NULL)
// @return Non-zero value on EOF (unclosed comment error), 0 otherwise
static int skip_comment(FILE *stream);

// Checks if {@code a} is from {a..z + A..Z + 0..9 + '_' + '$'}
// @param a Character to check
// @return False on failed check, true otherwise
static bool is_alnum(int a);

// Checks if {@code a} is from {0..9}
// @param a Character to check
// @return False on failed check, true otherwise
static bool is_digit(int a);

// Read all characters from {@code stream} that meet {@code condition} function and append them to {@code arr}.
// @param arr Output dynamic array (!= NULL)
// @param condition Check function (!= NULL)
// @param a First symbol of a string
// @param stream Input stream of characters (!= NULL)
// @return Non-zero value of failed appending, 0 otherwise
static int read_characters(Array *arr, bool (*condition)(), int a, FILE *stream);

/*
 * Token operations
 */

// Read (composite) identifier tag from {@code stream} and store it to {@code tag}.
// If a tag matches one of reserved strings, it is deallocated and a type of reserved literal is returned.
// @param tag String to store input tag (!= NULL)
// @param a First character of identifier
// @param stream Input stream of characters (!= NULL)
// @return one of token_type if any error occures, valid type of accepted lexeme otherwise
static int read_identifier(char **tag, int a, FILE *stream);

// Read exponent part from {@code stream} using dynamic {@code arr}; on success, store string in {@code tag}.
// @param arr Dynamic array for (potentially infinite) input (!= NULL)
// @param tag Output string (!= NULL)
// @param strem Input stream of characters (!= NULL)
// @return one of token_type if any error occures and DOUBLE_VALUE otherwise
static int read_exponent(Array *arr, char **tag, FILE *stream);

// Read number literal from {@code stream}; on success, store string in {@code tag}.
// @param tag Output string (!= NULL)
// @param a First character of a string
// @param stream Input stream of characters (!= NULL)
// @return one of token_type if any error occures, valid type of lexeme otherwise
static int read_number(char **tag, int a, FILE *stream);

// Read a string enclosed by '\"' characters from {@code stream} and store it to {@code tag}.
// @param tag Output string (!= NULL)
// @param stream Input stream of characters (!= NULL)
// @return Non-zero value on failed appending, 0 otherwise
static int read_string(char **tag, FILE *stream);

/***************
 * DEFINITIONS *
 ***************/

/*
 * Dynamic array operations
 */

static Array *arr_ctor(void) {
	/* Initial capacity of a dynamic array (4B). */
	const unsigned INITIAL_CAPACITY = 32;
	/* Output array structure */
	Array *arr;

	// Allocate structure
	arr = malloc(sizeof(Array));
	if(!arr) {
		return NULL;
	}

	// Allocate array itself
	arr->str = malloc(INITIAL_CAPACITY);
	if(!arr->str) {
		free(arr);
		return NULL;
	}
	
	// Build array structure
	arr->cap = INITIAL_CAPACITY;
	arr->size = 0;

	return arr;
}

static void arr_dtor(Array *arr) {
	free(arr->str);
	free(arr);
}

static int arr_append(Array *arr, char c) {
	// Check if array is full
	if(arr->size == arr->cap) {
		// Array is full: reallocate
		char *ptr = realloc(arr->str, arr->cap * 2);
		if(!ptr) {
			// Reallocation fail: recover
			arr_dtor(arr);
			return ALLOCATION_FAILURE;
		}

		// Reallocation success
		arr->cap *= 2;
		arr->str = ptr;
	}

	// Array is not full: append
	arr->str[arr->size] = c;
	arr->size++;

	return 0;
}

/*
 *  Input string handlers
 */

static int skip_comment(FILE *stream) {
	/* Input character */
	int a;
	/* Indicates if the last character was '*' */
	bool flag = false;

	// Read input
	while((a = fgetc(stream)) != EOF) {
		if(flag) {
			// Last character was '*'
			if(a == '/') {
				// Comment is closed
				return 0;
			}
			else if(a != '*') {
				flag = false;
			}
		}
		else {
			// Last character was not '*'
			if(a == '*') {
				flag = true;
			}
		}
	}

	// EOF occured: unclosed comment error
	return UNCLOSED_COMMENT;
}

static bool is_alnum(int a) {
	return isalnum(a) || a == '_' || a == '$';
}

static bool is_digit(int a) {
	return isdigit(a);
}

static int read_characters(Array *arr, bool (*check)(), int a, FILE *stream) {
	// Read all consecutive digits and construct an array
	do {
		append(arr, (char) a);
		a = fgetc(stream);
	} while (check(a));

	// Return last character back
	ungetc(a, stream);

	return 0;
}

/*
 * Token operations 
 */

static int read_identifier(char **tag, int a, FILE *stream) {
	/* Dynamic array to store input tag */
	Array *arr;
	/* List of reserved literals in IFJ16 */
	const char *reserved[] = {
		"boolean", "break", "class", "continue", "do",
		"double", "else", "false", "for", "if", "int",
		"return", "String", "static", "true", "void", "while"
	};

	// Create dynamic array
	arr = arr_ctor();
	if(!arr) {
		return ALLOCATION_FAILURE;
	}

	// Read alphanumeric characters and construct an array
	if(read_characters(arr, is_alnum, a, stream)) {
		return ALLOCATION_FAILURE;
	}
	
	// Simple identifier is processed, check next symbol
	if((a = fgetc(stream)) == '.') {
		// Composite identifier? Append '.' and check next character
		append(arr, '.');
		a = fgetc(stream);
		if(isalpha(a) || a == '_' || a == '$') {
			// Composite identifier: read and construct suffix
			if(read_characters(arr, is_alnum, a, stream)) {
				return ALLOCATION_FAILURE;
			}
		}
		else {
			//Invalid identifier
			arr_dtor(arr);
			return INVALID_IDENTIFIER;
		}
	}
	else {
		// Return character back
		ungetc(a, stream);
	}
	
	// Finish the string by '\0'
	append(arr, '\0');
	
	// Scan through all 17 reserved literals and check if the literal is reserved
	for(int i = 0; i < 17; i++) {
		if(!strcmp(arr->str, reserved[i])) {
			// String is reserved
			arr_dtor(arr);
			return BOOLEAN + i;
		}
	}

	// Build output
	*tag = arr->str;
	free(arr);

	return IDENTIFIER;
}

static int read_exponent(Array *arr, char **tag, FILE *stream) {
	/* Input symbol */
	int a;

	// Append 'E'
	append(arr, 'E');

	// Append sign
	a = fgetc(stream);
	if(a == '+' || a == '-') {
		// Explicit sign
		append(arr, (char) a);
	}
	else {
		// Implicit '+'
		ungetc(a, stream);
		append(arr, '+');
	}

	a = fgetc(stream);
	if(isdigit(a)) {
		// Read all digits
		if(read_characters(arr, is_digit, a, stream)) {
			return ALLOCATION_FAILURE;
		}
	}
	else {
		// Invalid number literal
		arr_dtor(arr);
		return INVALID_NUMBER;
	}
	
	// Build output
	append(arr, '\0');
	*tag = arr->str;
	free(arr);

	return DOUBLE_LITERAL;
}

static int read_number(char **tag, int a, FILE *stream) {
	/* Dynamic array to store input tag */
	Array *arr;

	// Create dynamic array
	arr = arr_ctor();
	if(!arr) {
		return ALLOCATION_FAILURE;
	}

	// Read input stream and construct the number literal
	if(read_characters(arr, is_digit, a, stream)) {
		return ALLOCATION_FAILURE;
	}

	// Integer part is processed, check suffix
	a = fgetc(stream);
	if(a == '.') {
		// Double literal? Append '.' and check next character
		append(arr, '.');
		a = fgetc(stream);
		if(isdigit(a)) {
			// Double literal: read all digits
			if(read_characters(arr, is_digit, a, stream)) {
				return ALLOCATION_FAILURE;
			}

			// int.int is processed, check exponent part
			a = fgetc(stream);
			if(a == 'e' || a == 'E') {
				return read_exponent(arr, tag, stream);
			}
			else {
				// Double literal: build output
				ungetc(a, stream);
				append(arr, '\0');
				*tag = arr->str;
				free(arr);

				return DOUBLE_LITERAL;
			}
		}
		else {
			// Invalid number literal
			arr_dtor(arr);
			return INVALID_NUMBER;
		}
	}
	else if(a == 'e' || a == 'E') {
		return read_exponent(arr, tag, stream);
	}
	else {
		// Return character back to stream
		ungetc(a, stream);
	}

	// Integer literal: build output
	append(arr, '\0');
	*tag = arr->str;
	free(arr);

	return INTEGER_LITERAL;	
}

int read_string(char **tag, FILE *stream) {
	/* Dynamic array to store input tag */
	Array *arr;
	/* Input symbol */
	int a;

	// Create dynamic array
	arr = arr_ctor();
	if(!arr) {
		return ALLOCATION_FAILURE;
	}

	// Read string literal (first '\"' was alredy processed)
	while((a = fgetc(stream)) != EOF) {
		if(a == '\"') {
			// End of string literal
			append(arr, '\0');

			// Build output and clean up
			*tag = arr->str;
			free(arr);

			return STRING_LITERAL;
		}
		else if(a == '\\') {
			// {'\"', '\n', '\t', '\\', '\ddd'}: check next character
			a = fgetc(stream);
			if(a == '\"') {
				append(arr, '\"');
				continue;
			}
			else if(a == 'n') {
				append(arr, '\n');
				continue;
			}
			else if(a == 't') {
				append(arr, '\t');
				continue;
			}
			else if(a >= '0' && a <= '3') {
				// Octal number expected, process 1st digit
				int num = (a - '0') * 8 * 8;

				// Process 2nd digit
				a = fgetc(stream);
				if(a >= '0' && a <= '7') {
					num += (a - '0') * 8;

					// Process 3rd digit
					a = fgetc(stream);
					if(a >= '0' && a <= '7') {
						num += (a - '0');
						append(arr, (char) num);

						// Octal number processed
						continue;
					}
				}
			}

			// Invalid '\\' usage
			arr_dtor(arr);
			return INVALID_STRING;
		}
		else if(a < ' ') {
			//Invalid character
			arr_dtor(arr);
			return INVALID_CHARACTER;
		}
		else {
			// Regular character: append
			append(arr, (char) a);
		}
	}

	// EOF occured: unclosed string
	arr_dtor(arr);
	return UNCLOSED_STRING;
}

Token get_token(FILE *stream) {
	assert(stream);

	/* Input symbol */
	int a;
	/* Output token */
	Token t;

	// Initialise token
	t.type = EOF;
	t.tag = NULL;

	// Read input
	while((a = fgetc(stream)) != EOF) {
		// Skip whitespace characters
		if(isspace(a)) {
			do {
				a = fgetc(stream);
			} while(isspace(a));
			ungetc(a, stream);
			continue;
		}

		// Non-whitespace character
		if(isalpha(a) || a == '_' || a == '$') {
			// Identifier or reserved literal
			t.type = read_identifier(&t.tag, a, stream);
			return t;
		}

		// Neither identifier nor reserved literal
		if(isdigit(a)) {
			// Numerical literal
			t.type = read_number(&t.tag, a, stream);
			return t;
		}

		// Neither identifier nor reserved/numerical literal
		switch(a) {
			case ',': t.type = COMMA; return t;
			case ';': t.type = SEMICOLON; return t;
			case '(': t.type = LEFT_BRACKET; return t;
			case ')': t.type = RIGHT_BRACKET; return t;
			case '{': t.type = LEFT_BRACE; return t;
			case '}': t.type = RIGHT_BRACE; return t;
			case '+': t.type = PLUS; return t;
			case '-': t.type = MINUS; return t;
			case '*': t.type = ASTERISK; return t;
			case '/':
				// {//, /*, /}: check next character
				a = fgetc(stream);
				if(a == '/') {
					// Single line comment: skip all characters before EOL
					do {
						a = fgetc(stream);
					} while(a != '\n');
					continue;
				}
				else if(a == '*') {
					// Multiline comment: skip all characters before "*/"
					if(skip_comment(stream)) {
						// Unclosed comment error occured
						t.type = UNCLOSED_COMMENT;
						return t;
					}
					continue;
				}
				else {
					// None of the above: regular '/'
					ungetc(a, stream);
					t.type = SLASH;
					return t;
				}
			case '=':
				// {==, =}
				if((a = fgetc(stream)) == '=')
					t.type = EQUAL;
				else {
					ungetc(a, stream);
					t.type = ASSIGNMENT;
				}
				return t;
			case '!':
				// {!=}
				if((a = fgetc(stream)) == '=')
					t.type = NOT_EQUAL;
				else {
					// Single '!' is invalid
					ungetc(a, stream);
					t.type = INVALID_CHARACTER;
				}
				return t;
			case '<':
				// {<=, <}
				if((a = fgetc(stream)) == '=')
					t.type = LESS_OR_EQUAL;
				else {
					ungetc(a, stream);
					t.type = LESS;
				}
				return t;
			case '>':
				// {>=, >}
				if((a = fgetc(stream)) == '=')
					t.type = GREATER_OR_EQUAL;
				else {
					ungetc(a, stream);
					t.type = GREATER;
				}
				return t;
			case '\"':
				// Start of a string
				t.type = read_string(&t.tag, stream);
				return t;
			default:
				// None of the above
				t.type = INVALID_CHARACTER;
				return t;
		}
	}

	// Out of loop: EOF occured
	return t;
}
