#include <stdio.h>
#include <math.h>

void exchange();
void exchangePlusMinus();
void seconds_to_time();
void time_to_seconds();
void implicationTruthTable();
void equivalenceTruthTable();
void power();
void cypher();
void cypherAlphabet();

int main() {
    cypherAlphabet();
    return 0;
}

void exchange() {
    int a, b;
    printf("Enter the first number : ");
    scanf("%d", &a);
    printf("Enter the second number : ");
    scanf("%d", &b);

    printf("The first number is %d and the second number is %d\n", a, b);

    int temp = a;
    a = b;
    b = temp;

    printf("The first number is %d and the second number is %d\n", a, b);
}

void exchangePlusMinus() {   
    int a, b, diff;
    printf("Enter the first number : ");
    scanf("%d", &a);
    printf("Enter the second number : ");
    scanf("%d", &b);

    printf("The first number is %d and the second number is %d\n", a, b);
    diff = a-b;
    a -= diff;
    b += diff;
    printf("The first number is %d and the second number is %d\n", a, b);
}

void seconds_to_time() {
    int seconds;
    printf("Enter the number of seconds : ");
    scanf("%d", &seconds);

    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int seconds_left = seconds % 60;

    printf("%d seconds = %02d hours, %02d minutes and %02d seconds\n", seconds, hours, minutes, seconds_left);
}

void time_to_seconds() {
    int hours, minutes, seconds;
    printf("Enter the number of hours : ");
    scanf("%d", &hours);
    printf("Enter the number of minutes : ");
    scanf("%d", &minutes);
    printf("Enter the number of seconds : ");
    scanf("%d", &seconds);

    int total_seconds = hours * 3600 + minutes * 60 + seconds;

    printf("%02d hours, %02d minutes and %02d seconds = %d seconds\n", hours, minutes, seconds, total_seconds);
}

void implicationTruthTable() {
    printf("Implication (x => y) Truth Table:\n");
    printf("| x | y | x => y |\n");
    printf("|---|---|--------|\n");

    int x, y;

    for (x = 0; x <= 1; x++) {
        for (y = 0; y <= 1; y++) {
            int result = !x || y;  
            printf("| %d | %d |   %d    |\n", x, y, result);
        }
    }
}

void equivalenceTruthTable() {
    printf("\nEquivalence (x <=> y) Truth Table:\n");
    printf("| x | y | x <=> y |\n");
    printf("|---|---|---------|\n");

    int x, y;

    for (x = 0; x <= 1; x++) {
        for (y = 0; y <= 1; y++) {
            int result = (x && y) || (!x && !y); 
            printf("| %d | %d |    %d    |\n", x, y, result);
        }
    }
}

void power(){
    float x, n;
    printf("Enter the number : ");
    scanf("%f", &x);
    printf("Enter the power : ");
    scanf("%f", &n);
    float power = pow(x, n);

    printf("%.0f^%.0f = %.0f\n", x, n, power);
}

void cypher(){
    char c;
    printf("Enter a character : ");
    scanf("%c", &c);
    c = (c - 'A' + 3)%26 + 'A';
    printf("The cyphered letter is %c\n", c);
}

void cypherAlphabet(){
    for (char c = 'A'; c <= 'Z'; c++) {
        char cypherLetter = (c - 'A' + 3)%26 + 'A';
        printf("%c", cypherLetter);
    }
}