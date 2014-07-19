#ifndef __rho_iPackable_h__
#define __rho_iPackable_h__


#include <rho/ppcheck.h>
#include <rho/iWritable.h>
#include <rho/iReadable.h>
#include <rho/eRho.h>
#include <rho/types.h>

#include <cmath>
#include <string>
#include <vector>


namespace rho
{


class iPackable
{
    public:

        virtual void pack(iWritable* out) const = 0;
        virtual void unpack(iReadable* in) = 0;

        virtual ~iPackable() { }
};


void pack(iWritable*  out, u8  x);
void unpack(iReadable* in, u8& x);

void pack(iWritable*  out, i8  x);
void unpack(iReadable* in, i8& x);

void pack(iWritable*  out, u16  x);
void unpack(iReadable* in, u16& x);

void pack(iWritable*  out, i16  x);
void unpack(iReadable* in, i16& x);

void pack(iWritable*  out, u32  x);
void unpack(iReadable* in, u32& x);

void pack(iWritable*  out, i32  x);
void unpack(iReadable* in, i32& x);

void pack(iWritable*  out, u64  x);
void unpack(iReadable* in, u64& x);

void pack(iWritable*  out, i64  x);
void unpack(iReadable* in, i64& x);

void pack(iWritable*  out, f32  x);
void unpack(iReadable* in, f32& x);

void pack(iWritable*  out, f64  x);
void unpack(iReadable* in, f64& x);

void pack(iWritable*  out, const std::string& str);
void unpack(iReadable* in, std::string& str, u64 maxlen = 0xFFFFFFFFFFFFFFFF);

void pack(iWritable*  out, const iPackable& packable);
void unpack(iReadable* in, iPackable& packable);

template <class T>
void pack(iWritable* out, const std::vector<T>& vtr)
{
    pack(out, (u64)vtr.size());
    for (size_t i = 0; i < vtr.size(); i++)
        pack(out, vtr[i]);
}

template <class T>
void unpack(iReadable* in, std::vector<T>& vtr, u64 maxlen = 0xFFFFFFFFFFFFFFFF)
{
    u64 size; unpack(in, size);
    if (size > maxlen)
        throw eBufferOverflow("Unpacking a vector: the max length was exceeded!");
    if ((sizeof(u64) > sizeof(size_t)) && (size > ((size_t)(-1))))
        throw eBufferOverflow("Unpacking a vector: size too big for machine.");
    vtr = std::vector<T>((size_t)size);
    for (u64 i = 0; i < size; i++)
        unpack(in, vtr[(size_t)i]);
}


}   // namespace rho


#endif   // __rho_iPackable_h__
