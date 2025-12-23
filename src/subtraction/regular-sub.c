#include <stdio.h>
#include <stdint.h>

void print_subtraction_demo() {
    printf("Regular Subtraction Operations\n");
    printf("===============================\n\n");

    // Test cases for subtraction
    uint8_t a1 = 59, b1 = 17;
    uint8_t a2 = 150, b2 = 50;
    uint8_t a3 = 255, b3 = 100;
    uint8_t a4 = 192, b4 = 64;
    uint8_t a5 = 200, b5 = 75;

    printf("Test 1: %u - %u = %u\n", a1, b1, a1 - b1);
    printf("Test 2: %u - %u = %u\n", a2, b2, a2 - b2);
    printf("Test 3: %u - %u = %u\n", a3, b3, a3 - b3);
    printf("Test 4: %u - %u = %u\n", a4, b4, a4 - b4);
    printf("Test 5: %u - %u = %u\n\n", a5, b5, a5 - b5);

    printf("Note: All operations performed on plaintext values\n");
    printf("      Vulnerable to Hardware Trojans snooping on registers\n\n");
}

int main() {
    print_subtraction_demo();
    return 0;
}
