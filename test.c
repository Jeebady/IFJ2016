/*
 * Project: IFJ16 language interpret
 * File: test.c
 * Description: Current implementation testbench
 * Author: xpalko06
 * Last edit: 15/10/16
 */

#include "scanner.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

void main(int argc, char *argv[]) {
	//Currently testing parser functionality
	Node root = parser(argv[1]);
	//Test to be added
}

