#include <stdlib.h>
#include "value.h"
#include "linkedlist.h"
#include <stdio.h>
#include "talloc.h"
#include "tokenizer.h"
#include <string.h>
#include "parser.h"

Value *stackBuilder(Value *tree, Value *token);

// Takes a list of tokens from a Scheme program, and returns a pointer to a
// parse tree representing that program.
Value *parse(Value *tokens) {
    Value *tree = makeNull();
    if(isNull(tokens)) {
        printf("Error (parse): null pointer\n");
        texit(1);
    }
    Value *cur = tokens;
    while(cur->type != NULL_TYPE) {  
        tree = stackBuilder(tree, car(cur));
        cur = cdr(cur);
    }
    cur = tree;
    while(cur->type != NULL_TYPE) {  
        if (car(cur)->type == OPEN_TYPE) {
            printf("Syntax error: No matching close parenthesis");
        }
        cur = cdr(cur);
    }
    tree = reverse(tree);
    return tree;
}

// Takes a token and a tree and adds the token to the tree, unless its a close paren in which case
// it seperates a list from the the section formed up to the corresponding open paren
Value *stackBuilder(Value *tree, Value *token) {
    if (token->type != CLOSE_TYPE) {
        Value *tok = talloc(sizeof(Value));
        switch (token->type) {
        case INT_TYPE:
            tok->i = token->i;
            tok->type = INT_TYPE;
            break;
    
        case DOUBLE_TYPE:
            tok->d = token->d;
            tok->type = DOUBLE_TYPE;
            break;
        
        case STR_TYPE:
            tok->s = talloc(sizeof(char) * (strlen(token->s) + 1));
            strcpy(tok->s, token->s);
            tok->type = STR_TYPE;
            break;

        case BOOL_TYPE:
            tok->s = talloc(sizeof(char) * (strlen(token->s) + 1));
            strcpy(tok->s, token->s);
            tok->type = BOOL_TYPE;
            break;

        case SYMBOL_TYPE:
            tok->s = talloc(sizeof(char) * (strlen(token->s) + 1));
            strcpy(tok->s, token->s);
            tok->type = SYMBOL_TYPE;
            break;

        case PTR_TYPE:
            tok->p = token->p;
            tok->type = PTR_TYPE;
            break;
        
        case OPEN_TYPE:
            tok->s = talloc(sizeof(char) * (strlen(token->s) + 1));
            strcpy(tok->s, token->s);
            tok->type = OPEN_TYPE;
            break;

        case NULL_TYPE:
            tok->type = NULL_TYPE;
            break;
        
        case CLOSE_TYPE:
            tok->type = CLOSE_TYPE;
            break;

        default:
            break;
        }
        tree = cons(tok, tree);
        return tree;
    } else {
        Value *sub = makeNull();
        Value *toAdd = tree;
        if(toAdd->type == NULL_TYPE) {
            printf("Syntax Error: No matching open parenthesis.\n");
            texit(0);
        }
        while (car(toAdd)->type != OPEN_TYPE) {
            sub = cons(car(toAdd), sub);
            toAdd = cdr(toAdd);
            if(toAdd->type == NULL_TYPE) {
                printf("Syntax Error: No matching open parenthesis.\n");
                texit(0);
            }
        }
        toAdd = cdr(toAdd);
        return tree = cons(sub, toAdd);
    }
    
}

// Value *treeBuilder(Value *tree) {
//     Value *cur = tree;
//     while (cur->type != NULL_TYPE) {
//         if(car(cur)->type == CLOSE_TYPE) {
//             Value *sub = makeNull();
//             Value *toAdd = cdr (cur);
//             while (car(toAdd)->type != OPEN_TYPE) {
//                 if(toAdd->type == NULL_TYPE) {
//                     printf("Syntax Error: No matching open parenthesis.");
//                     texit(0);
//                 }
//                 sub = cons(toAdd, sub);
//                 toAdd = cdr(toAdd);
//             }
//             cur->c.car = sub;
//             cur->c.cdr = cdr(toAdd);
//         }
//         if(car(cur)->type == OPEN_TYPE) {
//             printf("Syntax Error: No matching close parenthesis.");
//             texit(0);
//         }
//         cur = cdr(cur);
//     }
//     return tree; 
// }

// Prints the tree to the screen in a readable fashion. It should look just like
// Scheme code; use parentheses to indicate subtrees.
void printTree(Value *tree) {
    if(tree == NULL) {
        return;
    }
    Value *cur = tree;
    while (cur->type == CONS_TYPE) {
        switch (car(cur)->type) {
        case CONS_TYPE:
            printf("(");
            printTree(car(cur));
            printf(") ");
            break;

        case INT_TYPE:
            printf("%i ", car(cur)->i);
            break;

        case DOUBLE_TYPE:
            printf("%lf ", car(cur)->d);
            break;

        case BOOL_TYPE:
            printf("%s ", car(cur)->s);
            break;

        case STR_TYPE:
            printf("%s ", car(cur)->s);
            break;

        case SYMBOL_TYPE:
            printf("%s ", car(cur)->s);
            break;

        case NULL_TYPE:
            printf("()");
            break;
        
        default:
            break;
        }
        cur = cdr(cur);
    }
}