/*
 * Project: IFJ16 language interpret
 * File: parser.c
 * Description: Syntax analysis implementation
 * Author: Jakub Palkovič xpalko06
 * Last edit: 05/11/16
 */

#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include <string.h>

 /*
	 Function declarations,
	 functions called for each link in a node
	 to create a tree.
 */

Node *root;
Node * getNode(NodeType);
Node * rootN(FILE *stream);
Node * classN(FILE *stream);
Node * mclassN(FILE *stream);
Node * functionsN(FILE *stream);
Node * functionN(FILE *stream);
Node * mfunctionN(FILE *stream);
Node * functbodyN(FILE *stream);
Node * fvarN(FILE *stream);
Node * rvarN(FILE *stream);
Node * svarN(FILE *stream);
Node * varN(FILE *stream);
Node * typeN(FILE *stream);
Node * rettypeN(FILE *stream);
Node * blockN(FILE *stream);
Node * statN(FILE *stream);
Node * ifN(FILE *stream);
Node * ifexecN(FILE *stream);
Node * mifexecN(FILE *stream);
Node * condsN(FILE *stream);
Node * condN(FILE *stream);
Node * exprN(FILE *stream);
Node * asN(FILE *stream);
Node * sN(FILE *stream);
Node * vN(FILE *stream);
Node * whileN(FILE *stream);
Node * doN(FILE *stream);
Node * forN(FILE *stream);
Node * for1N(FILE *stream);
Node * for2N(FILE *stream);
Node * forvarN(FILE *stream);

/*	Global variable declarations	*/
Token tk;

/*
	Initial entry into parser
	Takes
*/
Node *parser(FILE *stream) {
	assert(stream);
	root = rootN(stream);
	if (tk.type == EOF) {
		closeSourceFile();
		return root;
	}
	else {
		end(2);
	}
}

Node *getNode(NodeType nodeType) {
	Node *node;
	node = (Node *)malloc(sizeof(Node));
	if (node) {
		node->nodeType = nodeType;
		node->link1 = node->link2 = node->value = NULL;
		return node;
	}
	else end(99);
}

void removeNode(Node *node)
{
	if (node != NULL) {
		if (node->value != NULL) free(node->value);
		free(node);
	}
}

void disposeTree(Node *node)
{
	if (node != NULL)
	{
		if (node->link1 != NULL) {
			disposeTree(node->link2);
			node->link1 = NULL;
		}
		if (node->link2 != NULL) {
			disposeTree(node->link2);
			node->link1 != NULL;
		}
		if ((node->link2 == NULL) && (node->link1 == NULL))
			removeNode(node);
	}
}

void end(int exitCode) {
	disposeTree(root);
	exit(exitCode);
}

Node * rootN(FILE *stream) {
	Node *node = getNode(rootNode);
	tk = get_token(stream);
	node->link1 = class(stream);
	node->link2 = mclass(stream);
	return node;
}

Node * classN(FILE *stream) {
	Node *node = getNode(classNode);
	if (tk.type == CLASS) {
		if ((tk = get_token(stream)).type == IDENTIFIER)
		{
			node->value = tk.tag;
			if ((tk = get_token(stream)).type == LEFT_BRACE) {
				node->link1 = svarN(stream);
				node->link2 = functionsN(stream);
				return node;
			}
		}
		else end(2);
	}
	else end(2);
}

Node * mclassN(FILE *stream) {
	if (tk.type != EOF) {
		Node *node = getNode(mclassNode);
		node->link1 = classN(stream);
		node->link2 = mclassN(stream);
		return node;
	}
}

Node * svarN(FILE *stream) {
	Node *node;
	if ((tk = get_token(stream)).type == STATIC) {
		node = getNode(svarNode);
		node->link1 = varN(stream);
		if (tk.type == SEMICOLON) {
			node->link2 = svarN(stream);
			return node;
		}
		else end(2);
	}
	else return NULL;
}

Node * functionsN(FILE *stream) {
	Node *node = getNode(functionsNode);
	if ((tk = get_token(stream)).type == STATIC) {
		node->link1 = functionN(stream);
		node->link2 = mfunctionN(stream);
		return node;
	}
	else end(2);
}

Node * functionN(FILE *stream) {
	if ((tk = get_token(stream)).type == STATIC) {
		Node *node = getNode(functionNode);
		node->link1 = rettypeN(stream);
		if ((tk = get_token(stream)).type == IDENTIFIER) {
			node->value = tk.tag;
			node->link2 = functbodyN(stream);
			return node;
		}
		else end(2);
	}
}

Node * mfunctionN(FILE *stream) {
	if ((tk = get_token(stream)).type == STATIC) {
		Node *node = getNode(mfunctionNode);
		node->link1 = functionN(stream);
		node->link2 = mfunctionN(stream);
	}
	else return NULL;
}

Node * functbodyN(FILE *stream) {
	Node *node = getNode(functbodyNode);
	//Function Bracket after varaibles check
	if ((tk = get_token(stream)).type == LEFT_BRACKET) {
		node->link1 = fvarN(stream);
		if ((tk = get_token(stream)).type == RIGHT_BRACKET) {
			if ((tk = get_token(stream)).type == LEFT_BRACE) {
				node->link1 = blockN(stream);
				//End Brace Check
				if ((tk = get_token(stream)).type == RIGHT_BRACE) {
					//Function syntax correct - return
					return node;
				}
				else end(2);
			}
			else end(2);
		}
		else end(2);
	}
	else end(2);
}

Node * typeN(FILE *stream)
{
	Node *node = getNode(typeNode);
	tk = get_token(stream);
	switch (tk.type) {
	case STRING:
		node->value = "STRING";
		return node;
	case DOUBLE:
		node->value = "DOUBLE";
		return node;
	case INT:
		node->value = "INT";
		return node;
	case BOOLEAN:
		node->value = "BOOLEAN";
		return node;
	default:
		end(2);
	}
}

Node * rettypeN(FILE *stream)
{
	Node *node = getNode(rettypeNode);
	tk = get_token(stream);
	switch (tk.type) {
	case STRING:
		node->value = "STRING";
		return node;
	case INT:
		node->value = "INT";
		return node;
	case DOUBLE:
		node->value = "DOUBLE";
		return node;
	case BOOLEAN:
		node->value = "DOUBLE";
		return node;
	case VOID:
		node->value = "DOUBLE";
		return node;
	default:
		end(2);
	}
}

Node * varN(FILE *stream)
{
	Node *node = getNode(varNode);
	node->link1 = typeN(stream);
	if ((tk = get_token(stream)).type == IDENTIFIER)
	{
		if ((tk = get_token(stream)).type == EQUAL) {
			node->link2 = exprN(stream);
		}
		return node;
	}
	else end(2);
}

Node * blockN(FILE *stream) {
	Node *node = getNode(blockNode);
	node->link1 = statN(stream);
	node->link2 = mstatsN(stream);
	return node;
}

Node * mstatsN(FILE *stream) {
	Node *node = getNode(mstatsNode);
	node->link1 = statN(stream);
	node->link2 = mstatsN(stream);
	return node;
}

Node * assign(FILE *stream)
{
	Node *node = getNode(assignNode);
	if (getTokenType(tk) == ASSIGNMENT)
	{
		 tk = getNextToken();
		node->link1 = expr();
	}
	else end(2);
	return node;
}

Node * stats(FILE *stream)
{
	Node *node = getNode(statsNode);
	node->link1 = stat();
	if (getTokenType(tk) == RIGHT_CURLY_BRACKET) {
		return node;
	}
	else
	{
		node->link2 = mstats();
	}
	return node;
}

Node * mstats(FILE *stream)
{
	Node *node = getNode(mstatsNode);
	node->link1 = stat();
	if (getTokenType(tk) == RIGHT_CURLY_BRACKET) {
		return node;
	}
	else
	{
		node->link2 = mstats();
	}
	return node;
}

Node * stat(FILE *stream)
{
	Node *node = getNode(statNode);
	switch (getTokenType(tk)) {
	case CIN:
		node->value->str = "CIN";
		node->link1 = cinf();
		break;
	case COUT:
		node->value->str = "COUT";
		node->link1 = coutf();
		break;
	case IF:
		node->value->str = "IF";
		node->link1 = iff();
		break;
	case WHILE:
		node->value->str = "WHILE";
		node->link1 = whilef();
		break;
	case FOR:
		node->value->str = "FOR";
		node->link1 = forf();
		break;
	case ID:
		node->value = tk->attr;
		if (getTokenType(tk) == LEFT_BRACKET)
		{
			node->link1 = rvar();
			if (getTokenType(tk) == RIGHT_BRACKET) {
				 tk = getNextToken();
			}
			else {
				end(2);
			}
		}
		else
		{
			 tk = getNextToken();
			node->link1 = assign();
		}
		if (getTokenType(tk) == SEMICLN) {
			 tk = getNextToken();
		}
		else {
			end(2);
		}
		break;
	case RETURN:
		node->value->str = "RETURN";
		//Return expression
		 tk = getNextToken();
		node->link1 = expr();
		//SEMICLN check
		if (getTokenType(tk) == SEMICLN) {
			 tk = getNextToken();
		}
		else {
			end(2);
		}
		break;
	default:
		return node;
		break;
	}
	return node;
}

Node * iff(FILE *stream)
{
	Node *node = getNode(iffNode);
	 tk = getNextToken();
	if (getTokenType(tk) == LEFT_BRACKET)
	{
		node->link1 = ro();
	}
	else
	{
		end(2);
	}
	if (getTokenType(tk) == RIGHT_BRACKET)
	{
		node->link2 = elsef();
		 tk = getNextToken();
		return node;
	}
	else
	{
		end(2);
	}
}

Node * whilef(FILE *stream)
{
	Node *node = getNode(whilefNode);
	 tk = getNextToken();
	if (getTokenType(tk) == LEFT_BRACKET)
	{
		node->link1 = ro();
	}
	 tk = getNextToken();
	if (getTokenType(tk) == RIGHT_BRACKET)
	{
		 tk = getNextToken();
		if (getTokenType(tk) == LEFT_CURLY_BRACKET)
		{
			node->link1 = block();
		}
		 tk = getNextToken();
		if (getTokenType(tk) == RIGHT_CURLY_BRACKET)
		{
			return node;
		}
		else end(2);
	}
	else end(2);
}

Node * forf(FILE *stream)
{
	Node *node = getNode(forfNode);
	 tk = getNextToken();
	if (getTokenType(tk) == LEFT_BRACKET)
	{
		 tk = getNextToken();
		node->link1 = for1();
	}
	else end(2);
	node->link2 = for2();
	return node;
}

Node * for1(FILE *stream)
{
	Node *node = getNode(for1Node);
	node->link2 = forvar();
	if (getTokenType(tk) == SEMICLN)
	{
		node->link1 = ro();
		if (getTokenType(tk) == SEMICLN)
		{
			return node;
		}
		else end(2);
	}
	else end(2);
}

Node * for2(FILE *stream)
{
	Node *node = getNode(for2Node);
	 tk = getNextToken();
	if (getTokenType(tk) == ID)
	{
		node->value = tk->attr;
		 tk = getNextToken();
		node->link1 = assign();
		if (getTokenType(tk) == RIGHT_BRACKET)
		{
			 tk = getNextToken();
			if (getTokenType(tk) == LEFT_CURLY_BRACKET)
			{
				 tk = getNextToken();
				node->link2 = block();
				if (getTokenType(tk) == RIGHT_CURLY_BRACKET)
				{
					 tk = getNextToken();
					return node;
				}
				else end(2);
			}
			else end(2);
		}
		else end(2);
	}
	else end(2);
}

Node * forvar(FILE *stream)
{
	Node *node = getNode(forvarNode);
	node->link1 = type();
	if (getTokenType(tk) == ID)
	{
		node->value = tk->attr;
		 tk = getNextToken();
		if (getTokenType(tk) == ASSIGNMENT)
		{
			node->link2 = assign();
		}
	}
	else end(2);
	return node;
}

Node * expr(FILE *stream)
{
	Node *node = getNode(exprNode);
	node->link1 = as();
	if (getTokenType(tk) != SEMICLN)
	{
		if (getTokenType(tk) == ADD)
		{
			node->value->str = "ADDITION";
			 tk = getNextToken();
			node->link2 = expr();
		}
		else
			if (getTokenType(tk) == SUB)
			{
				node->value->str = "SUBTRACTION";
				 tk = getNextToken();
				node->link2 = expr();
			}
		return node;
	}
	else return node;
}

Node * as(FILE *stream)
{
	Node *node = getNode(asNode);
	if (getTokenType(tk) == LEFT_BRACKET)
	{
		node->value->str = "BRACKET";
		 tk = getNextToken();
		node->link2 = expr();
		return node;
	}
	else
	{
		node->link1 = s();
	}
	if (getTokenType(tk) == MULTIPLICATION)
	{
		node->value->str = "MULTIPLICATION";
		 tk = getNextToken();
		node->link2 = as();
	}
	else
		if (getTokenType(tk) == DIVISION)
		{
			printf("div\n");
			node->value->str = "DIVISION";
			 tk = getNextToken();
			node->link2 = as();
		}
	return node;
}

Node * s(FILE *stream)
{
	Node *node = getNode(sNode);
	if (getTokenType(tk) == SUB)
	{
		node->value->str = "MINUS";
		node->link1 = s();
		 tk = getNextToken();
	}
	else
	{
		node->value->str = "PLUS";
		node->link1 = v();
	}
	return node;
}

Node * v(FILE *stream)
{
	Node *node = getNode(vNode);
	switch (getTokenType(tk)) {
	case LEFT_BRACKET:
		node->link1 = expr();
		if (getTokenType(tk) == RIGHT_BRACKET) {
			 tk = getNextToken();
			return node;
		}
		else {
			end(2);
		}
	case ID:
		node->link1 = id();
		 tk = getNextToken();
		if (getTokenType(tk) == LEFT_BRACKET)
		{
			node->link2 = rvar();
			 tk = getNextToken();
			if (getTokenType(tk) == RIGHT_BRACKET) {
				return node;
			}
			else {
				end(2);
			}
		}
		else {
			return node;
		}
		break;
	case INT:
		node->link1 = intf();
		break;
	case DBL:
		node->link1 = doublef();
		break;
	case STRING_LITERAL:
		node->link1 = stringf();
		break;
	default:
		end(2);
	}
	return node;
}

Node * ro(FILE *stream)
{
	Node *node = getNode(roNode);
	 tk = getNextToken();
	node->link1 = expr();
	switch (getTokenType(tk)) {
	case LSS:
		node->value->str = "LSS";
		break;
	case GTR:
		node->value->str = "GTR";
		break;
	case LESS_OR_EQUAL:
		node->value->str = "LESS_OR_EQUAL";
		break;
	case GREATER_OR_EQUAL:
		node->value->str = "GREATER_OR_EQUAL";
		break;
	case EQUAL:
		node->value->str = "EQUAL";
		break;
	case NOT_EQUAL:
		node->value->str = "NOT_EQUAL";
		break;
	default:
		end(2);
	}
	 tk = getNextToken();
	node->link2 = expr();
	return node;
}

Node * elsef(FILE *stream)
{
	Node *node = getNode(elseNode);
	 tk = getNextToken();
	if (getTokenType(tk) == LEFT_CURLY_BRACKET)
	{
		 tk = getNextToken();
		node->link1 = block();
		if (getTokenType(tk) == RIGHT_CURLY_BRACKET)
		{
			 tk = getNextToken();
			if (getTokenType(tk) == ELSE)
			{
				 tk = getNextToken();
				if (getTokenType(tk) == LEFT_CURLY_BRACKET)
				{
					 tk = getNextToken();
					node->link2 = block();
					if (getTokenType(tk) == RIGHT_CURLY_BRACKET)
					{
						return node;
					}
					else end(2);
				}
				else end(2);
			}
			else return node;
		}
		else end(2);
	}
	else end(2);
}

Node * id(FILE *stream)
{
	Node *node = getNode(idNode);
	node->value = tk->attr;
	return node;
}

Node * fvar(FILE *stream)
{
	Node *node = getNode(fvarNode);
	 tk = getNextToken();
	node->link1 = rettype();
	if (getTokenType(tk) == ID)
	{
		node->value = tk->attr;
		 tk = getNextToken();
		if (getTokenType(tk) == COMMA)
		{
			node->link2 = rvar();
		}
		else
		{
			return node;
		}
	}
	return node;
}

Node * rvar(FILE *stream)
{
	Node *node = getNode(rvarNode);
	if (getTokenType(tk) != RIGHT_BRACKET)
	{
		node->link1 = expr();
		if (getTokenType(tk) == COMMA)
		{
			node->link2 = rvar();
		}
		else
		{
			end(2);
		}
	}
	return node;
}

Node * intf(FILE *stream)
{
	Node *node = getNode(intNode);
	node->value = tk->attr;
	 tk = getNextToken();
	return node;
}

Node * stringf(FILE *stream)
{
	Node *node = getNode(stringNode);
	node->value = tk->attr;
	 tk = getNextToken();
	if (getTokenType(tk) == SEMICLN)
	{
		return node;
	}
	else
	{
		end(2);
	}
}

Node * doublef(FILE *stream)
{
	Node *node = getNode(doubleNode);
	node->value = tk->attr;
	 tk = getNextToken();
	return node;
}

Node * autof(FILE *stream)
{
	Node *node = getNode(autoNode);
	node->value = tk->attr;
	 tk = getNextToken();
	return node;
}