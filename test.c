/*
 * Project: IFJ16 language interpret
 * File: test.c
 * Description: Current implementation testbench
 * Author: xpalko06
 * Last edit: 15/10/16
 */

#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "parser.h"

void main(int argc, char *argv[]) {
	//Currently testing parser functionality
	FILE *stream = fopen(argv[1], "r");
	Node *root;
	if (stream) root = parser(stream);
	disposeTree(root);
	//Test to be added
}