#ifndef _VALUE
#define _VALUE


typedef enum {
   INT_TYPE, DOUBLE_TYPE, STR_TYPE, CONS_TYPE, NULL_TYPE, PTR_TYPE,
   OPEN_TYPE, CLOSE_TYPE, BOOL_TYPE, SYMBOL_TYPE,
    
   // Types below are only for bonus work
   OPENBRACKET_TYPE, CLOSEBRACKET_TYPE, DOT_TYPE, SINGLEQUOTE_TYPE
} valueType;

struct Value {
   valueType type;
   union {
      int i;
      double d;
      char *s;
      void *p;
      struct ConsCell {
         struct Value *car;
         struct Value *cdr;
      } c;
   };
};
// Where do you store a BOOL_TYPE boolean in the union? I recommend the int i
// (0 for false, 1 for true).

typedef struct Value Value;

#endif
