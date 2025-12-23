/*****************************************************************************/
/* Includes:                                                                 */
/*****************************************************************************/
#include <stdio.h>
#include <string.h> // CBC mode, for memset
#include "aes.h"

/*****************************************************************************/
/* Defines:                                                                  */
/*****************************************************************************/
// The number of columns comprising a state in AES. This is a constant in AES. Value=4
#define Nb 4

#if defined(AES256) && (AES256 == 1)
    #define Nk 8
    #define Nr 14
#elif defined(AES192) && (AES192 == 1)
    #define Nk 6
    #define Nr 12
#else
    #define Nk 4        // The number of 32 bit words in a key.
    #define Nr 10       // The number of rounds in AES Cipher.
#endif

// jcallan@github points out that declaring Multiply as a function 
// reduces code size considerably with the Keil ARM compiler.
// See this link for more information: https://github.com/kokke/tiny-AES-C/pull/3
#ifndef MULTIPLY_AS_A_FUNCTION
  #define MULTIPLY_AS_A_FUNCTION 0
#endif


/*****************************************************************************/
/* Private variables:                                                        */
/*****************************************************************************/
// state - array holding the intermediate results during decryption.
#if USE_RNC_STATE == 1
typedef Encoded state_t[4][4];
#else
typedef uint8_t state_t[4][4];
#endif

// The lookup-tables are marked const so they can be placed in read-only storage instead of RAM
// The numbers below can be computed dynamically trading ROM for RAM - 
// This can be useful in (embedded) bootloader applications, where ROM is often limited.
static const uint8_t sbox[256] = {
  //0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

#if USE_RNC_GLOBAL == 1
static Encoded sboxEnc[256];

  #if USE_SBOX == SBOX_TREE
    static RNC_Node*  sboxEncTree = NULL;
  #elif USE_SBOX == SBOX_GRID
    static RNC_Grid*  sboxEncGrid = NULL;
  #endif

#endif

#if (defined(CBC) && CBC == 1) || (defined(ECB) && ECB == 1)
static const uint8_t rsbox[256] = {
  0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
  0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
  0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
  0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
  0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
  0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
  0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
  0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
  0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
  0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
  0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
  0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
  0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
  0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
  0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
  0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };
#endif

// The round constant word array, Rcon[i], contains the values given by 
// x to the power (i-1) being powers of x (x is denoted as {02}) in the field GF(2^8)
static const uint8_t Rcon[11] = {
  0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36 };

/*
 * Jordan Goulder points out in PR #12 (https://github.com/kokke/tiny-AES-C/pull/12),
 * that you can remove most of the elements in the Rcon array, because they are unused.
 *
 * From Wikipedia's article on the Rijndael key schedule @ https://en.wikipedia.org/wiki/Rijndael_key_schedule#Rcon
 * 
 * "Only the first some of these constants are actually used – up to rcon[10] for AES-128 (as 11 round keys are needed), 
 *  up to rcon[8] for AES-192, up to rcon[7] for AES-256. rcon[0] is not used in AES algorithm."
 */

/*****************************************************************************/
/* Private functions:                                                        */
/*****************************************************************************/
/*
static uint8_t getSBoxValue(uint8_t num)
{
  return sbox[num];
}
*/
#define getSBoxValue(num) (sbox[(num)])

#if USE_RNC_GLOBAL == 1
  #if USE_SBOX == SBOX_DECD
    #define getSBoxEncVal(num,m1,m2) (sboxEnc[decode((num),(m1),(m2),NULL,NULL)]) // Decodes to index into SBoxEnc Array
  #elif USE_SBOX == SBOX_TREE
    #define getSBoxEncVal(num,m1,m2) (*rncTreeAt(sboxEncTree,(num),(m1),(m2))) // Uses Encoded comparisons to retrieve value from sboxRncTree
  #elif USE_SBOX == SBOX_OFFS
   #define getSBoxEncVal(num,m1,m2) (*((Encoded*)OffEnc(sboxEnc,(num),(m1),(m2)))) // Homomorphically computes target address // FIXME segfaults
  #elif USE_SBOX == SBOX_GRID
    #define getSBoxEncVal(num,m1,m2) (*readRNC_Grid(sboxEncGrid,(num))) // indexes into 2D array
  #endif
#endif

static void decodeRoundKey(const Encoded* RoundKey, uint8_t m1, uint8_t m2, uint8_t* rKeyDec)
{
  for(uint8_t i = 0; i < AES_keyExpSize; i++)
  {
    rKeyDec[i] = decode(RoundKey[i],m1,m2,NULL,NULL);
  }
}

// This function produces Nb(Nr+1) round keys. The round keys are used in each round to decrypt the states. 
static void KeyExpansion(Encoded* RoundKey, const Encoded* Key, uint8_t m1, uint8_t m2)
{
  unsigned i, j, k;
  Encoded tempa[4]; // Used for the column/row operations
  //printf("The key is : %x | %x \n",Key->u1,Key->u2);
  //printf("The In Round key is : %x | %x \n",RoundKey->u1,RoundKey->u2);
  // The first round key is the key itself.
  for (i = 0; i < Nk; ++i)
  {
    RoundKey[(i * 4) + 0] = Key[(i * 4) + 0];
    //printf("Round Key1 is : %x | %x\n", RoundKey->u1, RoundKey->u2);
    RoundKey[(i * 4) + 1] = Key[(i * 4) + 1];
    //printf("Round Key1 is : %x | %x\n", RoundKey->u1, RoundKey->u2);
    RoundKey[(i * 4) + 2] = Key[(i * 4) + 2];
    //printf("Round Key1 is : %x | %x\n", RoundKey->u1, RoundKey->u2);
    RoundKey[(i * 4) + 3] = Key[(i * 4) + 3];
    //printf("Round Key1 is : %x | %x\n", RoundKey->u1, RoundKey->u2);
  }

  // All other round keys are found from the previous round keys.
  for (i = Nk; i < Nb * (Nr + 1); ++i)
  {
    {
      k = (i - 1) * 4;
      tempa[0]=RoundKey[k + 0];
      tempa[1]=RoundKey[k + 1];
      tempa[2]=RoundKey[k + 2];
      tempa[3]=RoundKey[k + 3];

    }

    if (i % Nk == 0)
    {
      // This function shifts the 4 bytes in a word to the left once.
      // [a0,a1,a2,a3] becomes [a1,a2,a3,a0]

      // Function RotWord()
      {
        const Encoded u8tmp = tempa[0];
        tempa[0] = tempa[1];
        tempa[1] = tempa[2];
        tempa[2] = tempa[3];
        tempa[3] = u8tmp;
      }

      // SubWord() is a function that takes a four-byte input word and 
      // applies the S-box to each of the four bytes to produce an output word.

      // Function Subword()
      {
        tempa[0] = getSBoxEncVal(tempa[0],m1,m2);
        tempa[1] = getSBoxEncVal(tempa[1],m1,m2);
        tempa[2] = getSBoxEncVal(tempa[2],m1,m2);
        tempa[3] = getSBoxEncVal(tempa[3],m1,m2);
      }

      tempa[0] = XorEnc(tempa[0], encode(Rcon[i/Nk],m1,m2),m1,m2);
    }
#if defined(AES256) && (AES256 == 1)
    if (i % Nk == 4)
    {
      // Function Subword()
      {
        tempa[0] = getSBoxEncVal(tempa[0],m1,m2);
        tempa[1] = getSBoxEncVal(tempa[1],m1,m2);
        tempa[2] = getSBoxEncVal(tempa[2],m1,m2);
        tempa[3] = getSBoxEncVal(tempa[3],m1,m2);
      }
    }
#endif
    j = i * 4; k=(i - Nk) * 4;
    RoundKey[j + 0] = XorEnc(RoundKey[k + 0], tempa[0],m1,m2);
    //printf("Round Key is : %x | %x\n", RoundKey->u1, RoundKey->u2);
    RoundKey[j + 1] = XorEnc(RoundKey[k + 1], tempa[1],m1,m2);
    //printf("Round Key is : %x | %x\n", RoundKey->u1, RoundKey->u2);
    RoundKey[j + 2] = XorEnc(RoundKey[k + 2], tempa[2],m1,m2);
    //printf("Round Key is : %x | %x\n", RoundKey->u1, RoundKey->u2);
    RoundKey[j + 3] = XorEnc(RoundKey[k + 3], tempa[3],m1,m2);
    //printf("Round Key is : %x | %x\n", RoundKey->u1, RoundKey->u2);
  }
}

void AES_init_ctx(struct AES_ctx* ctx, const Encoded* key)
{
  for(unsigned l = 0; l < 256; l++)
  {
    sboxEnc[l] = encode(sbox[l],ctx->m1,ctx->m2);
  }

#if USE_SBOX == SBOX_TREE
  sboxEncTree = rncTreeFromArr(sboxEnc,0,256,ctx->m1,ctx->m2);
#elif USE_SBOX == SBOX_GRID
  sboxEncGrid = (RNC_Grid*)malloc(sizeof(RNC_Grid));
  initRNC_Grid(sboxEncGrid);
  loadRNC_Grid(sboxEncGrid,ctx->m1,ctx->m2,sboxEnc,256);
#endif

  KeyExpansion(ctx->RoundKey, key,ctx->m1,ctx->m2);

#if USE_SBOX == SBOX_TREE
  rncTreeDel(&sboxEncTree);
#elif USE_SBOX == SBOX_GRID
  freeRNC_Grid(sboxEncGrid);
  free(sboxEncGrid);
  sboxEncGrid = NULL;
#endif
}


int main(void)
{
    struct AES_ctx ctx;
    uint8_t i;
    // 128bit key
    uint8_t key[16] =        { (uint8_t) 0x2b, (uint8_t) 0x7e, (uint8_t) 0x15, (uint8_t) 0x16, (uint8_t) 0x28, (uint8_t) 0xae, (uint8_t) 0xd2, (uint8_t) 0xa6, (uint8_t) 0xab, (uint8_t) 0xf7, (uint8_t) 0x15, (uint8_t) 0x88, (uint8_t) 0x09, (uint8_t) 0xcf, (uint8_t) 0x4f, (uint8_t) 0x3c };
    // 512bit text
    uint8_t plain_text[64] = { (uint8_t) 0x6b, (uint8_t) 0xc1, (uint8_t) 0xbe, (uint8_t) 0xe2, (uint8_t) 0x2e, (uint8_t) 0x40, (uint8_t) 0x9f, (uint8_t) 0x96, (uint8_t) 0xe9, (uint8_t) 0x3d, (uint8_t) 0x7e, (uint8_t) 0x11, (uint8_t) 0x73, (uint8_t) 0x93, (uint8_t) 0x17, (uint8_t) 0x2a,
                               (uint8_t) 0xae, (uint8_t) 0x2d, (uint8_t) 0x8a, (uint8_t) 0x57, (uint8_t) 0x1e, (uint8_t) 0x03, (uint8_t) 0xac, (uint8_t) 0x9c, (uint8_t) 0x9e, (uint8_t) 0xb7, (uint8_t) 0x6f, (uint8_t) 0xac, (uint8_t) 0x45, (uint8_t) 0xaf, (uint8_t) 0x8e, (uint8_t) 0x51,
                               (uint8_t) 0x30, (uint8_t) 0xc8, (uint8_t) 0x1c, (uint8_t) 0x46, (uint8_t) 0xa3, (uint8_t) 0x5c, (uint8_t) 0xe4, (uint8_t) 0x11, (uint8_t) 0xe5, (uint8_t) 0xfb, (uint8_t) 0xc1, (uint8_t) 0x19, (uint8_t) 0x1a, (uint8_t) 0x0a, (uint8_t) 0x52, (uint8_t) 0xef,
                               (uint8_t) 0xf6, (uint8_t) 0x9f, (uint8_t) 0x24, (uint8_t) 0x45, (uint8_t) 0xdf, (uint8_t) 0x4f, (uint8_t) 0x9b, (uint8_t) 0x17, (uint8_t) 0xad, (uint8_t) 0x2b, (uint8_t) 0x41, (uint8_t) 0x7b, (uint8_t) 0xe6, (uint8_t) 0x6c, (uint8_t) 0x37, (uint8_t) 0x10 };
    

    // print text to encrypt, key and IV
    printf("ECB encrypt verbose:\n\n");
    printf("plain text:\n");
    for (i = (uint8_t) 0; i < (uint8_t) 4; ++i)
    {
        // phex(plain_text + i * (uint8_t) 16);// Phex loop iterator changes with AES_KEYLEN, not local length
        for(unsigned j = 0; j < 16; j++)
        {
            printf("%.2x ", plain_text[i*16 +j]);
            if((j+1) % 16 == 0)
            {
                printf("\n");
            }
        }
    }
    printf("\n");

    printf("key (Plain):\n");
    // phex(key); // Phex loop iterator changes with AES_KEYLEN, not local length
    for(i = 0; i < 16; i++)
    {
        printf("%.2x ", key[i]);
        if((i+1) % 16 == 0)
        {
            printf("\n");
        }
    }
    printf("\n");


    Encoded keyEnc[16];
    ctx.m1 = 25;
    ctx.m2 = 26;
    for(i = 0; i < 16; i++)
    {
        keyEnc[i] = encode(key[i],ctx.m1,ctx.m2);
        //printf("After encode = %x | %x | %x ",keyEnc[i].u1, keyEnc[i].u2, keyEnc[i]);
    }
    printf("key (Encoded w/ RNC):\n");
    for(i = 0; i < 16; i++)
    {
        printf("%.2x,%.2x ", keyEnc[i].u1,keyEnc[i].u2);
        if((i+1) % 8 == 0)
        {
            printf("\n");
        }
    }
    printf("\n");
    printf("\n");

    // print the resulting cipher as 4 x 16 byte strings
    printf("ciphertext:\n");
    printf("Nai kono Cipher Nai -____- \n");
    // for (int j = 0; j <200 ; j++) {
    //   printf("The value of ctx RK before is %d = %.2x \n", j, ctx.RoundKey[j]);
    // }
    AES_init_ctx(&ctx, keyEnc);

    printf("Key Expansion done \n");
    for (int j = 0; j <32 ; j++) {
      printf("The value of ctx RK is %d = %.2x \n", j, ctx.RoundKey[j]);
    }
}