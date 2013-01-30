#include <rho/ip/_pre.h>   // for ntohs, ntohl, htons, htonl

#include <rho/iPackable.h>
#include <rho/eRho.h>

#include <cmath>
#include <cstdlib>

namespace rho
{


void pack(iWritable* out, u8  x)
{
    if (out->writeAll(&x, 1) != 1)
        throw eBufferOverflow("Cannot pack to the given output stream.");
}

void unpack(iReadable* in, u8& x)
{
    if (in->readAll(&x, 1) != 1)
        throw eBufferUnderflow("Cannot unpack from the given input stream.");
}

void pack(iWritable* out, i8  x)
{
    pack(out, *((u8*)(&x)));
}

void unpack(iReadable* in, i8& x)
{
    unpack(in, *((u8*)(&x)));
}

void pack(iWritable* out, u16  x)
{
    x = htons(x);
    u8* p = (u8*)(&x);
    pack(out, p[0]);
    pack(out, p[1]);
}

void unpack(iReadable* in, u16& x)
{
    u8* p = (u8*)(&x);
    unpack(in, p[0]);
    unpack(in, p[1]);
    x = ntohs(x);
}

void pack(iWritable* out, i16  x)
{
    pack(out, *((u16*)(&x)));
}

void unpack(iReadable* in, i16& x)
{
    unpack(in, *((u16*)(&x)));
}

void pack(iWritable* out, u32  x)
{
    x = htonl(x);
    u8* p = (u8*)(&x);
    pack(out, p[0]);
    pack(out, p[1]);
    pack(out, p[2]);
    pack(out, p[3]);
}

void unpack(iReadable* in, u32& x)
{
    u8* p = (u8*)(&x);
    unpack(in, p[0]);
    unpack(in, p[1]);
    unpack(in, p[2]);
    unpack(in, p[3]);
    x = ntohl(x);
}

void pack(iWritable* out, i32  x)
{
    pack(out, *((u32*)(&x)));
}

void unpack(iReadable* in, i32& x)
{
    unpack(in, *((u32*)(&x)));
}

void pack(iWritable* out, u64  x)
{
    pack(out, (u8)((x >> 56) & 0xFF));
    pack(out, (u8)((x >> 48) & 0xFF));
    pack(out, (u8)((x >> 40) & 0xFF));
    pack(out, (u8)((x >> 32) & 0xFF));
    pack(out, (u8)((x >> 24) & 0xFF));
    pack(out, (u8)((x >> 16) & 0xFF));
    pack(out, (u8)((x >> 8 ) & 0xFF));
    pack(out, (u8)((x >> 0 ) & 0xFF));
}

void unpack(iReadable* in, u64& x)
{
    x = 0;
    u8 y;
    u64 z;
    unpack(in, y); z = (u64)y; x |= (z << 56);
    unpack(in, y); z = (u64)y; x |= (z << 48);
    unpack(in, y); z = (u64)y; x |= (z << 40);
    unpack(in, y); z = (u64)y; x |= (z << 32);
    unpack(in, y); z = (u64)y; x |= (z << 24);
    unpack(in, y); z = (u64)y; x |= (z << 16);
    unpack(in, y); z = (u64)y; x |= (z << 8 );
    unpack(in, y); z = (u64)y; x |= (z << 0 );
}

void pack(iWritable* out, i64  x)
{
    pack(out, *((u64*)(&x)));
}

void unpack(iReadable* in, i64& x)
{
    unpack(in, *((u64*)(&x)));
}

void pack(iWritable*  out, f32  x)
{
    const static u32 kMaxU32 = ~0;

    i32 exp = 0;
    u32 fracI = 0;

    f32 fracF = frexpf(x, &exp);   // fracF is 0.0 or in [0.5, 1)
                                   // x = fracF * 2^exp

    if (fracF > 0.0)
    {
        fracF -= 0.5;                    // fracF in [0.0, 0.5)
        fracF *= 2.0;                    // fracF in [0.0, 1.0)
        fracI = (u32)(fracF * kMaxU32);  // fracI in [0, kMaxU32)
    }

    pack(out, fracI);
    pack(out, exp);
}

void unpack(iReadable* in, f32& x)
{
    i32 exp = 0;
    u32 fracI = 0;

    unpack(in, fracI);
    unpack(in, exp);

    f32 fracF = (f32) fracI;

    x = ldexpf(fracF, exp);
}

void pack(iWritable*  out, f64  x)
{
    const static u64 kMaxU64 = ~0LL;

    i32 exp = 0;
    u64 fracI = 0LL;

    f64 fracF = frexp(x, &exp);   // fracF is 0.0 or in [0.5, 1)
                                  // x = fracF * 2^exp

    if (fracF > 0.0)
    {
        fracF -= 0.5;                    // fracF in [0.0, 0.5)
        fracF *= 2.0;                    // fracF in [0.0, 1.0)
        fracI = (u64)(fracF * kMaxU64);  // fracI in [0, kMaxU64)
    }

    pack(out, fracI);
    pack(out, exp);
}

void unpack(iReadable* in, f64& x)
{
    i32 exp = 0;
    u64 fracI = 0LL;

    unpack(in, fracI);
    unpack(in, exp);

    f64 fracF = (f64) fracI;

    x = ldexp(fracF, exp);
}

void pack(iWritable* out, const std::string& str)
{
    pack(out, (u32)str.length());
    for (size_t i = 0; i < str.length(); i++)
        pack(out, (u8)str[i]);
}

void unpack(iReadable* in, std::string& str)
{
    u32 length; unpack(in, length);
    str = "";
    for (u32 i = 0; i < length; i++)
    {
        u8 c; unpack(in, c);
        str += (char)c;
    }
}

void pack(iWritable* out, const iPackable& packable)
{
    packable.pack(out);
}

void unpack(iReadable* in, iPackable& packable)
{
    packable.unpack(in);
}


}   // namespace rho
