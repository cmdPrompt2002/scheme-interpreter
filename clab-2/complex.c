#include<stdio.h>


struct Complex {
    double real;
    double imaginary;
};

int main() {
    struct Complex c1;
    struct Complex c2;

    printf("Enter real part of c1: ");
    scanf("%lf",&(c1.real));

    printf("Enter imaginary part of c1: ");
    scanf("%lf",&(c1.imaginary));

    printf("Enter real part of c2: ");
    scanf("%lf",&(c2.real));

    printf("Enter imaginary part of c2: ");
    scanf("%lf",&(c2.imaginary));

    double real = (c1.real * c2.real) - (c1.imaginary * c2.imaginary);
    double imaginary = (c1.real * c2.imaginary) + (c1.imaginary * c2.real);

    printf("Answer = %10.2f + %10.2f i\n", real, imaginary);
    return 0;
}

