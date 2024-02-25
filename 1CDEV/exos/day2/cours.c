#include <stdio.h>
#include <stdbool.h>

int main() {
    int number;
    int sum = 0;

    while (true) {
        printf("Enter a number: ");
        scanf("%d", &number);
        if (number == 0) {
            break;
        }
        sum += number;
    }

    printf("Sum = %d\n", sum);
}