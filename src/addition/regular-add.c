#include <stdio.h>
#include <stdint.h>

void print_addition_demo() {
    printf("Regular Addition Operations\n");
    printf("============================\n\n");

    // Test cases for addition
    uint8_t a1 = 42, b1 = 17;
    uint8_t a2 = 100, b2 = 50;
    uint8_t a3 = 255, b3 = 0;
    uint8_t a4 = 128, b4 = 64;
    uint8_t a5 = 75, b5 = 83;

    printf("Test 1: %u + %u = %u\n", a1, b1, a1 + b1);
    printf("Test 2: %u + %u = %u\n", a2, b2, a2 + b2);
    printf("Test 3: %u + %u = %u\n", a3, b3, a3 + b3);
    printf("Test 4: %u + %u = %u\n", a4, b4, a4 + b4);
    printf("Test 5: %u + %u = %u\n\n", a5, b5, a5 + b5);

    printf("Note: All operations performed on plaintext values\n");
    printf("      Vulnerable to Hardware Trojans snooping on registers\n\n");
}

int main() {
    print_addition_demo();
    return 0;
}
