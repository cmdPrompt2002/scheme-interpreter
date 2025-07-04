#include <stdbool.h>
#include "value.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "linkedlist.h"
#include <string.h>
#include "talloc.h"


// Create a new NULL_TYPE value node.
Value *makeNull() {
    Value *myNull = talloc(sizeof(Value));
    myNull->type = NULL_TYPE;
    return myNull;
}

// Create a new CONS_TYPE value node.
Value *cons(Value *newCar, Value *newCdr) {
    Value *myCons = talloc(sizeof(Value));
    myCons->type = CONS_TYPE;
    myCons->c.car = newCar;
    myCons->c.cdr = newCdr;
    return myCons;
}

// Utility to make it less typing to get car value. Use assertions to make sure
// that this is a legitimate operation.
Value *car(Value *list){
    assert(list->type == CONS_TYPE);
    return list->c.car;
}

// Utility to make it less typing to get cdr value. Use assertions to make sure
// that this is a legitimate operation.
Value *cdr(Value *list) {
    assert(list->type == CONS_TYPE);
    return list->c.cdr;
}

// Utility to check if pointing to a NULL_TYPE value. Use assertions to make sure
// that this is a legitimate operation.
bool isNull(Value *value) {
    if(value->type == NULL_TYPE) {
        return true;
    }
    return false;
}

// Measure length of list. Use assertions to make sure that this is a legitimate
// operation.
int length(Value *value) {
    int count = 0;
    Value *tracker = value;
    if(value->type == NULL_TYPE) {
        return 0;
    }
    if(value->type != CONS_TYPE) {
        return 1;
    }
    while(tracker->type == CONS_TYPE) {
        count++;
        tracker = cdr(tracker);
    }
    return count;
}

// Display the contents of the linked list to the screen in some kind of
// readable format
void display(Value *list) {
    Value *toPrint = list;
    while (toPrint->type == CONS_TYPE) {
        switch (car(toPrint)->type) {
        case (INT_TYPE):
            printf("%i:integer\n", car(toPrint)->i);
            break;
        case (DOUBLE_TYPE):
            printf("%lf:double\n", car(toPrint)->d);
            break;
        case (STR_TYPE):
            printf("%s:string\n", car(toPrint)->s);
            break;
        case (SYMBOL_TYPE):
            printf("%s:symbol\n", car(toPrint)->s);
            break;
        case (BOOL_TYPE):
            printf("%s:boolean\n", car(toPrint)->s);
            break;
        case (OPEN_TYPE):
            printf("%s:open\n", car(toPrint)->s);
            break;
        case (CLOSE_TYPE):
            printf("%s:close\n", car(toPrint)->s);
            break;
        default:
            break;
        }
        toPrint = cdr(toPrint);
    }
    switch (toPrint->type) {
        case (INT_TYPE):
            printf("value: %i\n", toPrint->i);
            break;
        case (DOUBLE_TYPE):
            printf("value: %f\n", toPrint->d);
            break;
        case (STR_TYPE):
            printf("value: %s\n", toPrint->s);
            break;
        case (SYMBOL_TYPE):
            printf("%s:symbol\n", car(toPrint)->s);
            break;
        case (BOOL_TYPE):
            printf("#%s:boolean\n", car(toPrint)->s);
            break;
        case (OPEN_TYPE):
            printf("%s:open\n", car(toPrint)->s);
            break;
        case (CLOSE_TYPE):
            printf("%s:open\n", car(toPrint)->s);
            break;
        default:
            break;
    }
}

// Return a new list that is the reverse of the one that is passed in. All
// content within the list should be duplicated; there should be no shared
// memory whatsoever between the original list and the new one.
//
// FAQ: What if there are nested lists inside that list?
// ANS: There won't be for this assignment. There will be later, but that will
// be after we've got an easier way of managing memory.
Value *reverse(Value *list) {
    if(list->type == CONS_TYPE) {
        Value *oldList = list;
        Value *newNextValue = makeNull();
        while (oldList->type == CONS_TYPE) {
            Value *carNode = talloc(sizeof(Value));
            switch (car(oldList)->type) {
                case (INT_TYPE):
                    carNode->type = INT_TYPE;
                    carNode->i = car(oldList)->i;
                    break;
        
                case (DOUBLE_TYPE):
                    carNode->type = DOUBLE_TYPE;
                    carNode->d = car(oldList)->d;
                    break;

                case (STR_TYPE):
                    carNode->type = STR_TYPE;
                    carNode->s = talloc(sizeof(char) * (strlen(car(oldList)->s) + 1));
                    strcpy(carNode->s, car(oldList)->s);
                    break;
                
                case (BOOL_TYPE):
                    carNode->type = BOOL_TYPE;
                    carNode->s = talloc(sizeof(char) * (strlen(car(oldList)->s) + 1));
                    strcpy(carNode->s, car(oldList)->s);
                    break;

                case (OPEN_TYPE):
                    carNode->type = OPEN_TYPE;
                    carNode->s = talloc(sizeof(char) * (strlen(car(oldList)->s) + 1));
                    strcpy(carNode->s, car(oldList)->s);
                    break;

                case (CLOSE_TYPE):
                    carNode->type = CLOSE_TYPE;
                    carNode->s = talloc(sizeof(char) * (strlen(car(oldList)->s) + 1));
                    strcpy(carNode->s, car(oldList)->s);
                    break;

                case (SYMBOL_TYPE):
                    carNode->type = SYMBOL_TYPE;
                    carNode->s = talloc(sizeof(char) * (strlen(car(oldList)->s) + 1));
                    strcpy(carNode->s, car(oldList)->s);
                    break;

                default:
                    break;
            }
            if(isNull(oldList)) {
                return newNextValue;
            }
            newNextValue = cons(carNode, newNextValue);
            oldList = cdr(oldList);
        }
        return newNextValue;
    }
    if(isNull(list)) {
        return makeNull();
    }
    Value *singleList = talloc(sizeof(Value));
    switch (list->type) {
        case (INT_TYPE):
            singleList->type = INT_TYPE;
            singleList->i = list->i;
            break;
        
        case (DOUBLE_TYPE):
            singleList->type = DOUBLE_TYPE;
            singleList->d = list->d;
            break;

        case (STR_TYPE):
            singleList->type = STR_TYPE;
            strcpy(singleList->s, list->s);
            break;

        default:
            break;
    }
    return singleList;
}

// Frees up all memory directly or indirectly referred to by list. This includes strings.
//
// FAQ: What if a string being pointed to is a string literal? That throws an
// error when freeing.
//
// ANS: Don't put a string literal into the list in the first place. All strings
// added to this list should be able to be freed by the cleanup function.
//
// FAQ: What if there are nested lists inside that list?
//
// ANS: There won't be for this assignment. There will be later, but that will
// be after we've got an easier way of managing memory.
void cleanup(Value *list) {
    Value *sweap = list;
    if(list->type == NULL_TYPE) {
        free (sweap);
        return;
    }
    if(car(sweap)->type == STR_TYPE) {
        free(car(sweap)->s);
    }
    while(sweap->type == CONS_TYPE) {
        Value *nextValue = cdr(sweap);
        if(car(sweap)->type == STR_TYPE) {
            free(car(sweap)->s);
        }
        if(car(sweap)->type == CONS_TYPE) {
            cleanup(car(sweap));
        }
        free(car(sweap));
        free(sweap);
        sweap = nextValue;
    }
    free(sweap);
}