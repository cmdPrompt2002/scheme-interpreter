#include <stdio.h>

int main() {
    float temp_f;
    printf("What is the temperature in degrees Fahrenheit? ");
    scanf("%f", &temp_f);
    if (temp_f < -459.67) {
        printf("Invalid temperature.");
    }
    else {
        float temp_c = (temp_f - 32) * 5/9;
        printf("%f degrees Fahrenheit is %f degrees Celsius.", temp_f, temp_c);
    }
    
    return 0;
}