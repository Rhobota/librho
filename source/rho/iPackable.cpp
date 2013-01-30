#include <rho/ip/_pre.h>   // for ntohs, ntohl, htons, htonl

#include <rho/iPackable.h>
#include <rho/eRho.h>

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
