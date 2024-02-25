#include <stdio.h>

void discount();
void sortNumbers();
void starsV1();
void starsV2();
void findMinMax(float arr[], int n);
void invertArray(int arr[], int n);
void printArray(int arr[], int n);

int main(void) {
    starsV2();
    return 0;
}

void discount() {
    short price, discountedPrice;
    printf("Input a price : ");
    scanf("%d", &price);

    if (100 <= price <= 500) {
        discountedPrice =  price * 0.95;
    } else if (500 < price) {
        discountedPrice =  price * 0.92;
    } else {
        discountedPrice = price;
    }

    printf("Discounted price : %d", discountedPrice);
}

#include <stdio.h>

void sortNumbers() {
    float x, y, z, temp;

    printf("Enter three real numbers: ");
    scanf("%f %f %f", &x, &y, &z);

    if (x > y) {
        temp = x;
        x = y;
        y = temp;
    }
    if (x > z) {
        temp = x;
        x = z;
        z = temp;
    }
    if (y > z) {
        temp = y;
        y = z;
        z = temp;
    }

    printf("Numbers in ascending order: %.2f %.2f %.2f\n", x, y, z);
}

void starsV1() {
    int i;
    printf("Value: ");
    scanf("%d", &i);

    for (int j = i; j > 0; --j) {
        for (int k = 0; k < j; ++k) {
            printf("*");
        }
        printf("\n");
    }
}

void starsV2() {
    int i;
    printf("Value: ");
    scanf("%d", &i);

    for (int j = 1; j <= i; ++j) {
        for (int k = 0; k < i-j; ++k) {
            printf(" ");
        }
        for (int k = 0; k < j*2-1; ++k) {
            printf("*");
        }
        printf("\n");
    }
}

void findMinMax(float arr[], int n) {
    float min = arr[0];
    float max = arr[0];
    int i;

    for (i = 0; i < n; i++) {
        if (arr[i] < min) {
            min = arr[i];
        }
        if (arr[i] > max) {
            max = arr[i];
        }
    }

    printf("Minimum = %.2f\n", min);
    printf("Maximum = %.2f\n", max);
}

void invertArray(int arr[], int n) {
    int start = 0;
    int end = n - 1;
    int temp;

    while (start < end) {
        temp = arr[start];
        arr[start] = arr[end];
        arr[end] = temp;
        start++;
        end--;
    }
}

void printArray(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}