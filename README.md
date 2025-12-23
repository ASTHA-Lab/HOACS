# HOACS: Homomorphic Obfuscation Assisted Concealing of Secrets

**A Lightweight Software-Based Framework for Protecting Cryptographic Secrets Against Hardware Trojans in Untrusted Processors**

[![License](https://img.shields.io/badge/License-Research-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-COTS%20Processors-green.svg)]()

---

## Overview

HOACS is a novel software-oriented countermeasure that safeguards the confidentiality of secret assets against Hardware Trojans (HTs) in Commercial-Off-The-Shelf (COTS) microprocessors. Unlike existing countermeasures that require trust in the design house or foundry, HOACS operates under a **zero-trust model** and requires no modifications to the IC design.

The framework utilizes **Residue Number Coding (RNC)** to transform standard C-based software functions into homomorphically obfuscated functions, ensuring that sensitive data (e.g., cryptographic keys, passwords, private data) remains protected even when executed on untrusted hardware.

### Key Innovation

HOACS enables programs to execute while keeping sensitive data in an **encoded form** throughout critical execution phases. Hardware Trojans that attempt to snoop on internal registers or side channels can only access unintelligible encoded values, effectively preventing data leakage.

---

## Authors

**Tanvir Hossain**, **Matthew Showers**, **Mahmudul Hasan**, **Tamzidul Hoque**

*University of Kansas, Lawrence, KS 66045, USA*

*Emails: {tanvir, mds2016, m.hasan, hoque}@ku.edu*

*This repository contains the implementation of research published in:*

**"On the Security of Lightweight Homomorphic Obfuscation for Protecting Secrets Against Hardware Trojans"**

*Accepted to IACR Transactions on Cryptographic Hardware and Embedded Systems (CHES 2026)*

---

## Key Features

- **Zero-Trust Architecture**: No trust required in any supply chain entity (design house, foundry, testing facility)
- **Software-Only Solution**: No hardware modifications needed
- **Lightweight Homomorphic Protection**: Significantly lower overhead compared to Fully Homomorphic Encryption (FHE)
- **LLVM Integration**: HOACS-IR plugin for automated code transformation
- **Side-Channel Resistance**: Inherent protection against power analysis attacks
- **COTS Compatibility**: Designed specifically for Commercial-Off-The-Shelf processors
- **Practical Performance**: 4-26× overhead vs. 10³-10¹⁰× for traditional FHE schemes

---

## What is Residue Number Coding (RNC)?

RNC is derived from the **Residue Number System (RNS)**, a non-weighted number representation system that encodes integers as a set of remainders modulo pairwise coprime integers (moduli).

### Core Concept

Instead of representing a number `v` in binary form, RNC represents it as:

```
v = (v₁, v₂, ..., vᵤ)
```

where each component `vᵢ = v mod mᵢ`, and `m₁, m₂, ..., mᵤ` are pairwise coprime moduli.

### Example

Given moduli `m₁ = 17` and `m₂ = 19`:
- Number `29` is encoded as `(12, 10)` since:
  - `29 mod 17 = 12`
  - `29 mod 19 = 10`

### Homomorphic Properties

RNC supports homomorphic operations directly on encoded values:

| Operation | Homomorphic Execution |
|-----------|----------------------|
| **Addition** | `(a₁, a₂) + (b₁, b₂) = ((a₁+b₁) mod m₁, (a₂+b₂) mod m₂)` |
| **Subtraction** | `(a₁, a₂) - (b₁, b₂) = ((a₁-b₁) mod m₁, (a₂-b₂) mod m₂)` |
| **Multiplication** | `(a₁, a₂) × (b₁, b₂) = ((a₁×b₁) mod m₁, (a₂×b₂) mod m₂)` |

### Decoding with Chinese Remainder Theorem (CRT)

To recover the original value from RNC components:

```
v = (Σᵢ Mᵢ · ⟨αᵢ·vᵢ⟩ₘᵢ) mod M
```

where:
- `M = m₁ × m₂ × ... × mᵤ` (dynamic range)
- `Mᵢ = M / mᵢ`
- `αᵢ = Mᵢ⁻¹ mod mᵢ` (multiplicative inverse)

### Security Properties

1. **Data Obfuscation**: Without knowledge of the moduli, encoded values are computationally infeasible to decode
2. **Side-Channel Resistance**: Operations on different residue components create randomized power signatures
3. **Parallel Processing**: Each residue component can be processed independently

### Dynamic Range

The system can represent integers in the range `[0, M-1]` where `M` is the product of all moduli. For example:
- Moduli: `[97, 101, 103, 107, 109, 113]`
- Dynamic Range: `M = 1,329,900,201,629` (~40 bits)

---

## Implementation Components

### 1. HOACS-C Library

A comprehensive C library providing:
- **RNC Component**: Basic encoding, decoding, and homomorphic operations
- **RNC-Tree Component**: Binary search tree with RNC-encoded values
- **RNC-Grid Component**: Constant-time lookup data structures
- **Supported Operations**: Addition, subtraction, multiplication, XOR, comparison, exponentiation

### 2. HOACS-IR (LLVM Plugin)

Automates RNC transformation at the LLVM Intermediate Representation level:
- Identifies sensitive variables by prefix annotation (`rnc_*`)
- Automatically replaces operations with RNC equivalents
- Performs compile-time encoding of constants
- Minimal source code modifications required

### 3. Security Analysis Tools

- **Register Trace Analyzer**: Detects plaintext key exposure in CPU registers
- **TVLA Framework**: Test Vector Leakage Assessment for side-channel evaluation
- **Side-Channel Correlation Analysis**: Validates protection against power analysis

---

## Quick Start

### Basic RNC Usage (HOACS-C)

```c
#include "RNC.h"

int main() {
    // Define moduli (must be pairwise coprime)
    uint32_t m1 = 17, m2 = 19;

    // Encode sensitive values
    unsigned int secret_key = 42;
    aEnc = encode(secret_key, m1, m2);

    // Perform homomorphic operations
    bEnc = encode(17, m1, m2);
    cEnc = AddEnc(aEnc, bEnc, m1, m2);

    // Decode result
    uint32_t result = decode(cEnc, m1, m2, NULL, NULL);
    // result = 59 (42 + 17)

    return 0;
}
```

### Protected AES Key Expansion

```c
// Encode AES key before processing
uint8_t plain_key[16] = {...};
Encoded key_enc[16];

for (int i = 0; i < 16; i++) {
    key_enc[i] = encode(plain_key[i], m1, m2);
}

// Key expansion operates on encoded values
KeyExpansion(key_enc, round_keys, m1, m2);

// Plaintext key never appears in registers during expansion
```

---

## Security Guarantees

### Protection Against Functional Hardware Trojans

- **Register Protection**: Plaintext keys never appear in CPU registers during critical operations
- **Exposure Window**: <5% of total execution time (only during init/decode phases)
- **Experimental Validation**: Verified on x86 (gem5 simulator) and RISC-V processors

### Protection Against Side-Channel Trojans

- **Power Randomization**: Different moduli configurations produce uncorrelated power traces
- **Correlation Analysis**: Pearson correlation coefficients <0.2 between execution variants
- **MOLES Resistance**: Effectively disrupts side-channel leakage trojans

### Brute-Force Attack Resistance

Time complexity for unknown moduli attack: `O(Mᵏ × k × log²(M))`

For typical configurations (M = 2³², k = 6):
- Search space: ~10¹⁹² possible moduli combinations
- Computationally infeasible even with advanced computing resources

---

## Performance

### Overhead Comparison

| Framework | Addition | Subtraction | Multiplication |
|-----------|----------|-------------|----------------|
| PyAono | 364897× | - | 97222× |
| SEAL | 228526× | 61666× | 527222× |
| HElib | 2888× | - | 26.2× |
| Paillier | 3000× | - | - |
| ElGamal | - | - | 17222× |
| **HOACS-C** | **26.04×** | **26.29×** | **25.47×** |
| **HOACS-IR** | **4.42×** | **3.97×** | **4.16×** |

*HOACS-IR achieves 10³-10⁶× better performance than traditional FHE schemes*

---

## Applications

- **Cryptographic Key Protection**: AES, RSA, elliptic curve cryptography
- **Secure Embedded Systems**: IoT devices, automotive ECUs, medical implants
- **Defense Systems**: Military hardware with untrusted COTS components
- **Privacy-Preserving Machine Learning**: Future extension for PPML workloads

---

## Citation

If you use HOACS in your research, please cite:

```bibtex
@article{hoacs2025,
  title={On the Security of Lightweight Homomorphic Obfuscation for Protecting Secrets Against Hardware Trojans},
  journal={IACR Transactions on Cryptographic Hardware and Embedded Systems},
  year={2025},
  note={Submitted}
}
```

---

## License

This project is licensed under the **MIT License**.

Copyright (c) 2025 **ASTHA Lab, University of Kansas**. All rights reserved.

See the [LICENSE](LICENSE) file for full details.

For commercial use or collaboration inquiries, please contact the authors.

---

## Acknowledgments

This research addresses critical security challenges in the COTS IC supply chain, where 72% of U.S. Department of Defense ICs are COTS components.

---

## Contact

For questions, issues, or collaboration opportunities, please open an issue on this repository.