#include <stdlib.h>
#include "value.h"
#include "linkedlist.h"
#include <stdio.h>
#include "talloc.h"


// Value *tMakeNull();
// Value *tCons(Value *newCar, Value *newCdr);
// bool tIsNull(Value *value);

//Global variable storageHead: a pointer to the first item of the talloc linked list
Value *storageHead;

// Create a new NULL_TYPE value node.
Value *tMakeNull() {
    Value *myNull = malloc(sizeof(Value));
    myNull->type = NULL_TYPE;
    return myNull;
}

// Create a new CONS_TYPE value node.
Value *tCons(Value *newCar, Value *newCdr) {
    Value *myCons = malloc(sizeof(Value));
    myCons->type = CONS_TYPE;
    myCons->c.car = newCar;
    myCons->c.cdr = newCdr;
    return myCons;
}

bool tIsNull(Value *value) {
    // printf("isNull\n");
    // fflush(stdout);
    if (value == NULL) {
        return true;
    }
    if(value->type == NULL_TYPE) {
        return true;
    }
    return false;
}

// Free all pointers allocated by talloc, as well as whatever memory you
// allocated in lists to hold those pointers.
void tfree() {
    
    if (storageHead != NULL) {
        while(tIsNull(storageHead) == false) {
        void *nextVal = storageHead->c.cdr;
        free(storageHead->c.car);
        free(storageHead);
        storageHead = nextVal;
    }
    free(storageHead);
    }
    storageHead = NULL;
}

// Replacement for malloc that stores the pointers allocated. It should store
// the pointers in some kind of list; a linked list would do fine, but insert
// here whatever code you'll need to do so; don't call functions in the
// pre-existing linkedlist.h. Otherwise you'll end up with circular
// dependencies, since you're going to modify the linked list to use talloc.
void *talloc(size_t size) {
    void *val = malloc(sizeof(Value));
    if(storageHead == NULL) {
        storageHead = tCons(val, tMakeNull());
    }
    else {
        storageHead = tCons(val, storageHead);
    }
    return(val);
}

// Replacement for the C function "exit", that consists of two lines: it calls
// tfree before calling exit. It's useful to have later on; if an error happens,
// you can exit your program, and all memory is automatically cleaned up.
void texit(int status) {
    tfree();
    exit(status);
}