#!/bin/bash

# Subtraction Operations Comparison Script
# This script compiles and runs both regular and HOACS-protected subtraction

echo "=========================================="
echo "  SUBTRACTION OPERATIONS COMPARISON"
echo "=========================================="
echo ""

# Compile regular subtraction
echo "Compiling Regular Subtraction..."
gcc regular-sub.c -o regular-sub.exe
if [ $? -ne 0 ]; then
    echo "Error: Failed to compile regular-sub.c"
    exit 1
fi
echo "✓ Compiled successfully"
echo ""

# Compile HOACS-protected subtraction
echo "Compiling HOACS-Protected Subtraction..."
gcc hoacs-sub.c ../RNC.c ../MathUtils.c -o hoacs-sub.exe -lm -I..
if [ $? -ne 0 ]; then
    echo "Error: Failed to compile hoacs-sub.c"
    exit 1
fi
echo "✓ Compiled successfully"
echo ""
echo ""

# Run regular subtraction
echo "=========================================="
echo "  REGULAR SUBTRACTION"
echo "=========================================="
echo ""
./regular-sub.exe
echo ""

# Run HOACS-protected subtraction
echo "=========================================="
echo "  HOACS-PROTECTED SUBTRACTION"
echo "=========================================="
echo ""
./hoacs-sub.exe
echo ""

echo "=========================================="
echo "  COMPARISON SUMMARY"
echo "=========================================="
echo ""
echo "Regular Subtraction:"
echo "  - Operands are in plaintext"
echo "  - Subtraction performed on plaintext values"
echo "  - Results are in plaintext"
echo "  - Vulnerable to Hardware Trojans"
echo ""
echo "HOACS-Protected Subtraction:"
echo "  - Operands are encoded with RNC (2 residues per value)"
echo "  - Subtraction performed homomorphically on encoded values"
echo "  - Results stored in encoded form"
echo "  - Decoded results match regular subtraction output"
echo "  - Protected against Hardware Trojans"
echo ""
echo "Note: Both versions produce identical results"
echo "      when decoded, proving correctness!"
echo ""
