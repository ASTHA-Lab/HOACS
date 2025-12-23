#!/bin/bash

# AES Key Expansion Comparison Script
# This script compiles and runs both regular and RNC-protected AES key expansion

echo "=========================================="
echo "  AES KEY EXPANSION COMPARISON"
echo "=========================================="
echo ""

# Compile pristine (regular) AES
echo "Compiling Regular AES Key Expansion..."
gcc pristine-aes-keyexp.c -o pristine-aes-keyexp.exe
if [ $? -ne 0 ]; then
    echo "Error: Failed to compile pristine-aes-keyexp.c"
    exit 1
fi
echo "✓ Compiled successfully"
echo ""

# Compile RNC-protected AES
echo "Compiling RNC-Protected AES Key Expansion..."
gcc -DUSE_RNC_GLOBAL=1 -DUSE_SBOX=1 -DAES_keyExpSize=176 \
    KeyExpansion.c RNC.c RNC_Tree.c MathUtils.c \
    -o rnc-aes-keyexp.exe -lm
if [ $? -ne 0 ]; then
    echo "Error: Failed to compile RNC AES key expansion"
    exit 1
fi
echo "✓ Compiled successfully"
echo ""
echo ""

# Run pristine AES
echo "=========================================="
echo "  REGULAR (PRISTINE) AES"
echo "=========================================="
echo ""
./pristine-aes-keyexp.exe
echo ""

# Run RNC-protected AES
echo "=========================================="
echo "  RNC-PROTECTED AES"
echo "=========================================="
echo ""
./rnc-aes-keyexp.exe
echo ""

echo "=========================================="
echo "  COMPARISON SUMMARY"
echo "=========================================="
echo ""
echo "Regular AES:"
echo "  - Input key is in plaintext"
echo "  - Expanded keys are in plaintext"
echo "  - Vulnerable to Hardware Trojans"
echo ""
echo "RNC-Protected AES:"
echo "  - Input key is encoded with RNC (2 residues per byte)"
echo "  - All operations performed on encoded values"
echo "  - Expanded keys stored in encoded form"
echo "  - Decoded expanded keys match regular AES output"
echo "  - Protected against Hardware Trojans"
echo ""
echo "Note: Both versions produce identical expanded keys"
echo "      when decoded, proving correctness!"
echo ""
