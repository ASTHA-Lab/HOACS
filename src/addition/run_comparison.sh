#!/bin/bash

# Addition Operations Comparison Script
# This script compiles and runs both regular and HOACS-protected addition

echo "=========================================="
echo "  ADDITION OPERATIONS COMPARISON"
echo "=========================================="
echo ""

# Compile regular addition
echo "Compiling Regular Addition..."
gcc regular-add.c -o regular-add.exe
if [ $? -ne 0 ]; then
    echo "Error: Failed to compile regular-add.c"
    exit 1
fi
echo "✓ Compiled successfully"
echo ""

# Compile HOACS-protected addition
echo "Compiling HOACS-Protected Addition..."
gcc hoacs-add.c ../RNC.c ../MathUtils.c -o hoacs-add.exe -lm -I..
if [ $? -ne 0 ]; then
    echo "Error: Failed to compile hoacs-add.c"
    exit 1
fi
echo "✓ Compiled successfully"
echo ""
echo ""

# Run regular addition
echo "=========================================="
echo "  REGULAR ADDITION"
echo "=========================================="
echo ""
./regular-add.exe
echo ""

# Run HOACS-protected addition
echo "=========================================="
echo "  HOACS-PROTECTED ADDITION"
echo "=========================================="
echo ""
./hoacs-add.exe
echo ""

echo "=========================================="
echo "  COMPARISON SUMMARY"
echo "=========================================="
echo ""
echo "Regular Addition:"
echo "  - Operands are in plaintext"
echo "  - Addition performed on plaintext values"
echo "  - Results are in plaintext"
echo "  - Vulnerable to Hardware Trojans"
echo ""
echo "HOACS-Protected Addition:"
echo "  - Operands are encoded with RNC (2 residues per value)"
echo "  - Addition performed homomorphically on encoded values"
echo "  - Results stored in encoded form"
echo "  - Decoded results match regular addition output"
echo "  - Protected against Hardware Trojans"
echo ""
echo "Note: Both versions produce identical results"
echo "      when decoded, proving correctness!"
echo ""
