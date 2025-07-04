#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    unsigned int seed;
    printf("Enter a random seed: ");
    scanf("%u", &seed);
    srandom(seed);
    //srandom((unsigned int)time(NULL));
    int randNum = random()%100 + 1;

    int guess;
    int count = 0;
    while (guess != randNum) {
        printf("Guess a number: ");
        scanf("%d",&guess);
        count++;
        if (guess > randNum) {
            printf("Too high!\n");
        }
        else if (guess < randNum) {
            printf("Too low!\n");
        }
    }
    printf("Correct! Total guesses = %d\n", count);
    return 0;
}