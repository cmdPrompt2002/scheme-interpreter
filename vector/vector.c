/* Header info for vector.c. The Vector struct contains the data necessary; but
   since C is not object-oriented, the traditional "methods" that you're
   accustomed to are instead functions that take a pointer to that struct as a
   parameter. */

#include <stdlib.h>
#include <stdio.h>

struct Vector
{
    // A pointer to a traditional C array to hold data
    int* array;

    // The size of the actual C array that you have allocated
    int memorySize;

    // The virtual size of the vector, i.e. how much data is in it
    // from the user's perspective
    int size;
};

/* Typing "struct Vector" all the time is cumbersome, this sets it up so you can
 just type "Vector" instead. */

typedef struct Vector Vector;

/* Function prototypes. */

/* Take an already existing Vector as a parameter. (Do not create a Vector
 inside this function. Set its internal memory size to match the value given in
 the parameter, allocate an appropriately-sized C array to hold the actual data,
 and initialize the size variable to 0. */

void init(Vector *vector, int memorySize) {
    vector->size = 0;
    vector->memorySize = memorySize;
    vector->array = malloc(sizeof(int)*memorySize);
}


/* Removes the array associated with the Vector. */
void cleanup(Vector *vector) {
    // for (int i = 0; i < vector->memorySize; i++) {
    //     free(&(vector->array[i]));
    // }
    free(vector->array);
}


/* Print out the Vector for debugging purposes. */

void print(Vector *vector) {
    for (int i = 0; i < vector->size; i++) {
        printf("%d\n", vector->array[i]);
    }
}


/* Insert value at location inside the Vector. If the list has items in
 positions 0 through n-1, you are only permitted to insert new values at
 locations 0 through n. (This is identical to Java's ArrayList behavior.) If
 there is already data at the location you are inserting into, insert slides the
 rest of the data down one position to make room for the new value. Returns 1 if
 success, and 0 if the location is invalid (less than 0, or greater than the
 size). Inserting at the very end of the current data (at position equal to
 size) is fine. All inserts should increase size by 1.

 If the internal array is too small to fit a new value, a new array of twice the
 size is created with malloc. The values are copied, the Vector is appropriately
 updated, and the old array is freed.) */

int insert(Vector *vector, int location, int value) {
    
    if (location >= vector->size + 1 || location < 0) {
        // printf("Invalid location: location must be 0 =< x =< size\n");
        return 0;
    } 
    
    //If we need to resize
    if (vector->size >= vector->memorySize) {
        int* newArray = realloc(vector->array, sizeof(int) *(vector->memorySize) * 2);
        
        if (newArray == NULL) { //If realloc fails, free newArray.
            free(newArray);
            return 0;
        } 
        else {
            vector->array = newArray;
        }
        vector->memorySize = vector->memorySize * 2;
    }
    
    for (int i = vector->size; i > location; i--) {
        vector->array[i] = vector->array[i-1];
    }
    vector->array[location] = value;
    vector->size++;
    return 1;
}


/* Obtains value at location inside the Vector. Returns 1 if success, and 0 if
 the location is invalid (less than 0, or greater than or equal to the
 size). The value itself is returned through the parameter value. */

int get(Vector *vector, int location, int *value) {
    if (location >= vector->size || location < 0) {
        return 0;
    }
    else {
        *value = vector->array[location];
        return 1;
    }
}


/* Deletes value at location inside the Vector.  Returns 1 if success, and 0 if
 the location is invalid (less than 0, or greater than or equal to the
 size). When an item is deleted, everything else past it in the array should be
 slid down. The internal array itself does not need to be compressed, i.e., you
 don't need to halve it in size if it becomes half full, or anything like
 that. */

int delete(Vector *vector, int location) {
    if (location >= vector->size || location < 0) {
        return 0;
    }
    
    for (int i = location; i < vector->size - 1; i++) {
        vector->array[i] = vector->array[i+1];
    }
    vector->size--;
    return 1;
}

// int main() {
//     Vector *vec = (Vector *) malloc(sizeof(Vector));
//     init(vec, 10);
//     for (int i=0; i < 50; i++) {
//         insert(vec,0,i);
//     }
//     int value;
//     for (int i=0; i < 50; i++) {
//         int status = get(vec,50-i-1,&value);
//         printf("%d\n",value);
//     }
//     free(vec->array);
//     free(vec);
//     return 0;
// }

