/*
 * Project: IFJ16 language interpret
 * File: parser.c
 * Description: Syntax analysis implementation
 * Author: Jakub Palkovič xpalko06
 * Last edit: 05/11/16
 */

#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "parser.h"
#include "string.h"

Token *tk;
Node *root;
Node * getNode(NodeType);
Node * root();
Node * class();
Node * mclass();
Node * functions();
Node * function();
Node * mfunction();
Node * functbody();
Node * fvar();
Node * rvar();
Node * svar();
Node * var();
Node * type();
Node * rettype();
Node * block();
Node * stat();
Node * mstats();
Node * iff();
Node * ifexec();
Node * mifexec();
Node * conds();
Node * cond();
Node * expr();
Node * as();
Node * s();
Node * v();
Node * whilef();
Node * dof();
Node * forf();
Node * for1();
Node * for2();
Node * forvar();

token_type getTokenType(Token *tok)
{
    return tok->type;
}

void str(char* strd, char stro[]){
	strd = malloc(strlen(stro)+1);
	strcpy(strd, stro);
}

Node *parser(char* file) {
    loadSourceFile(file);
    tk = getNextToken();
    root = program();
    if (getTokenType(tk) == END_OF_FILE){
        printf("Parse OK! \n");
        closeSourceFile();
        destroyToken(tk);
    }
    else {
        exit(2);
    }
    return root;
}

Node *getNode(NodeType nodeType) {
	Node *node;
	node = (Node *) malloc(sizeof(Node));
	node->nodeType = nodeType;
	node->link1 = node->link2 = NULL;
	return node;
}

void removeNode(Node *node)
{
    if(node != NULL){
        if(node->value != NULL) free(node->value);
        free(node);
    }
}

void disposeTree(Node *node)
{
    if (node != NULL) 
	{
    	if(node->link1 != NULL)
			disposeTree(node->link1);
		if(node->link2 != NULL)
            disposeTree(node->link2);
		if((node->link2 == NULL) && (node->link1 == NULL))
            removeNode(node);
    }	
}

Node * program(){
    Node *node = getNode(programNode);
	node->link1 = class();
    node->link2 = mfunction();
    return node;
}

Node * function(){
    Node *node = getNode(functionNode);
    //Function return type
	node->link1 = rettype();
    //Function ID check - save in value
    if (getTokenType(tk) == ID || MAIN) {
		node->value = tk->attr;
		destroyToken(tk); tk = getNextToken();
        node->link2 = functbody();
        return node;
	} else {
		exit(2);
	}
}

Node * functbody(){
    Node *node = getNode(functbodyNode);
    //Function Bracket after varaibles check
    if (getTokenType(tk) == LEFT_BRACKET) {
        node->link1 = fvar();
        if (getTokenType(tk) == RIGHT_BRACKET) {
        //Check if full function definiton
            destroyToken(tk); tk = getNextToken();
            if (getTokenType(tk) == LEFT_CURLY_BRACKET) 
            {
                destroyToken(tk); tk = getNextToken();
                node->link1 = block();
                //End Brace Check
                if (getTokenType(tk) == RIGHT_CURLY_BRACKET) {
                    destroyToken(tk); tk = getNextToken();
                    //Function syntax correct - return
                    return node;
                } else {
                    exit(2);
                }
            }
        // Or only pre def
        else if(getTokenType(tk) == SEMICLN) {
            return node;
        } else exit(2);
    } else exit(2);
    } else exit(2);
}

Node * mfunction()
{
    Node *node = getNode(mfunctionNode);
    switch(getTokenType(tk))
    {
    case STRING:
        node->link1 = function();
        node->link2 = mfunction();
        break;
    case INTEGER:
        node->link1 = function();
        node->link2 = mfunction();
        break;
    case DOUBLE:
        node->link1 = function();
        node->link2 = mfunction();
        break;
    default:
        break;
    }
    return node;
}

Node * type()
{
    Node *node = getNode(typeNode);
    switch(getTokenType(tk)) {
    case STRING  :
        node->value->str = "STRING";
        destroyToken(tk); tk = getNextToken();
        return node;
        break;
    case DOUBLE  :
        node->value->str = "DOUBLE";
        destroyToken(tk); tk = getNextToken();
        return node;
        break;
    case INTEGER:
        node->value->str = "INT";
        destroyToken(tk); tk = getNextToken();
        return node;
        break;
    case AUTO:
        node->value->str = "AUTO";
        destroyToken(tk); tk = getNextToken();
        return node;
        break;
    default : 
        exit(2);
    }
}

Node * rettype()
{
    Node *node = getNode(rettypeNode);
    switch(getTokenType(tk)) {
        case STRING  :
            node->value->str = "STRING";
            destroyToken(tk); tk = getNextToken();
            return node;
            break;
        case INTEGER  :
            node->value->str = "INT";
            destroyToken(tk); tk = getNextToken();
            return node;
            break;
        case DOUBLE :
            node->value->str = "DOUBLE";
            destroyToken(tk); tk = getNextToken();
            return node;
            break;
        default : 
            printf("exit");
            exit(2);
    }
}

Node * block()
{
    Node *node = getNode(blockNode);
    node->link1 = var();
    node->link2 = stats();
    return node;
}

Node * var()
{
    Node *node = getNode(varNode);
    switch(getTokenType(tk)) {
    case STRING  :
        node->value->str = "STRING";
        destroyToken(tk); tk = getNextToken();
        node->link1 = id();
        destroyToken(tk); tk = getNextToken();
        break;
    case INTEGER  :
        node->value->str = "INTEGER";
        destroyToken(tk); tk = getNextToken();
        node->link1 = id();
        destroyToken(tk); tk = getNextToken();
        break;
    case DOUBLE :
        node->value->str = "DOUBLE";
        destroyToken(tk); tk = getNextToken();
        node->link1 = id();
        destroyToken(tk); tk = getNextToken();
        break;
    case AUTO :
        node->value->str = "AUTO";
        destroyToken(tk); tk = getNextToken();
        node->link1 = id();
        destroyToken(tk); tk = getNextToken();
        break;
    default:
        return node;
    }
    if(getTokenType(tk) == SEMICLN)
    {
        destroyToken(tk); tk = getNextToken();
        node->link2 = var();
        return node;
    } else exit(2);
}

Node * assign()
{
    Node *node = getNode(assignNode);
    if(getTokenType(tk) == ASSIGNMENT)
    {
        destroyToken(tk); tk = getNextToken();
        node->link1 = expr();
    } else exit(2);
    return node;
}

Node * stats()
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

Node * mstats()
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

Node * stat()
{
    Node *node = getNode(statNode);
        switch(getTokenType(tk)) {
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
            if(getTokenType(tk)==LEFT_BRACKET)
            {
                node->link1 = rvar();
                if (getTokenType(tk) == RIGHT_BRACKET) {
                    destroyToken(tk); tk = getNextToken();
                } else {
                    exit(2);
                }
            }
            else
            {
                destroyToken(tk); tk = getNextToken();
                node->link1 = assign();
            }
            if (getTokenType(tk) == SEMICLN) {
                destroyToken(tk); tk = getNextToken();
            } else {
                exit(2);
            }
        break;
        case RETURN:
            node->value->str = "RETURN";
            //Return expression
            destroyToken(tk); tk = getNextToken();
            node->link1 = expr();
            //SEMICLN check
            if (getTokenType(tk) == SEMICLN) {
                destroyToken(tk); tk = getNextToken();
            } else {
                exit(2);
            }
        break;
        default:
            return node;
        break;
    }
    return node;
}

Node * cinf()
{
    Node *node = getNode(cinfNode);
    destroyToken(tk); tk = getNextToken();
    if(getTokenType(tk) == IN)
    {
            destroyToken(tk); tk = getNextToken();
            if(getTokenType(tk) == ID)
            {
                node->link1 = id();
                node->link2 = mcinf();
                return node;
            } else exit(2);
        } else exit(2);
}

Node * mcinf()
{
    Node *node = getNode(mcinfNode);
    destroyToken(tk); tk = getNextToken();
    if(getTokenType(tk) == IN)
    {
            destroyToken(tk); tk = getNextToken();
            if(getTokenType(tk) == ID)
            {
                node->link1 = id();
                node->link2 = mcinf();
                return node;
            } else exit(2);
    }
    else if(getTokenType(tk) == SEMICLN)
    {
        destroyToken(tk); tk = getNextToken();
        return node;
    }
    else exit(2);
}

Node * coutf()
{
    Node *node = getNode(coutfNode);
    destroyToken(tk); tk = getNextToken();
    if(getTokenType(tk) == OUT)
    {
            destroyToken(tk); tk = getNextToken();
            node->link1 = expr();
            node->link2 = mcoutf();
            return node;
        } else exit(2);
}

Node * mcoutf()
{
    Node *node = getNode(mcoutfNode);
    if(getTokenType(tk) == OUT)
    {
            destroyToken(tk); tk = getNextToken();
            node->link1 = expr();
            node->link2 = mcoutf();
            return node;
    }
    else if(getTokenType(tk) == SEMICLN)
    {
        destroyToken(tk); tk = getNextToken();
        return node;
    }
    else exit(2);
}

Node * iff()
{
    Node *node = getNode(iffNode);
    destroyToken(tk); tk = getNextToken();
    if(getTokenType(tk) == LEFT_BRACKET)
    {
        node->link1 = ro();
    } else 
    {
        exit(2);
    }
    if(getTokenType(tk) == RIGHT_BRACKET)
    {
        node->link2 = elsef();
        destroyToken(tk); tk = getNextToken();
        return node;
    } else 
    {
        exit(2);
    }
}

Node * whilef()
{
    Node *node = getNode(whilefNode);
    destroyToken(tk); tk = getNextToken();
    if(getTokenType(tk) == LEFT_BRACKET)
    {
        node->link1 = ro();
    }
    destroyToken(tk); tk = getNextToken();
    if(getTokenType(tk) == RIGHT_BRACKET)
    {
        destroyToken(tk); tk = getNextToken();
        if(getTokenType(tk) == LEFT_CURLY_BRACKET)
        {
            node->link1 = block();
        }
        destroyToken(tk); tk = getNextToken();
        if(getTokenType(tk) == RIGHT_CURLY_BRACKET)
        {
            return node;
        } else exit(2);
    } else exit(2);
}

Node * forf()
{
    Node *node = getNode(forfNode);
    destroyToken(tk); tk = getNextToken();
    if(getTokenType(tk) == LEFT_BRACKET)
    {
        destroyToken(tk); tk = getNextToken();
        node->link1 = for1();
    } else exit(2);
    node->link2 = for2();
    return node;
}
Node * for1()
{
    Node *node = getNode(for1Node);
    node->link2 = forvar();
    if(getTokenType(tk) == SEMICLN)
    {
        node->link1 = ro();
        if(getTokenType(tk) == SEMICLN)
        {
            return node;
        } else exit(2);
    } else exit(2);
}

Node * for2()
{
    Node *node = getNode(for2Node);
    destroyToken(tk); tk = getNextToken();
    if(getTokenType(tk) == ID)
    {
        node->value = tk->attr;
        destroyToken(tk); tk = getNextToken();
        node->link1 = assign();
        if(getTokenType(tk) == RIGHT_BRACKET)
        {
            destroyToken(tk); tk = getNextToken();
            if(getTokenType(tk) == LEFT_CURLY_BRACKET)
            {
                destroyToken(tk); tk = getNextToken();
                node->link2 = block();
                if(getTokenType(tk) == RIGHT_CURLY_BRACKET)
                {
                    destroyToken(tk); tk = getNextToken();
                    return node;
                } else exit(2);
            } else exit(2);
        } else exit(2);
    } else exit(2);
}

Node * forvar()
{
    Node *node = getNode(forvarNode);
    node->link1 = type();
    if(getTokenType(tk) == ID)
    {
        node->value = tk->attr;
        destroyToken(tk); tk = getNextToken();
        if(getTokenType(tk) == ASSIGNMENT)
        {
            node->link2 = assign();
        }
    } else exit(2);
    return node;
}

Node * expr()
{
    Node *node = getNode(exprNode);
    node->link1 = as();
    if(getTokenType(tk) != SEMICLN)
    {
        if(getTokenType(tk) == ADD)
        {
            node->value->str = "ADDITION";
            destroyToken(tk); tk = getNextToken();
            node->link2 = expr();
        }
        else
        if(getTokenType(tk) == SUB)
        {
            node->value->str = "SUBTRACTION";
            destroyToken(tk); tk = getNextToken();
            node->link2 = expr();
        }
        return node;
    } else return node;
}

Node * as()
{
    Node *node = getNode(asNode);
    if(getTokenType(tk) == LEFT_BRACKET)
    {
        node->value->str = "BRACKET";
        destroyToken(tk); tk = getNextToken();
        node->link2 = expr();
        return node;
    }
    else
    {
        node->link1 = s();
    }
    if(getTokenType(tk) == MULTIPLICATION)
    {
        node->value->str = "MULTIPLICATION";
        destroyToken(tk); tk = getNextToken();
        node->link2 = as();
    }
    else
    if(getTokenType(tk) == DIVISION)
    {
        printf("div\n");
        node->value->str = "DIVISION";
        destroyToken(tk); tk = getNextToken();
        node->link2 = as();
    }
    return node;
}

Node * s()
{
    Node *node = getNode(sNode);
    if(getTokenType(tk) == SUB)
    {
        node->value->str = "MINUS";
        node->link1 = s();
        destroyToken(tk); tk = getNextToken();
    }
    else 
    {
        node->value->str = "PLUS";
        node->link1 = v();
    }
    return node;
}

Node * v()
{
    Node *node = getNode(vNode);
    switch(getTokenType(tk)) {
        case LEFT_BRACKET:
            node->link1 = expr();
            if (getTokenType(tk) == RIGHT_BRACKET) {
                destroyToken(tk); tk = getNextToken();
                return node;
            } else {
                exit(2);
            }
        case ID:
            node->link1 = id();
            destroyToken(tk); tk = getNextToken();
            if(getTokenType(tk)==LEFT_BRACKET)
            {
                node->link2 = rvar();
                destroyToken(tk); tk = getNextToken();
                if (getTokenType(tk) == RIGHT_BRACKET) {
                    return node;
                } else {
                    exit(2);
                }
            } else{
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
            exit(2);
    }
    return node;
}

Node * ro()
{
    Node *node = getNode(roNode);
    destroyToken(tk); tk = getNextToken();
    node->link1 = expr();
    switch(getTokenType(tk)) {
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
            exit(2);
    }
    destroyToken(tk); tk = getNextToken();
    node->link2 = expr();
    return node;
}

Node * elsef()
{
    Node *node = getNode(elseNode);
    destroyToken(tk); tk = getNextToken();
    if(getTokenType(tk) == LEFT_CURLY_BRACKET)
    {
        destroyToken(tk); tk = getNextToken();
        node->link1 = block();
        if(getTokenType(tk) == RIGHT_CURLY_BRACKET)
        {
            destroyToken(tk); tk = getNextToken();
            if(getTokenType(tk) == ELSE)
            {
                destroyToken(tk); tk = getNextToken();
                if(getTokenType(tk) == LEFT_CURLY_BRACKET)
                {
                    destroyToken(tk); tk = getNextToken();
                    node->link2 = block();
                    if(getTokenType(tk) == RIGHT_CURLY_BRACKET)
                    {
                         return node;
                    } else exit(2);
                } else exit(2);
            } else return node;
        } else exit(2);
    } else exit(2);
}

Node * id()
{
    Node *node = getNode(idNode);
    node->value = tk->attr;
    return node;
}

Node * fvar()
{
    Node *node = getNode(fvarNode);
    destroyToken(tk); tk = getNextToken();
    node->link1 = rettype();
    if(getTokenType(tk) == ID)
    {
        node->value = tk->attr;
        destroyToken(tk); tk = getNextToken();
        if(getTokenType(tk) == COMMA)
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

Node * rvar()
{
    Node *node = getNode(rvarNode);
    if(getTokenType(tk) != RIGHT_BRACKET)
    {
        node->link1 = expr();
        if(getTokenType(tk) == COMMA)
        {
            node->link2 = rvar();
        }
        else
        {
            exit(2);
        }
    }
    return node;
}

Node * intf()
{
    Node *node = getNode(intNode);
    node->value = tk->attr;
    destroyToken(tk); tk = getNextToken();
    return node;
}

Node * stringf()
{
    Node *node = getNode(stringNode);
    node->value = tk->attr;
    destroyToken(tk); tk = getNextToken();
    if(getTokenType(tk) == SEMICLN)
    {
        return node;
    }else
    {
        exit(2);
    }
}

Node * doublef()
{
    Node *node = getNode(doubleNode);
    node->value = tk->attr;
    destroyToken(tk); tk = getNextToken();
    return node;
}

Node * autof()
{
    Node *node = getNode(autoNode);
    node->value = tk->attr;
    destroyToken(tk); tk = getNextToken();
    return node;
}

int main(int argc, char* argv[])
{
    printf("%s\n", argv[0]);
    if (argc == 2)
    {
        printf("%s\n", argv[0]);
        root = parser(argv[1]);
        disposeTree(root);
    }else exit(1);
}
