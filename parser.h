/*
 * Project: IFJ16 language interpret
 * File: parser.h
 * Description: Syntax analysis header
 * Author: Jakub Palkovič xpalko06
 * Last edit: 05/11/16
 */

#ifndef PARSER_H 
#define PARSER_H 

typedef enum {
	rootNode,	// Root container, contains classNode and mclassNode
	classNode,	// Contains svarNode and functionsNode, value is set to class ID
	mclassNode,	// Contains classNode and mclassNode
	functionsNode,	// Contains functionNode and mfunctionNode
	functionNode,	// Contains rettypeNode and functbodyNode, value is set to function ID
	mfunctionNode,	// Is empty, or contains functionNode and mfunctionNode
	functbodyNode,	// Contains fvarNode and blockNode
	fvarNode,	// Is empty, or value is set to variable ID and then contains typeNode and fvarNode
	rvarNode,	// Is empty, or contains exprNode and rvarNode
	svarNode,	// Is empty, or contains varNode and svarNode
	varNode,	// Value is set to variable ID, and contains typeNode and possible exprNode (for assignment)
	typeNode,	// Value is set to variable type
	rettypeNode,	// Value is set to return variable type
	blockNode,	// contains statNode and mstatsNode

	statNode,	/* contains either ifNode,
			   whileNode,
			   doNode,
			   forNode,
			   exprNode (value is set to ID of assigned variable),
			   rvarNode (value is set to ID of called function),
			   exprNode (expression for return statement),
			   typeNode (value is set to ID of declared variable or
			   typeNode and exprNode (value is set to ID of assigned variable)
			*/

	ifNode,		// Contains condsNode and ifexecNode
	ifexecNode	// Contains blockNode and mifexecNode
	mifexecNode	// Is empty, or contains blockNode and mifexecNode
	condsNode	/* Contains condNode,
 			   condsNode (value is set to () or !)
 			   condNode and condsNode (value is set to && or ||
			*/

	condNode	// Contains exprNode and exprNode, value is set to operator
			// (>, <, >=, <=, ==, !=)

	exprNode	/* Contains either asNode and exprNode (value is set to *, /),
			   exprNode in the case of (expr) (value is set to () ),
			   asNode (value is set to empty)
			*/

	asNode		// Contains sNode and asNode or only sNode
	sNode		// Contains sNode (when -sNode) or vNode
	vNode		/* Contains exprNode when (expr),
			   
			   is empty when variable ID is present in value,
			   is empty when numerical value is present in value
			*/
	whileNode	// Contains condsNode and blockNode
	doNode		// Contains blockNode and condsNode
	forNode		// Contains for1Node and for2Node
	for1Node	// Contains forvarNode and condNode
	for2Node	// Contains exprNode and blockNode
	forvarNode	// Contains typeNode (value is set to variable ID) or,
			// typeNode and exprNode (value is set to variable ID)
} NodeType;

/*------- TREE -------*/
struct nodeTag {
	NodeType nodeType;
	string* value;
	struct nodeTag *link1;
	struct nodeTag *link2;
} Node;

extern int numToken;
extern Token *tokens;
Node* parser(char* file);
void disposeTree(Node *node);
#endif
