#include <stdlib.h>
#include "value.h"
#include "linkedlist.h"
#include <stdio.h>
#include "talloc.h"
#include "tokenizer.h"
#include <string.h>

Value *openToken(Value * list, char charRead);
Value *closeToken(Value * list, char charRead);
Value *numToken(Value * list, char charRead);
Value *symbolToken(Value * list, char charRead);
Value *stringToken(Value * list, char charRead);
Value *boolToken(Value * list, char charRead);
bool isValidForInitial(char charRead);
bool isValidForSubsequent(char charRead);

// Read all of the input from stdin, and return a linked list consisting of the
// tokens.
Value *tokenize() {
    char charRead;
    Value *list = makeNull();
    charRead = (char)fgetc(stdin);
    while (charRead != EOF) {
        char check = charRead;
        if(charRead == ';') {
            while (charRead != '\n') {
                charRead = (char)fgetc(stdin);
            }
        } else if(charRead == '(') {                           //open
            list = openToken(list, charRead);
        } else if (charRead == ')') {                   //close
            list = closeToken(list, charRead);
        } else if (charRead == '-' || charRead == '+') {
            char nextChar = (char) fgetc(stdin);
            if (nextChar >= '0' && nextChar <= '9') {
                ungetc(nextChar, stdin);
                list = numToken(list, charRead);
            } else {
                ungetc(nextChar, stdin);
                list = symbolToken(list, charRead);
            }
        }else if (charRead >= '0' && charRead <= '9') {    //int or double
            list = numToken(list, charRead);
        } else if (charRead == '#') {
            list = boolToken(list, charRead);
        } else if (charRead == '"') {                               
            list = stringToken(list, charRead);
        } else if (isValidForInitial(charRead)) {   //symbol
            list = symbolToken(list, charRead);
        } else if (charRead != '\n' && charRead != ' ') {
            printf("Syntax error (readSymbol): symbol %c does not start with an allowed first character.\n", charRead);
            texit(1);
        }
        charRead = (char) fgetc(stdin);
    }
    list = reverse(list);
    return list;
}

// Takes a ( char and adds it to the front of the given list, returning the modified list.
Value *openToken(Value *list, char charRead) {
    Value *tokenOpen = talloc(sizeof(Value));
    tokenOpen->type  = OPEN_TYPE;
    char *openHolder = talloc(sizeof(char) * 2);
    openHolder[0] = '(';
    openHolder[1] = '\0';
    tokenOpen->s = openHolder;
    return list = cons(tokenOpen, list);
}

// Takes a ) char and adds it to the front of the given list, returning the modified list.
Value *closeToken(Value *list, char charRead) {
    Value *tokenClose = talloc(sizeof(Value));
    tokenClose->type  = CLOSE_TYPE;
    char *closeHolder = talloc(sizeof(char) * 2);
    closeHolder[0] = ')';
    closeHolder[1] = '\0';
    tokenClose->s = closeHolder;
    return list = cons(tokenClose, list);
}

// Takes a number or a + or a - char and adds the completed int or double to the front of the given list, returning the modified list.
Value *numToken(Value *list, char charRead) {
    int numHolder = 0;
    bool neg = false;
    if (charRead == '-') {
        neg = true;
        charRead = (char) fgetc(stdin);
    } else if (charRead == '+') {
        charRead = (char) fgetc(stdin);
    }
    
    while (charRead >= '0' && charRead <= '9') {
        numHolder = numHolder * 10;
        numHolder = numHolder + charRead - 48; //0 is 48 in ascii, 1 is 49, etc
        charRead = (char)fgetc(stdin);   
    }
    if (charRead != '.') {
        Value *tokenInt = talloc(sizeof(Value));
        tokenInt->type = INT_TYPE;
        if(neg) {
            tokenInt->i = -numHolder;
        } else {
            tokenInt->i = numHolder;
        }
        ungetc(charRead, stdin);
        return list = cons(tokenInt, list);
    } else {
        int deciCount = 1;
        double decimalHolder = 0;
        charRead = (char)fgetc(stdin);   
        while (charRead >= '0' && charRead <= '9') {
            deciCount = deciCount * 10;
            decimalHolder = decimalHolder * 10;
            decimalHolder = decimalHolder + charRead - 48; //0 is 48 in ascii, 1 is 49, etc
            charRead = (char)fgetc(stdin);   
        }
        double doubleVal = numHolder;
        doubleVal = doubleVal + decimalHolder / deciCount;
        Value *tokenDouble = talloc(sizeof(Value));
        tokenDouble->type = DOUBLE_TYPE;
        if(neg) {
            tokenDouble->d = -doubleVal;
        } else {
            tokenDouble->d = doubleVal;
        }
        ungetc(charRead, stdin);
        return list = cons(tokenDouble, list);
    }
}

// Takes a # char and adds a bool determined by a valid following char to the front of the given list, returning the modified list.
Value *boolToken(Value *list, char charRead) {
    Value *tokenBool = talloc(sizeof(Value));
    tokenBool->type  = BOOL_TYPE;
    charRead = (char)fgetc(stdin);
    char *boolHolder = talloc(sizeof(char) * 3);
    boolHolder[0] = '#';
    if(charRead == 't' || charRead == 'f') {
        boolHolder[1] = charRead;
    } else {
        printf("Syntax error (readBoolean): boolean was not #t or #f\n");
        texit(1);
    }
    boolHolder[1] = charRead;
    boolHolder[2] = '\0';
    tokenBool->s = talloc(sizeof(char) * 3);
    strcpy(tokenBool->s, boolHolder);                               //t is true, f is false
    return list = cons(tokenBool, list);
}

// Takes a " char and adds the following string to the front of the given list, returning the modified list.
Value *stringToken(Value *list, char charRead) {
    char *stringHolder = talloc(sizeof(char) * 301);
    int i = 0;
    stringHolder[i] = charRead;
    charRead = (char)fgetc(stdin);
    i++;
    while (charRead != '"') {
        stringHolder[i] = charRead;
        charRead = (char)fgetc(stdin);
        i++;
    }
    stringHolder[i] = charRead;
    i++;
    stringHolder[i] = '\0';
    Value *tokenString = talloc(sizeof(Value));
    tokenString->type = STR_TYPE;
    tokenString->s = talloc(sizeof(char) * (i + 1)); //301 before
    strcpy(tokenString->s, stringHolder);
    return list = cons(tokenString, list);
}

// Takes a valid initial char and adds the complete symbol to the front of the given list, returning the modified list.
Value *symbolToken(Value *list, char charRead) {
    char *symbolHolder = talloc(sizeof(char) * 301);
    int i = 0;

    /* Start new */
    symbolHolder[i] = charRead;
    i++;
    charRead = (char)fgetc(stdin);
    /* End new */

    while (isValidForSubsequent(charRead)) {
        symbolHolder[i] = charRead;
        charRead = (char)fgetc(stdin);
        i++;
    }
    symbolHolder[i] = '\0';
    Value *tokenSymbol = talloc(sizeof(Value));
    tokenSymbol->type = SYMBOL_TYPE;
    tokenSymbol->s = talloc(sizeof(char) * (i + 1)); //301 before
    strcpy(tokenSymbol->s, symbolHolder);
    ungetc(charRead, stdin);
    return list = cons(tokenSymbol, list);
}

// Takes a char and checks if it is a valid initial value for a symbol, returning true if so and flase if not.
bool isValidForInitial(char charRead) {
    if (charRead >= 'a' && charRead <= 'z' || charRead >= 'A' && charRead <= 'Z') {
        return true;
    } else if (charRead == '!' || charRead == '$' || charRead == '&' || charRead == '*' || charRead == '/' || charRead == ':' || charRead == '<' || charRead == '=' || charRead == '>' || charRead == '?' || charRead == '-' || charRead == '_' || charRead == '^') {
        return true;
    }
    return false;
}

// Takes a char and checks if it is a valid subsequent value for a symbol, returning true if so and flase if not.
bool isValidForSubsequent(char charRead) {
    if (isValidForInitial(charRead)) {
        return true;
    } else if(charRead >= '0' && charRead <= '9') {
        return true;
    } else if (charRead == '+' || charRead == '-' || charRead == '.') {
        return true;
    }
    return false;
}

// Displays the contents of the linked list as tokens, with type information
void displayTokens(Value *list) {
    display(list);
}