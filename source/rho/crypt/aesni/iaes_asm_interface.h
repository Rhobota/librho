/*
 * Copyright (c) 2010, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 *     * Neither the name of Intel Corporation nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

/*
 * Modified by Ryan Henning on April 6, 2015.
 */


#ifndef _INTEL_AES_ASM_INTERFACE_H__
#define _INTEL_AES_ASM_INTERFACE_H__


#include "iaesni.h"


typedef unsigned char UCHAR;


//structure to pass aes processing data to asm level functions
struct sAesData
{
    UCHAR   *in_block;
    UCHAR   *out_block;
    UCHAR   *expanded_key;
    UCHAR   *iv;                    // for CBC mode
    size_t  num_blocks;
};


// prepearing the different key rounds, for enc/dec in asm
// expnaded key should be 16-byte aligned
// expanded key should have enough space to hold all key rounds (16 bytes per round) - 256 bytes would cover all cases (AES256 has 14 rounds + 1 xor)
void iEncExpandKey256(UCHAR *key, UCHAR *expanded_key);
void iEncExpandKey192(UCHAR *key, UCHAR *expanded_key);
void iEncExpandKey128(UCHAR *key, UCHAR *expanded_key);

void iDecExpandKey256(UCHAR *key, UCHAR *expanded_key);
void iDecExpandKey192(UCHAR *key, UCHAR *expanded_key);
void iDecExpandKey128(UCHAR *key, UCHAR *expanded_key);


//enc/dec asm functions
void iEnc128(sAesData *data);
void iDec128(sAesData *data);
void iEnc256(sAesData *data);
void iDec256(sAesData *data);
void iEnc192(sAesData *data);
void iDec192(sAesData *data);

void iEnc128_CBC(sAesData *data);
void iDec128_CBC(sAesData *data);
void iEnc256_CBC(sAesData *data);
void iDec256_CBC(sAesData *data);
void iEnc192_CBC(sAesData *data);
void iDec192_CBC(sAesData *data);


#endif
