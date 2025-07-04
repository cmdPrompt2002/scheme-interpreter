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

/*----FUNCTION APPLICATION----*/
Value *apply(Value *function, Value *args);
Value *primitiveApply(Value *function, Value *args, Frame *frame);

/*----SPECIAL FORMS----*/
Value *evalIf(Value *args, Frame *frame);
Value *evalLet(Value *expr, Frame *frame);
Value *evalLetRec(Value *args, Frame *frame);
Value *evalDefine(Value *args, Frame *frame);
Value *evalLambda(Value *args, Frame *frame);
Value *evalSetBang(Value *args, Frame *frame);
Value *evalBegin(Value *args, Frame *frame);

/*----PRIMITIVES----*/
Value *primitiveAdd(Value *args);
Value *primitiveCons(Value *args);
Value *primitiveCar(Value *args);
Value *primitiveCdr(Value *args);
Value *primitiveEqual(Value *args);
Value *primitiveMinus(Value *args);
Value *primitiveMoreThan(Value *args);
Value *primitiveLessThan(Value *args);
Value *primitiveIsNull(Value *args);
void bind(char *name, Value *(*function)(struct Value *), Frame *frame);

/*----MISC----*/
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

    //Store all the primitives in the global frame
    bind("+", primitiveAdd, globalFrame);
    bind("-", primitiveMinus, globalFrame);
    bind("<", primitiveLessThan, globalFrame);
    bind(">", primitiveMoreThan, globalFrame);
    bind("=", primitiveEqual, globalFrame);
    bind("cons", primitiveCons, globalFrame);
    bind("car", primitiveCar, globalFrame);
    bind("cdr", primitiveCdr, globalFrame);

    Value *branch = tree;
    Value *result = makeNull();
    while(branch->type == CONS_TYPE) {
        result = cons(eval(car(branch), globalFrame), result);//HERE
        branch = cdr(branch);
    }
    result = reverse(result);
    printResult(result);
}

/*@Params:
Value *expr: A given expression of parsed scheme code
Frame *frame: A given frame of bindings to evaluate with the expression
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
    case CLOSURE_TYPE:
        return expr;
        break;
    case PRIMITIVE_TYPE:
        return expr;
        break;
    case UNSPECIFIED_TYPE:
        return expr;
        break;
    case CONS_TYPE:
        ;
        Value *initial = car(expr);
        Value *args = cdr(expr);

        if(initial->type != SYMBOL_TYPE) {
            if(initial->type != CONS_TYPE) {
                printf("Evaluation error: type\n");
                texit(0);
            }
            //Should return a primitive or a closure type (remember that we have a binding in the global frame)
            //Because cal calls findsymbol, findsymbol looks in the global frame and returns the value of the variable
            initial = eval(initial,frame); 
            if (initial->type == CLOSURE_TYPE) {
                return apply(eval(initial, frame), args); //eval(initial, frame)
            }
            else if (initial->type == PRIMITIVE_TYPE) {
                return primitiveApply(eval(initial, frame), args, frame);
            }
            printf("Evaluation error: type\n");
            texit(0);
            
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
        } else if (!strcmp(initial->s, "letrec")) {
            if(cdr(args)->type == NULL_TYPE) {
                printf("Evaluation error: Wrong number of args for let\n");
                texit(1);
            }
            return evalLetRec(args, frame);
        } else if(!strcmp(initial->s, "quote")) {
            if(args->type == NULL_TYPE) {
                printf("Evaluation error: Wrong number of args for quote\n");
                texit(1);
            }
            if(cdr(args)->type != NULL_TYPE) {
                Value *err = talloc(sizeof(Value));
                err->type = STR_TYPE;
                err->s = "Evaluation error\n";
                return err;
            }
            return car(args);

        } else if (!strcmp(initial->s, "set!")) {
            return evalSetBang(args, frame);
        } else if(!strcmp(initial->s, "begin")) {
            return evalBegin(args, frame);
        } else if(!strcmp(initial->s, "define")) {
            return evalDefine(args, frame);
        } else if(!strcmp(initial->s, "lambda")) {
            return evalLambda(args, frame);
        } else { //at this point, initial is presumably a function name.

            //If initial is a function name, then OK. Otherwise not OK.
            Value *function = findSymbol(initial, frame);
            
            if (function->type == CLOSURE_TYPE) {
                Value *tempArgs = args;
                Value *evaledArgs = makeNull();
                while (tempArgs->type == CONS_TYPE) {
                    evaledArgs = cons(eval(car(tempArgs),frame), evaledArgs);
                    tempArgs = cdr(tempArgs);
                }
                evaledArgs = reverse(evaledArgs);
                return apply(function, evaledArgs);

            } else if (function->type == PRIMITIVE_TYPE) {
                return primitiveApply(function, args, frame);
            }

            printf("Evaluation Error: Invalid findSymbol result type\n");
            texit(0);
            return apply(function, args);
        }
        break;

    case NULL_TYPE:
        return expr;
        break;
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
Value *args: all tokens after the if symbol that is relevant to the if block
Frame *frame: frame that includes relevant local or global variables
Evaluates the condition into a boolean, then evaluate the corresponding return value accordingly and return it
*/
Value *evalIf(Value *args, Frame *frame) {
    Value *temp = args;
    Value *myBool = eval(car(temp), frame);

    if(myBool->type != BOOL_TYPE) {
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
    if(strcmp(myBool->s, "#t") == 0) {
        Value *tru = car(cdr(args));
        return eval(tru, frame);
    } else if (strcmp(myBool->s, "#f") == 0) {
        Value *fals = car(cdr(cdr(args)));
        return eval(fals, frame);
    }
    printf("if's ifs don't work.\n");
    texit(1);
    return args;
}

/*@Params:
Value *args: all tokens after the let symbol that is relevant to the let block
Frame *frame: frame that includes relevant local and global variables
Creates a binding from the first branch of arguments, then evaluates the latter arguments using the bindings.
*/
Value *evalLet(Value *args, Frame *frame) {
    if(args->type != CONS_TYPE) {
        printf("Evaluation Error: Invalid let type\n");
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
        printf("Evaluation Error: Invalid let pair.\n");
        texit(1);
    }
    args = cdr(args);
    while(letPairs->type == CONS_TYPE) {
        Value *pair = car(letPairs);
        if(pair->type != CONS_TYPE) {
            printf("Evaluation Error: Invalid let pair.\n");
            texit(1);
        }
        if(car(pair)->type != SYMBOL_TYPE || cdr(pair)->type != CONS_TYPE) {
            printf("Evaluation Error: Invalid let pair.\n");
            texit(1);
        }
        Value *symb = car(pair);
        Value *val = eval(car(cdr(pair)), frame);
        Value *cur = newFrame->bindings;
        while(cur->type != NULL_TYPE) {
            Value *bind = car(cur);
            if(!strcmp(symb->s, car(bind)->s)) {
                printf("Evaluation Error: Repeated variable in same let call.\n");
                texit(1);
            }
            cur = cdr(cur);
        }
        Value *newBind = cons(symb, val);
        newFrame->bindings = cons(newBind, newFrame->bindings);
        letPairs = cdr(letPairs);
    }
    while(cdr(args)->type == CONS_TYPE) {
        eval(car(args), newFrame);
        args = cdr(args);
    }
    return eval(car(args), newFrame);
}

/*@Params:
Value *args: all tokens after the if symbol that is relevant to the letrec block
Frame *frame: frame that includes relevant local and global variables
A variant of the let function that allows a local variable to be bound to a recursive function.
*/
Value *evalLetRec(Value *args, Frame *frame) {
    if(args->type != CONS_TYPE) {
        printf("Evaluation Error: Invalid let type\n");
        texit(1);
    }
    Frame *newFrame = talloc(sizeof(Frame));
    newFrame->parent = frame;
    newFrame->bindings = makeNull();

    //Error checking
    Value *letPairs = car(args);
    if(letPairs->type == NULL_TYPE) {
        return eval(car(cdr(args)), newFrame);
    }
    if(letPairs->type != CONS_TYPE) {
        printf("Evaluation Error: Invalid let pair.\n");
        texit(1);
    }
    args = cdr(args); //THIS IS IMPORTANT!!!
    //Add symbols to the bindings and eval all the values
    Value *evaledValues = makeNull();
    while(letPairs->type == CONS_TYPE) {
        Value *pair = car(letPairs);

        //Error checking
        if(pair->type != CONS_TYPE) {
            printf("Evaluation Error: Invalid let pair.\n");
            texit(1);
        }
        if(car(pair)->type != SYMBOL_TYPE || cdr(pair)->type != CONS_TYPE) {
            printf("Evaluation Error: Invalid let pair.\n");
            texit(1);
        }

        //Setting up the bindings: symbol
        Value *symb = car(pair);
        symb->type = UNSPECIFIED_TYPE;

        //Add the symbol to the bindings
        Value *binding = cons(symb, makeNull());
        binding->c.cdr->type = UNSPECIFIED_TYPE;
        newFrame->bindings = cons(binding,newFrame->bindings);

        //Setting up the bindings: value
        Value *val = eval(car(cdr(pair)), newFrame);
        if (val->type == UNSPECIFIED_TYPE) {
            printf("Evaluation error: eval called on UNSPECIFIED_TYPE\n");
            texit(0);
        }
        evaledValues = cons(val, evaledValues);
        letPairs = cdr(letPairs);
    }

    //Add the evaled values to the bindings
    //newFrame->bindings = reverse(frame->bindings);
    Value *curBinding = newFrame->bindings;
    Value *curValue = evaledValues;
    while(curBinding->type != NULL_TYPE) { 
        curBinding->c.car->c.car->type = SYMBOL_TYPE;
        curBinding->c.car->c.cdr = curValue->c.car;

        curBinding = cdr(curBinding);
        curValue = cdr(curValue);
    }

    while(cdr(args)->type == CONS_TYPE) {
        eval(car(args),newFrame);
        args = cdr(args);
    }
    return eval(car(args), newFrame);
}

/*@Params:
Value *args: all tokens after the 'begin' symbol that is relevant to the begin block
Frame *frame: frame that includes relevant local and global variables
Evaluates the arguments sequentially, and returns the evaluation of the last argument
*/
Value *evalBegin(Value *args, Frame *frame) {
    if (args->type == NULL_TYPE) {
        Value *result = talloc(sizeof(Value));
        result->type = VOID_TYPE;
        return result;
    }
    Value *currArg = args;
    while (cdr(currArg)->type == CONS_TYPE) {
        eval(car(currArg), frame);
        currArg = cdr(currArg);
    }
    //After the while loop, currArg should be the last arg
    Value *result = eval(car(currArg), frame);
    return result;
}

/*@Params:
Value *args: all tokens after the define symbol that is relevant: the variable and its definitinon
Frame *frame: frame that includes relevant local and global variables
Adds the variable and its definition into the global frame bindings.
Returns a VOID_TYPE Value* (bc the eval function must return a Value *)
*/
Value *evalDefine(Value *args, Frame *frame) {
    if(args->type != CONS_TYPE) {
        printf("Evaluation Error: Invalid number of args\n");
        texit(1);
    }
    if(cdr(args)->type != CONS_TYPE) {
        printf("Evaluation Error: Invalid number of args\n");
        texit(1);
    }
    if(cdr(cdr(args))->type != NULL_TYPE) {
        printf("Evaluation Error: Invalid number of args\n");
        texit(1);
    }
    Value *symb = car(args);
    if(symb->type != SYMBOL_TYPE) {
        printf("Evaluation Error: Invalid type of args\n");
        texit(1);
    }
    Value *val = eval(car(cdr(args)), frame);


    Value *newBindPair = cons(symb, val);
    frame->bindings = cons(newBindPair, frame->bindings);
    Value *returnVoid = talloc(sizeof(Value));
    returnVoid->type = VOID_TYPE;
    return returnVoid;
}

/*@Params:
Value *args: all tokens after the lambda symbol that is relevant: the function's parameters and code body
Frame *frame: frame that includes relevant local and global variables
Returns a Value* that points to a value containing a closure
The closure stores a list of parameters (paramnames), the body code to be executed (functioncode), and a frame containing relevant variables
*/
Value *evalLambda(Value *args, Frame* frame) {
    Value *closure = talloc(sizeof(Value));
    closure->type = CLOSURE_TYPE;
    if(args->type != CONS_TYPE) {
        printf("Evaluation Error: no args following lambda\n");
        texit(1);
    }
    if(cdr(args)->type != CONS_TYPE) {
        printf("Evaluation Error: only 1 arg following lambda\n");
        texit(1);
    }
    if(cdr(cdr(args))->type != NULL_TYPE) {
        printf("Evaluation Error: too many args following lambda\n");
        texit(1);
    }
    Value *paramTemp = car(args);
    while(paramTemp->type == CONS_TYPE) {
        if(car(paramTemp)->type != SYMBOL_TYPE) {
            printf("Evaluation error: formal parameters for lambda must be symbols.\n");
            texit(1);
        }
        Value *dupCheck = cdr(paramTemp);
        while (dupCheck->type != NULL_TYPE) {
            if (!strcmp(car(paramTemp)->s, car(dupCheck)->s)) {
                printf("Evaluation Error: Duplicate parameter symbols\n");
                texit(1);
            }
            dupCheck = cdr(dupCheck);
        }
        paramTemp = cdr(paramTemp);
    }    
    closure->cl.paramNames = car(args);
    closure->cl.functionCode = car(cdr(args));
    closure->cl.frame = frame;
    return closure;
}

/*@Params:
Value *args: all tokens after the set! symbol that is relevant: the variable and its new definition
Frame *frame: frame that includes relevant local and global variables
Modifies the definition of the existing variable. If the variable does not exist in the chain of frames, throw an error.
Returns a VOID_TYPE Value* (bc the eval function must return a Value *)
*/
Value *evalSetBang(Value *args, Frame *frame) {
    if (args->type == NULL_TYPE || cdr(args)->type == NULL_TYPE || cdr(cdr(args))->type != NULL_TYPE) {
        printf("Evaluation error: set! expects exactly 2 arguments\n");
        texit(0);
    }

    if (car(args)->type != SYMBOL_TYPE) {
        printf("Evaluation error: 1st arg should be an existing symbol\n");
        texit(0);
    }
    Value *arg1 = car(args);
    Value *arg2 = car(cdr(args));
    Value *newSymbol = findSymbol(arg1,frame); 

    if (arg2->type == CONS_TYPE || arg2->type == SYMBOL_TYPE) {
        arg2 = eval(arg2,frame); 
    }
    switch (arg2->type) {
        case CONS_TYPE:
            newSymbol->type = CONS_TYPE;
            newSymbol->c = arg2->c;
            break;
        case INT_TYPE:
            newSymbol->type = INT_TYPE;
            newSymbol->i = arg2->i;
            break;
        case DOUBLE_TYPE:
            newSymbol->type = DOUBLE_TYPE;
            newSymbol->d = arg2->d;
            break;
        case BOOL_TYPE:
            newSymbol->type = BOOL_TYPE;
            newSymbol->s = arg2->s;
            break;
        case STR_TYPE:
            newSymbol->type = STR_TYPE;
            newSymbol->s = arg2->s;
            break;
        case CLOSURE_TYPE:
            newSymbol->type = CLOSURE_TYPE;
            newSymbol->cl = arg2->cl;
            break;
        case VOID_TYPE:
            newSymbol->type = VOID_TYPE;
            break;
        case PTR_TYPE:
            newSymbol->type = PTR_TYPE;
            newSymbol->p = arg2->p;
            break;
        default:
            printf("Evaluation error: wrong type for 2nd argument\n");
            texit(0);
            break;
    }
    Value *returnVoid = talloc(sizeof(Value));
    returnVoid->type = VOID_TYPE;
    return returnVoid;
}

/*@Params:
Value *function: closure struct
Value *args: actual arguments to be passed into the function
Applies the function to the actual arguments.
Returns the result of the evaluation
*/
Value *apply(Value *function, Value *args) {
    Frame *tempFrame = talloc(sizeof(Frame));
    tempFrame->parent = function->cl.frame;
    tempFrame->bindings = makeNull();
    Value *params = function->cl.paramNames;
    //Add parameters and arguments to the current frame
    while(params->type == CONS_TYPE && args->type == CONS_TYPE) {
        Value *newBind = cons(car(params), car(args));
        params = cdr(params);
        args = cdr(args);
        tempFrame->bindings = cons(newBind, tempFrame->bindings);
    }
    if (!(params->type == NULL_TYPE && args->type == NULL_TYPE)) {
        printf("Evaluation Error: invalid number of arguments to function\n");
        texit(1);
    }
    return eval(function->cl.functionCode, tempFrame);
}

/*@Params:
Value *function: primitive struct
Value *args: actual arguments to be passed into the function
Frame *frame: frame including all relevant local and global variables
Applies the function to the actual arguments.
Returns the result of the evaluation
*/
Value *primitiveApply(Value *function, Value *args, Frame *frame) {
    Value *evaledArgs = makeNull();
    Value *temp = args;
    while (temp->type != NULL_TYPE) {
        evaledArgs = cons(eval(car(temp),frame) , evaledArgs);
        temp = cdr(temp);
    }
    evaledArgs = reverse(evaledArgs);
    return function->pf(evaledArgs);
}

/*@Params:
Char *name: Name of the symbol
Value *(*function)(struct Value *): Pointer to a function
Frame *frame: Frame to add bindings to
Creates a PRIMITIVE_TYPE Value* and binds it to a symbol in the given frame
*/
void bind(char *name, Value *(*function)(struct Value *), Frame *frame) {
    // Add primitive functions to top-level bindings list
    Value *value = talloc(sizeof(Value));
    value->type = PRIMITIVE_TYPE;
    value->pf = function;

    Value *funcName = talloc(sizeof(Value));
    funcName->type = SYMBOL_TYPE;
    funcName->s = talloc((strlen(name) + 1)*sizeof(char));
    strcpy(funcName->s, name);
    

    //funcName->s = name; (should also work)!
    
    Value *newBind = cons(funcName, value);
    frame->bindings = cons(newBind, frame->bindings);
    //printf("func: %s\n", car(car(frame->bindings))->s);
}

/*@Params:
Value *expr: a given symbol type Value *
Frame *frame: frame that includes a map of global and local variables to their values.
Returns the value of the given symbol from the given frame.
*/
Value *findSymbol(Value *expr, Frame *frame) {
    Value *cur = frame->bindings;
    while(cur->type != NULL_TYPE) {
        Value *bind = car(cur);
        if(!strcmp(expr->s, car(bind)->s)) {
            if (cdr(bind)->type == CONS_TYPE && car(cdr(bind))->type != SYMBOL_TYPE) { //If the value is a list: (1 2 3)
                return cdr(bind);
            }
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
Value *args: a list of ints or doubles to be added
Returns the sum of all valid (int or double) arguments
*/
Value *primitiveAdd(Value *args) {
    Value *result = talloc(sizeof(Value));
    int isDouble = 0;
    int iSum = 0;
    double dSum = 0;

    //Genius while loop: if encoutered a double, set isDouble to true and change sum to double.
    while (args->type == CONS_TYPE) {
        if(isDouble == 1 || car(args)->type == DOUBLE_TYPE) {
            isDouble = 1;
            dSum += iSum + car(args)->d;
            iSum = 0;
        } else if(car(args)->type == INT_TYPE) {
            iSum += car(args)->i;
        } else {
            printf("Evaluation Error: Wrong type in add\n");
            texit(0);
        }
        args = cdr(args);
    }
    if (isDouble) {
        result->type = DOUBLE_TYPE;
        result->d = dSum;
    } else {
        result->type = INT_TYPE;
        result->i = iSum;
    }
    return result;
}

/*@Params:
Value *args: a list of ints or doubles to be operated on
Returns the value of [1st arg] - [2nd arg + ... + nth arg]
*/
Value *primitiveMinus(Value *args) {
    Value *result = talloc(sizeof(Value));
    int isDouble = 0;
    int iSum = 0;
    double dSum = 0;

    //Save the first arg for the calculation at the end
    Value *temp = car(args);
    args = cdr(args);

    //Genius while loop: if encoutered a double, set isDouble to true and change sum to double.
    //Computes the sum of all args except the first arg
    while (args->type == CONS_TYPE) {
        if(isDouble == 1 || car(args)->type == DOUBLE_TYPE) {
            isDouble = 1;
            dSum += iSum + car(args)->d;
            iSum = 0;
        } else if(car(args)->type == INT_TYPE) {
            iSum += car(args)->i;
        } else {
            printf("Evaluation Error: Wrong type in add\n");
            texit(0);
        }
        args = cdr(args);
    }
    //Calculate [first arg] - sum
    if (isDouble || temp->type == DOUBLE_TYPE) {
        result->type = DOUBLE_TYPE;
        result->d = temp->d - dSum;
    } else {
        result->type = INT_TYPE;
        result->i = temp->i - iSum;
    }
    return result;
}

/*@Params:
Value *args: a list of ints or doubles to be added
Returns a bool of true if the two args is the same *number* (5 is same as 5.0), else, false.
*/
Value *primitiveEqual(Value *args) {
    if (args->type == NULL_TYPE || cdr(args)->type == NULL_TYPE || cdr(cdr(args))->type != NULL_TYPE) {
        printf("Evaluation error: null? expects exactly 2 argument\n");
        texit(0);
    }

    Value *result = talloc(sizeof(Value));
    result->s = talloc(sizeof(char)*3);
    result->type = BOOL_TYPE;
    int isTrue;

    Value *arg1 = car(args);
    Value *arg2 = car(cdr(args));
    if (arg1->type == INT_TYPE && arg2->type == INT_TYPE) {
        if (arg1->i == arg2->i) { isTrue = 1; }
        else { isTrue = 0; }
    }
    else if (arg1->type == DOUBLE_TYPE && arg2->type == DOUBLE_TYPE) {
        if (arg1->d == arg2->d) { isTrue = 1; }
        else { isTrue = 0; }
    }
    else if (arg1->type == INT_TYPE && arg2->type == DOUBLE_TYPE) {
        if (arg1->i == arg2->d) { isTrue = 1; }
        else { isTrue = 0; }
    }
    else if (arg1->type == DOUBLE_TYPE && arg2->type == INT_TYPE) {
        if (arg1->d == arg2->i) { isTrue = 1; }
        else { isTrue = 0; }
    }
    else {
        printf("Evaluation error: wrong type for =");
        texit(0);
    }
        
    if (isTrue) {
        result->s[0] = '#';
        result->s[1] = 't';
        result->s[2] = '\0';
    } else {
        result->s[0] = '#';
        result->s[1] = 'f';
        result->s[2] = '\0';
    }
    return result;
}

/*@Params:
Value *args: a list of ints or doubles to be added
Returns a bool of true if 1st arg is less than 2nd arg, else, false.
*/
Value *primitiveLessThan(Value *args) {
    if (args->type == NULL_TYPE || cdr(args)->type == NULL_TYPE || cdr(cdr(args))->type != NULL_TYPE) {
        printf("Evaluation error: null? expects exactly 2 argument\n");
        texit(0);
    }

    Value *result = talloc(sizeof(Value));
    result->s = talloc(sizeof(char)*3);
    result->type = BOOL_TYPE;
    
    Value *arg1 = car(args);
    Value *arg2 = car(cdr(args));
    int isTrue;
    if (arg1->type == INT_TYPE && arg2->type == INT_TYPE) {
        if (arg1->i < arg2->i) { isTrue = 1; }
        else { isTrue = 0; }
    }
    else if (arg1->type == DOUBLE_TYPE && arg2->type == DOUBLE_TYPE) {
        if (arg1->d < arg2->d) { isTrue = 1; }
        else { isTrue = 0; }
    }
    else if (arg1->type == INT_TYPE && arg2->type == DOUBLE_TYPE) {
        if (arg1->i < arg2->d) { isTrue = 1; }
        else { isTrue = 0; }
    }
    else if (arg1->type == DOUBLE_TYPE && arg2->type == INT_TYPE) {
        if (arg1->d < arg2->i) { isTrue = 1; }
        else { isTrue = 0; }
    }
    else {
        printf("Evaluation error: wrong type for =");
        texit(0);
    }

    if (isTrue) {
        result->s[0] = '#';
        result->s[1] = 't';
        result->s[2] = '\0';
    } else {
        result->s[0] = '#';
        result->s[1] = 'f';
        result->s[2] = '\0';
    }
    return result;
}

/*@Params:
Value *args: a list of ints or doubles to be added
Returns a bool of true if 1st arg is more than 2nd arg, else, false.
*/
Value *primitiveMoreThan(Value *args) {
    if (args->type == NULL_TYPE || cdr(args)->type == NULL_TYPE || cdr(cdr(args))->type != NULL_TYPE) {
        printf("Evaluation error: null? expects exactly 2 argument\n");
        texit(0);
    }
    Value *result = talloc(sizeof(Value));
    result->s = talloc(sizeof(char)*3);
    result->type = BOOL_TYPE;
    
    Value *arg1 = car(args);
    Value *arg2 = car(cdr(args));
    int isTrue;
    if (arg1->type == INT_TYPE && arg2->type == INT_TYPE) {
        if (arg1->i > arg2->i) { isTrue = 1; }
        else { isTrue = 0; }
    }
    else if (arg1->type == DOUBLE_TYPE && arg2->type == DOUBLE_TYPE) {
        if (arg1->d > arg2->d) { isTrue = 1; }
        else { isTrue = 0; }
    }
    else if (arg1->type == INT_TYPE && arg2->type == DOUBLE_TYPE) {
        if (arg1->i > arg2->d) { isTrue = 1; }
        else { isTrue = 0; }
    }
    else if (arg1->type == DOUBLE_TYPE && arg2->type == INT_TYPE) {
        if (arg1->d > arg2->i) { isTrue = 1; }
        else { isTrue = 0; }
    }
    else {
        printf("Evaluation error: wrong type for =");
        texit(0);
    }

    if (isTrue) {
        result->s[0] = '#';
        result->s[1] = 't';
        result->s[2] = '\0';
    } else {
        result->s[0] = '#';
        result->s[1] = 'f';
        result->s[2] = '\0';
    }
    return result;
}

/*@Params:
Value *args: a single value
Returns a bool of true if the arg is NULL_TYPE, else, false
*/
Value *primitiveIsNull(Value *args) {
    if (args->type == NULL_TYPE || cdr(args)->type != NULL_TYPE) {
        printf("Evaluation error: null? expects exactly 1 argument\n");
        texit(0);
    }
    
    Value *result = talloc(sizeof(Value));
    result->s = talloc(sizeof(char)*3);
    result->type = BOOL_TYPE;
    if (car(args)->type == NULL_TYPE)  {
        result->s[0] = '#';
        result->s[1] = 't';
        result->s[2] = '\0';
    }
    else {
        result->s[0] = '#';
        result->s[1] = 'f';
        result->s[2] = '\0';
    }
    return result;

}

/*@Params:
Value *args: two Value*'s
Returns a cons cell of the two args
*/
Value *primitiveCons(Value *args) {
    if(args->type == NULL_TYPE || cdr(args)->type == NULL_TYPE || cdr(cdr(args))->type != NULL_TYPE) {
        printf("Evaluation Error: Wrong number of params for Cons\n");
        texit(0);
    }
    Value *consCar = car(args);
    Value *consCdr = car(cdr(args));
    return cons(consCar, consCdr);
}

/*@Params:
Value *args: a single CONS_TYPE value
Returns the car of a CONS_TYPE Value*
*/
Value *primitiveCar(Value *args) {
    if(args->type == NULL_TYPE || cdr(args)->type != NULL_TYPE) {
        printf("Evaluation Error: Wrong number of params for Car\n");
        texit(0);
    }
    if(car(args)->type != CONS_TYPE) {
        printf("Evaluation Error: Car operation on invalid type\n");
        texit(0);
    }
    return car(car(args));
}

/*@Params:
Value *args: a single CONS_TYPE value
Returns the cdr of a CONS_TYPE Value*
*/
Value *primitiveCdr(Value *args) {
    if(args->type == NULL_TYPE || cdr(args)->type != NULL_TYPE) {
        printf("Evaluation Error: Wrong number of params for Cdr\n");
        texit(0);
    }
    if(car(args)->type != CONS_TYPE) {
        printf("Evaluation Error: Car operation on invalid type\n");
        texit(0);
    }
    return cdr(car(args));
}

/*@Params:
Value *tree: A linked list containing the result of every top level S-expression
Returns nothing, prints out the tree to stdout.
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
            printResult(car(cur));
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
        case CLOSURE_TYPE:
            printf("#<procedure>");
            break;
        case NULL_TYPE:
            printf("() ");
            break;
        case VOID_TYPE:
            break;
        default:
            break;
        }
        cur = cdr(cur);
    }
    
    if (cur->type != CONS_TYPE && cur->type != NULL_TYPE) {
        printf(". ");
        switch (cur->type) {
        case CONS_TYPE:
            break;
        case INT_TYPE:
            printf("%i ", cur->i);
            break;
        case DOUBLE_TYPE:
            printf("%lf ", cur->d);
            break;
        case BOOL_TYPE:
            printf("%s ", cur->s);
            break;
        case STR_TYPE:
            printf("%s ", cur->s);
            break;
        case SYMBOL_TYPE:
            printf("%s ", cur->s);
            break;
        case CLOSURE_TYPE:
            printf("#<procedure>");
            break;
        case VOID_TYPE:
            break;
        default:
            break;
        }
    }
    printf("\n");
}
//mwah