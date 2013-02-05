#include <rho/ip/_pre.h>   // for ntohs, ntohl, htons, htonl

#include <rho/iPackable.h>
#include <rho/eRho.h>

#include <cmath>
#include <cstdlib>
#include <iostream>

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
    i32 exp = 0;
    u32 fracI = 0;

    if (std::isnan(x))
    {
        // x is not-a-number (nan).
        exp = 0x7FFFFFFF;
    }
    else if (! std::isfinite(x))
    {
        // x must be infinity or -infinity.
        if (x == INFINITY)
            exp = 0x7FFFFFFE;
        else if (x == -INFINITY)
            exp = 0x7FFFFFFD;
        else
            throw eImpossiblePath();
    }
    else if (x == 0.0f)
    {
        // x is zero.
        exp = 0x7FFFFFFC;
    }
    else
    {
        // x is finite and non-zero.
        f32 fracF = frexpf(x, &exp);   // fracF is in [0.5, 1)
                                       // x = fracF * 2^exp

        bool isneg = (fracF < 0.0f);

        fracF = fabsf(fracF);

        if (fracF >= 0.5f && fracF < 1.0f)
        {
            fracF -= 0.5f;             // fracF is in [0.0, 0.5)
            fracF *= 2.0f;             // fracF is in [0.0, 1.0)
            fracF *= (f32)0x7FFFFFFF;       // fracF is in [0, 0x7FFFFFFF)
            fracI = (u32)fracF;
        }
        else
        {
            throw eImpossiblePath();
        }

        if (isneg) fracI |= 0x80000000;
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

    if (exp == 0x7FFFFFFF)       // nan indicator
    {
        x = NAN;
    }
    else if (exp == 0x7FFFFFFE)  // infinity indicator
    {
        x = INFINITY;
    }
    else if (exp == 0x7FFFFFFD)  // -infinity indicator
    {
        x = -INFINITY;
    }
    else if (exp == 0x7FFFFFFC)  // zero indicator
    {
        x = 0.0f;
    }
    else
    {
        bool isneg = ((fracI & 0x80000000) != 0);
        if (isneg) fracI ^= 0x80000000;

        f32 fracF = (f32)fracI;        // fracF is in [0, 0x7FFFFFFF)
        fracF /= (f32)0x7FFFFFFF;           // fracF is in [0.0, 1.0)
        fracF /= 2.0f;                 // fracF is in [0.0, 0.5)
        fracF += 0.5f;                 // fracF is in [0.5, 1.0)
        if (isneg) fracF = -fracF;

        x = ldexpf(fracF, exp);
    }
}

void pack(iWritable*  out, f64  x)
{
    i32 exp = 0;
    u64 fracI = 0;

    if (std::isnan(x))
    {
        // x is not-a-number (nan).
        exp = 0x7FFFFFFF;
    }
    else if (! std::isfinite(x))
    {
        // x must be infinity or -infinity.
        if (x == (f64)INFINITY)
            exp = 0x7FFFFFFE;
        else if (x == (f64)(-INFINITY))
            exp = 0x7FFFFFFD;
        else
            throw eImpossiblePath();
    }
    else if (x == 0.0)
    {
        // x is zero.
        exp = 0x7FFFFFFC;
    }
    else
    {
        // x is finite and non-zero.
        f64 fracF = frexp(x, &exp);   // fracF is in [0.5, 1)
                                      // x = fracF * 2^exp

        bool isneg = (fracF < 0.0);

        fracF = fabs(fracF);

        if (fracF >= 0.5 && fracF < 1.0)
        {
            fracF -= 0.5;                // fracF is in [0.0, 0.5)
            fracF *= 2.0;                // fracF is in [0.0, 1.0)
            fracF *= (f64)0x7FFFFFFFFFFFFFFF; // fracF is in [0, 0x7FFFFFFF)
            fracI = (u64)fracF;
        }
        else
        {
            throw eImpossiblePath();
        }

        if (isneg) fracI |= 0x8000000000000000;
    }

    pack(out, fracI);
    pack(out, exp);
}

void unpack(iReadable* in, f64& x)
{
    i32 exp = 0;
    u64 fracI = 0;

    unpack(in, fracI);
    unpack(in, exp);

    if (exp == 0x7FFFFFFF)       // nan indicator
    {
        x = NAN;
    }
    else if (exp == 0x7FFFFFFE)  // infinity indicator
    {
        x = INFINITY;
    }
    else if (exp == 0x7FFFFFFD)  // -infinity indicator
    {
        x = -INFINITY;
    }
    else if (exp == 0x7FFFFFFC)  // zero indicator
    {
        x = 0.0;
    }
    else
    {
        bool isneg = ((fracI & 0x8000000000000000) != 0);
        if (isneg) fracI ^= 0x8000000000000000;

        f64 fracF = (f64)fracI;        // fracF is in [0, 0x7FFFFFFF)
        fracF /= (f64)0x7FFFFFFFFFFFFFFF;   // fracF is in [0.0, 1.0)
        fracF /= 2.0;                 // fracF is in [0.0, 0.5)
        fracF += 0.5;                 // fracF is in [0.5, 1.0)
        if (isneg) fracF = -fracF;

        x = ldexp(fracF, exp);
    }
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
