/* crypto/sha/sha.h */
/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 *
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 *
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 *
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */


/*
 * Modified by Ryan Henning on March 12, 2014.
 */


#ifndef HEADER_SHA_H
#define HEADER_SHA_H

#include <stddef.h>
#include <rho/types.h>

#define SHA_LONG rho::u32

#define SHA_LBLOCK  16
#define SHA_CBLOCK  (SHA_LBLOCK*4)  /* SHA treats input data as a
                                     * contiguous array of 32 bit
                                     * wide big-endian values. */
#define SHA_LAST_BLOCK  (SHA_CBLOCK-8)
#define SHA_DIGEST_LENGTH 20

typedef struct SHAstate_st
{
    SHA_LONG h0,h1,h2,h3,h4;
    SHA_LONG Nl,Nh;
    SHA_LONG data[SHA_LBLOCK];
    SHA_LONG num;
} SHA_CTX;

int SHA_Init(SHA_CTX *c);
int SHA_Update(SHA_CTX *c, const void *data, size_t len);
int SHA_Final(rho::u8 *md, SHA_CTX *c);
rho::u8 *SHA(const rho::u8 *d, size_t n, rho::u8 *md);
void SHA_Transform(SHA_CTX *c, const rho::u8 *data);

int SHA1_Init(SHA_CTX *c);
int SHA1_Update(SHA_CTX *c, const void *data, size_t len);
int SHA1_Final(rho::u8 *md, SHA_CTX *c);
rho::u8 *SHA1(const rho::u8 *d, size_t n, rho::u8 *md);
void SHA1_Transform(SHA_CTX *c, const rho::u8 *data);

#define SHA256_CBLOCK   (SHA_LBLOCK*4)  /* SHA-256 treats input data as a
                                         * contiguous array of 32 bit
                                         * wide big-endian values. */
#define SHA224_DIGEST_LENGTH    28
#define SHA256_DIGEST_LENGTH    32

typedef struct SHA256state_st
{
    SHA_LONG h[8];
    SHA_LONG Nl,Nh;
    SHA_LONG data[SHA_LBLOCK];
    SHA_LONG num,md_len;
} SHA256_CTX;

int SHA224_Init(SHA256_CTX *c);
int SHA224_Update(SHA256_CTX *c, const void *data, size_t len);
int SHA224_Final(rho::u8 *md, SHA256_CTX *c);
rho::u8 *SHA224(const rho::u8 *d, size_t n,rho::u8 *md);

int SHA256_Init(SHA256_CTX *c);
int SHA256_Update(SHA256_CTX *c, const void *data, size_t len);
int SHA256_Final(rho::u8 *md, SHA256_CTX *c);
rho::u8 *SHA256(const rho::u8 *d, size_t n,rho::u8 *md);
void SHA256_Transform(SHA256_CTX *c, const rho::u8 *data);


#define SHA384_DIGEST_LENGTH    48
#define SHA512_DIGEST_LENGTH    64

#define SHA512_CBLOCK   (SHA_LBLOCK*8)  /* SHA-512 treats input data as a
                                         * contiguous array of 64 bit
                                         * wide big-endian values. */
#define SHA_LONG64 rho::u64
#define U64(C)     C              // also try C##ULL

typedef struct SHA512state_st
{
    SHA_LONG64 h[8];
    SHA_LONG64 Nl,Nh;
    union {
        SHA_LONG64  d[SHA_LBLOCK];
        rho::u8   p[SHA512_CBLOCK];
    } u;
    SHA_LONG num,md_len;
} SHA512_CTX;

int SHA384_Init(SHA512_CTX *c);
int SHA384_Update(SHA512_CTX *c, const void *data, size_t len);
int SHA384_Final(rho::u8 *md, SHA512_CTX *c);
rho::u8 *SHA384(const rho::u8 *d, size_t n,rho::u8 *md);

int SHA512_Init(SHA512_CTX *c);
int SHA512_Update(SHA512_CTX *c, const void *data, size_t len);
int SHA512_Final(rho::u8 *md, SHA512_CTX *c);
rho::u8 *SHA512(const rho::u8 *d, size_t n,rho::u8 *md);
void SHA512_Transform(SHA512_CTX *c, const rho::u8 *data);


#endif
