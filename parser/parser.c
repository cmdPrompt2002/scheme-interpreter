#include "value.h"
#include <stdio.h>
#include "talloc.h"
#include "linkedlist.h"
#include <stdlib.h>
#include "tokenizer.h"
#include <string.h>
#include <assert.h>

//HELPER FUNCTIONS
void syntaxError(int numParens);


// Takes a list of tokens from a Scheme program, and returns a pointer to a
// parse tree representing that program.
Value *parse(Value *tokens) {
    assert(tokens != NULL && "Error (parse): null pointer");
    Value *stackTop = makeNull(); //utility stack: stackTop is always the most recent item added
    Value *currNode = tokens;
    int numParens = 0;
    while (currNode->type != NULL_TYPE) {
        Value *currToken = car(currNode);
        
        if (currToken->type == OPEN_TYPE) {
            numParens++;
        }

        //if it is NOT a close paren, push on the stack
        if (currToken->type != CLOSE_TYPE) {
            Value *newCar = talloc(sizeof(Value));
            switch (currToken->type) {
                case DOUBLE_TYPE:
                    newCar->type = currToken->type;
                    newCar->d = currToken->d;
                    break;
                    
                case INT_TYPE:
                    newCar->type = currToken->type;
                    newCar->i = currToken->i;
                    break;

                case PTR_TYPE:
                    break;

                case NULL_TYPE:
                    break;

                case CONS_TYPE:
                    break;

                default:
                    newCar->type = currToken->type;
                    newCar->s = talloc(sizeof(char)*(1 + strlen(currToken->s)));
                    strcpy(newCar->s,currToken->s);
                    break;
            }
            stackTop = cons(newCar, stackTop);     
        }

        else { //if it is a close paren
            Value *newList = makeNull();
            
            while(stackTop->type != NULL_TYPE && car(stackTop)->type != OPEN_TYPE) {
                
                newList = cons(car(stackTop), newList);
                stackTop = cdr(stackTop); //pop the stack
            }
            if (stackTop->type == NULL_TYPE) {
                printf("Syntax error: too many close parentheses\n");
                texit(0);
            }
            stackTop = cdr(stackTop); //pop the "("
            numParens--;
            stackTop = cons(newList,stackTop); //push the list onto the stack 

        }
        currNode = cdr(currNode);
    }

    syntaxError(numParens);
    return reverse(stackTop);

}


// Prints the tree to the screen in a readable fashion. It should look just like
// Scheme code; use parentheses to indicate subtrees.
void printTree(Value *tree) {
    if (tree->type == NULL_TYPE) {
        return;
    }
    

    Value *currNode = tree;
    while(currNode->type != NULL_TYPE) {
        Value *currCar = car(currNode);
        switch (currCar->type) {
        case (INT_TYPE):
            printf("%i ", currCar->i);
            break;
        case (DOUBLE_TYPE):
            printf("%lf ", currCar->d);
            break;
        case (STR_TYPE):
            printf("%s ", currCar->s);
            break;
        case (SYMBOL_TYPE):
            printf("%s ", currCar->s);
            break;
        case (BOOL_TYPE):
            printf("%s ", currCar->s);
            break;
        case (CONS_TYPE):
            printf("(");
            printTree(currCar);
            printf(") "); //(+ 1 2)
            break;
        case (NULL_TYPE):
            printf("()");
            break;
        default:
            break;
        }
        currNode = cdr(currNode);
    }
    
    return;
}


//HELPER FUNCTIONS:

//Check if input is a valid parenthetical expression. If not valid then exit.
void syntaxError(int numParens) {
    if (numParens < 0) {
        printf("Syntax error: too many close parentheses\n");
        texit(0);
    }
    else if (numParens > 0) {
        printf("Syntax error: not enough close parentheses\n");
        texit(0);
    }
    return;
}