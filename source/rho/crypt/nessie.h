/**
 * The Whirlpool hashing function.
 *
 * <P>
 * <b>References</b>
 *
 * <P>
 * The Whirlpool algorithm was developed by
 * <a href="mailto:pbarreto@scopus.com.br">Paulo S. L. M. Barreto</a> and
 * <a href="mailto:vincent.rijmen@cryptomathic.com">Vincent Rijmen</a>.
 *
 * See
 *      P.S.L.M. Barreto, V. Rijmen,
 *      ``The Whirlpool hashing function,''
 *      NESSIE submission, 2000 (tweaked version, 2001),
 *      <https://www.cosic.esat.kuleuven.ac.be/nessie/workshop/submissions/whirlpool.zip>
 *
 * @author  Paulo S.L.M. Barreto
 * @author  Vincent Rijmen.
 *
 * @version 3.0 (2003.03.12)
 */


/*
 * Modified by Ryan Henning on March 13, 2014.
 */


#ifndef __whirlpool_nessie_h__
#define __whirlpool_nessie_h__

#include <rho/types.h>

typedef rho::u8  u8;
typedef rho::u16 u16;
typedef rho::u32 u32;
typedef rho::u64 u64;

typedef rho::i8  s8;
typedef rho::i16 s16;
typedef rho::i32 s32;
typedef rho::i64 s64;

#define LL(v)   (v##ULL)

#define ONE8    0xffU
#define ONE16   0xffffU
#define ONE32   0xffffffffU
#define ONE64   LL(0xffffffffffffffff)

#define T8(x)   ((x) & ONE8)
#define T16(x)  ((x) & ONE16)
#define T32(x)  ((x) & ONE32)
#define T64(x)  ((x) & ONE64)

#define ROTR64(v, n)   (((v) >> (n)) | T64((v) << (64 - (n))))

/*
 * U8TO32_BIG(c) returns the 32-bit value stored in big-endian convention
 * in the unsigned char array pointed to by c.
 */
#define U8TO32_BIG(c)  (((u32)T8(*(c)) << 24) | ((u32)T8(*((c) + 1)) << 16) | ((u32)T8(*((c) + 2)) << 8) | ((u32)T8(*((c) + 3))))

/*
 * U8TO32_LITTLE(c) returns the 32-bit value stored in little-endian convention
 * in the unsigned char array pointed to by c.
 */
#define U8TO32_LITTLE(c)  (((u32)T8(*(c))) | ((u32)T8(*((c) + 1)) << 8) | ((u32)T8(*((c) + 2)) << 16) | ((u32)T8(*((c) + 3)) << 24))

/*
 * U8TO32_BIG(c, v) stores the 32-bit-value v in big-endian convention
 * into the unsigned char array pointed to by c.
 */
#define U32TO8_BIG(c, v)    do { u32 x = (v); u8 *d = (c); d[0] = T8(x >> 24); d[1] = T8(x >> 16); d[2] = T8(x >> 8); d[3] = T8(x); } while (0)

/*
 * U8TO32_LITTLE(c, v) stores the 32-bit-value v in little-endian convention
 * into the unsigned char array pointed to by c.
 */
#define U32TO8_LITTLE(c, v)    do { u32 x = (v); u8 *d = (c); d[0] = T8(x); d[1] = T8(x >> 8); d[2] = T8(x >> 16); d[3] = T8(x >> 24); } while (0)

/*
 * ROTL32(v, n) returns the value of the 32-bit unsigned value v after
 * a rotation of n bits to the left. It might be replaced by the appropriate
 * architecture-specific macro.
 *
 * It evaluates v and n twice.
 *
 * The compiler might emit a warning if n is the constant 0. The result
 * is undefined if n is greater than 31.
 */
#define ROTL32(v, n)   (T32((v) << (n)) | ((v) >> (32 - (n))))

/*
 * Whirlpool-specific definitions.
 */

#define DIGESTBYTES 64
#define DIGESTBITS  (8*DIGESTBYTES) /* 512 */

#define WBLOCKBYTES 64
#define WBLOCKBITS  (8*WBLOCKBYTES) /* 512 */

#define LENGTHBYTES 32
#define LENGTHBITS  (8*LENGTHBYTES) /* 256 */

typedef struct NESSIEstruct {
    u8  bitLength[LENGTHBYTES]; /* global number of hashed bits (256-bit counter) */
    u8  buffer[WBLOCKBYTES];    /* buffer of data to hash */
    s32 bufferBits;             /* current number of bits on the buffer */
    s32 bufferPos;              /* current (possibly incomplete) byte slot on the buffer */
    u64 hash[DIGESTBYTES/8];    /* the hashing state */
} NESSIEstruct;

#endif   /* __whirlpool_nessie_h__ */
