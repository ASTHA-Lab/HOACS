#include <stdio.h>
#include <stdint.h>
#include "../RNC.h"
#include "../MathUtils.h"

void print_hoacs_subtraction_demo() {
    printf("HOACS-Protected Subtraction Operations\n");
    printf("=======================================\n\n");

    // Define moduli (must be coprime)
    uint8_t m1 = 17;
    uint8_t m2 = 19;

    printf("Using RNC with moduli: m1=%u, m2=%u\n", m1, m2);
    printf("Dynamic Range: [0, %u]\n\n", m1 * m2 - 1);

    // Test cases for subtraction (same as regular)
    uint8_t a1 = 59, b1 = 17;
    uint8_t a2 = 150, b2 = 50;
    uint8_t a3 = 255, b3 = 100;
    uint8_t a4 = 192, b4 = 64;
    uint8_t a5 = 200, b5 = 75;

    // Test 1
    printf("Test 1:\n");
    printf("  Input (Plain): a=%u, b=%u\n", a1, b1);

    Encoded a1_enc = encode(a1, m1, m2);
    Encoded b1_enc = encode(b1, m1, m2);
    printf("  Encoded: a=(%u,%u), b=(%u,%u)\n",
           a1_enc.u1, a1_enc.u2, b1_enc.u1, b1_enc.u2);

    Encoded c1_enc = SubEnc(a1_enc, b1_enc, m1, m2);
    printf("  Result (Encoded): c=(%u,%u)\n", c1_enc.u1, c1_enc.u2);

    uint8_t c1 = decode(c1_enc, m1, m2, NULL, NULL);
    printf("  Result (Decoded): c=%u\n", c1);
    printf("  Expected: %u (matches: %s)\n\n", a1 - b1, c1 == (a1 - b1) ? "YES" : "NO");

    // Test 2
    printf("Test 2:\n");
    printf("  Input (Plain): a=%u, b=%u\n", a2, b2);

    Encoded a2_enc = encode(a2, m1, m2);
    Encoded b2_enc = encode(b2, m1, m2);
    printf("  Encoded: a=(%u,%u), b=(%u,%u)\n",
           a2_enc.u1, a2_enc.u2, b2_enc.u1, b2_enc.u2);

    Encoded c2_enc = SubEnc(a2_enc, b2_enc, m1, m2);
    printf("  Result (Encoded): c=(%u,%u)\n", c2_enc.u1, c2_enc.u2);

    uint8_t c2 = decode(c2_enc, m1, m2, NULL, NULL);
    printf("  Result (Decoded): c=%u\n", c2);
    printf("  Expected: %u (matches: %s)\n\n", a2 - b2, c2 == (a2 - b2) ? "YES" : "NO");

    // Test 3
    printf("Test 3:\n");
    printf("  Input (Plain): a=%u, b=%u\n", a3, b3);

    Encoded a3_enc = encode(a3, m1, m2);
    Encoded b3_enc = encode(b3, m1, m2);
    printf("  Encoded: a=(%u,%u), b=(%u,%u)\n",
           a3_enc.u1, a3_enc.u2, b3_enc.u1, b3_enc.u2);

    Encoded c3_enc = SubEnc(a3_enc, b3_enc, m1, m2);
    printf("  Result (Encoded): c=(%u,%u)\n", c3_enc.u1, c3_enc.u2);

    uint8_t c3 = decode(c3_enc, m1, m2, NULL, NULL);
    printf("  Result (Decoded): c=%u\n", c3);
    printf("  Expected: %u (matches: %s)\n\n", a3 - b3, c3 == (a3 - b3) ? "YES" : "NO");

    // Test 4
    printf("Test 4:\n");
    printf("  Input (Plain): a=%u, b=%u\n", a4, b4);

    Encoded a4_enc = encode(a4, m1, m2);
    Encoded b4_enc = encode(b4, m1, m2);
    printf("  Encoded: a=(%u,%u), b=(%u,%u)\n",
           a4_enc.u1, a4_enc.u2, b4_enc.u1, b4_enc.u2);

    Encoded c4_enc = SubEnc(a4_enc, b4_enc, m1, m2);
    printf("  Result (Encoded): c=(%u,%u)\n", c4_enc.u1, c4_enc.u2);

    uint8_t c4 = decode(c4_enc, m1, m2, NULL, NULL);
    printf("  Result (Decoded): c=%u\n", c4);
    printf("  Expected: %u (matches: %s)\n\n", a4 - b4, c4 == (a4 - b4) ? "YES" : "NO");

    // Test 5
    printf("Test 5:\n");
    printf("  Input (Plain): a=%u, b=%u\n", a5, b5);

    Encoded a5_enc = encode(a5, m1, m2);
    Encoded b5_enc = encode(b5, m1, m2);
    printf("  Encoded: a=(%u,%u), b=(%u,%u)\n",
           a5_enc.u1, a5_enc.u2, b5_enc.u1, b5_enc.u2);

    Encoded c5_enc = SubEnc(a5_enc, b5_enc, m1, m2);
    printf("  Result (Encoded): c=(%u,%u)\n", c5_enc.u1, c5_enc.u2);

    uint8_t c5 = decode(c5_enc, m1, m2, NULL, NULL);
    printf("  Result (Decoded): c=%u\n", c5);
    printf("  Expected: %u (matches: %s)\n\n", a5 - b5, c5 == (a5 - b5) ? "YES" : "NO");

    printf("Note: All operations performed on ENCODED values\n");
    printf("      Hardware Trojans can only see obfuscated residues\n");
    printf("      Original values remain hidden throughout computation\n\n");
}

int main() {
    print_hoacs_subtraction_demo();
    return 0;
}
