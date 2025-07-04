#include <stdlib.h>
#include "value.h"
#include "linkedlist.h"
#include <stdio.h>
#include "talloc.h"
#include "tokenizer.h"
#include <string.h>
#include "parser.h"
#include "interpreter.h"

void interpret(Value *tree);
Value *eval(Value *expr, Frame *frame);
Value *evalIf(Value *args, Frame *frame);
Value *evalLet(Value *expr, Frame *frame);
Value *findSymbol(Value *expr, Frame *frame);
void printResult(Value *tree);

/*@Params:
Value *tree: A parsed tree of scheme code 
Creates a global frame and calls eval on every top level branch of the tree, printing the results
*/
void interpret(Value *tree) {
    Frame *globalFrame = talloc(sizeof(Frame));
    globalFrame->bindings = makeNull();
    globalFrame->parent = NULL;
    Value *branch = tree;
    Value *result = makeNull();
    while(branch->type == CONS_TYPE) {
        result = cons(eval(car(branch), globalFrame), result);//HERE
        branch = cdr(branch);
    }
    printResult(result);
}

/*@Params:
Value *expr: A given expression of parsed scheme code
Frame *frame: A given frame of bindings (which contains relevant local and global variables) to evaluate with the expression
Returns Value * depending on the type of expression, suitably finding the correct value for a symbol by calling findSymbol on SYMBOL_TYPE
expressions. For CONS_TYPEs, it checks if it matches the scheme special functions, and calls their respective functions if it does.
*/
Value *eval(Value *expr, Frame *frame) {
    switch (expr->type) {
    case INT_TYPE:
        return expr;
        break;

    case DOUBLE_TYPE:
        return expr;
        break;

    case BOOL_TYPE:
        return expr;
        break;

    case STR_TYPE:
        return expr;
        break;
    
    case SYMBOL_TYPE:
        return findSymbol(expr, frame);
        break;
        
    case CONS_TYPE:
        ;
        Value *initial = car(expr);
        Value *args = cdr(expr);

        if(initial->type != SYMBOL_TYPE) {
            printf("Evaluation error! type\n");
            texit(1);
            return initial;
        }

        if(!strcmp(initial->s, "if")) {
            return evalIf(args, frame);
        }
        else if(!strcmp(initial->s, "let")) {
            if(cdr(args)->type == NULL_TYPE) {
                printf("Evaluation error: Wrong number of args for let\n");
                texit(1);
            }
            return evalLet(args, frame);
        } else {
            printf("Evaluation error! invalid cons type\n");
            texit(1);
            return initial;
        }
        break;

    case NULL_TYPE:
        printf("Eval called on NULL_TYPE!\n");
        texit(1);

    default:
        printf("Weird Case Type error!\n");
        texit(1);
        break;
    }
    printf("Weird Stuff is happening");
    texit(1);
    return expr;
}

/*@Params:
Value *args: all tokens after the if symbol that is within the scope of the if block
Frame *frame: frame that includes relevant local or global variables
Evaluates the condition into a boolean, then evaluate the corresponding value and returns it
*/
Value *evalIf(Value *args, Frame *frame) {
    if(car(args)->type == SYMBOL_TYPE) {
        Value *symb = eval(car(args), frame);
        args = cons(symb, cdr(args));
        evalIf(args, frame);
    }
    if(car(args)->type != BOOL_TYPE) {
        printf("Evaluation Error: If not followed by a boolean\n");
        texit(1);
    }
    if(cdr(args)->type == NULL_TYPE) {
        printf("Evaluation Error: If not followed by a true case\n");
        texit(1); 
    }
    if(cdr(cdr(args))->type == NULL_TYPE) {
        printf("Evaluation Error: If not followed by a false case\n");
        texit(1);
    }
    if(strcmp(car(args)->s, "#t") == 0) {
        Value *tru = cdr(args);
        return eval(car(tru), frame);
    } else if (strcmp(car(args)->s, "#f") == 0) {
        Value *fals = cdr(args);
        fals = cdr(fals);
        return eval(car(fals), frame);
    }
    printf("if's ifs don't work.\n");
    texit(1);
    return args;
}

/*@Params:
Value *args: all tokens after the let symbol that is within the scope of the let block
Frame *frame: frame that includes relevant local or global variables
Creates a binding from the first branch of arguments, then evaluates the latter arguments.
*/
Value *evalLet(Value *args, Frame *frame) {
    if(args->type != CONS_TYPE) {
        printf("Syntax Error: Invalid let type\n");
        texit(1);
    }
    Frame *newFrame = talloc(sizeof(Frame));
    newFrame->parent = frame;
    newFrame->bindings = makeNull();
    Value *letPairs = car(args);
    if(letPairs->type == NULL_TYPE) {
        return eval(car(cdr(args)), newFrame);
    }
    if(letPairs->type != CONS_TYPE) {
        printf("Evaluation error: Invalid let pair.\n");
        texit(1);
    }
    args = cdr(args);
    while(letPairs->type == CONS_TYPE) {
        Value *pair = car(letPairs);
        if(pair->type != CONS_TYPE) {
            printf("Evaluation error: Invalid let pair.\n");
            texit(1);
        }
        if(car(pair)->type != SYMBOL_TYPE || cdr(pair)->type != CONS_TYPE) {
            printf("Evaluation error: Invalid let pair.\n");
            texit(1);
        }
        Value *symb = car(pair);
        Value *val = eval(car(cdr(pair)), frame);
        Value *cur = newFrame->bindings;
        while(cur->type != NULL_TYPE) {
            Value *bind = car(cur);
            if(!strcmp(symb->s, car(bind)->s)) {
                printf("Evaluation error: Repeated variable in same let call.\n"); 
                texit(1); 
            } 
            cur = cdr(cur); 
        }
        Value *newBind = cons(symb, val);
        newFrame->bindings = cons(newBind, newFrame->bindings);
        letPairs = cdr(letPairs);
    }
    while(cdr(args)->type == CONS_TYPE) {
        args = cdr(args);
    }
    return eval(car(args), newFrame);
}

/*@Params:
Value *expr: all tokens after the if symbol that is relevant to the if block
Frame *frame: frame that includes relevant local or global variables
Creates a binding from the first branch of arguments, then evaluates the latter arguments.
*/
Value *findSymbol(Value *expr, Frame *frame) {
    Value *cur = frame->bindings;
    while(cur->type != NULL_TYPE) {
        Value *bind = car(cur);
        if(!strcmp(expr->s, car(bind)->s)) {
            return eval(cdr(bind), frame);
        }
        cur = cdr(cur);
    } if(frame->parent != NULL) {
        return findSymbol(expr, frame->parent);
    } else {
        printf("Evaluation Error: symbol '%s' not found\n", expr->s);
        texit(1);
    }
    return eval(expr, frame);
}

/*@Params:
Value *tree: a linked list the contains all the result(s) from the evaluations
Prints out the result of evaluations
*/
void printResult(Value *tree) {
    if(tree == NULL) {
        return;
    }
    Value *cur = tree;
    while (cur->type == CONS_TYPE) {
        switch (car(cur)->type) {
        case CONS_TYPE:
            printf("(");
            printTree(car(cur));
            printf(")");
            break;

        case INT_TYPE:
            printf("%i", car(cur)->i);
            break;

        case DOUBLE_TYPE:
            printf("%lf", car(cur)->d);
            break;

        case BOOL_TYPE:
            printf("%s", car(cur)->s);
            break;

        case STR_TYPE:
            printf("%s", car(cur)->s);
            break;

        case SYMBOL_TYPE:
            printf("%s", car(cur)->s);
            break;

        case NULL_TYPE:
            printf("()");
            break;
        
        default:
            break;
        }
        cur = cdr(cur);
    }
    printf("\n");
}