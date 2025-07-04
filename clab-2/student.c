/* Example of C structs. */

#include <stdio.h>

/* A structure type representing a student. */
struct Student {
    char *first_name;
    char *last_name;
    int id;
};

void print_student(struct Student s) {
    printf("%d: %s %s\n", s.id, s.first_name, s.last_name);
}

int main() {
    /* Create an instance of the Student struct. */
    struct Student s1;
    s1.first_name = "Bart";
    s1.last_name = "Simpson";
    s1.id = 666;
    print_student(s1);

    return 0;
}