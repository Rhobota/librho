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
#include <map>


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

template <> inline
void pack(iWritable* out, const std::vector<u8>& vtr)
{
    u64 size = (u64)vtr.size();
    u64 w = 0;
    pack(out, size);
    while (w < size)
    {
        i32 where = 0;
        if (size - w > 0x7FFFFFFF)
            where = 0x7FFFFFFF;
        else
            where = (i32)(size - w);
        if (out->writeAll(&vtr[(size_t)w], where) != where)
            throw eBufferOverflow("Cannot pack to the given output stream.");
        w += where;
    }
}

template <> inline
void unpack(iReadable* in, std::vector<u8>& vtr, u64 maxlen)
{
    u64 size; unpack(in, size);
    if (size > maxlen)
        throw eBufferOverflow("Unpacking a vector: the max length was exceeded!");
    if ((sizeof(u64) > sizeof(size_t)) && (size > ((size_t)(-1))))
        throw eBufferOverflow("Unpacking a vector: size too big for machine.");
    vtr.resize((size_t)size);
    u64 r = 0;
    while (r < size)
    {
        i32 rhere = 0;
        if (size - r > 0x7FFFFFFF)
            rhere = 0x7FFFFFFF;
        else
            rhere = (i32)(size - r);
        if (in->readAll(&vtr[(size_t)r], rhere) != rhere)
            throw eBufferUnderflow("Cannot unpack from the given input stream.");
        r += rhere;
    }
}

template <class T, class U>
void pack(iWritable* out, const std::map<T,U>& mp)
{
    pack(out, (u64)mp.size());
    typename std::map<T,U>::const_iterator itr;
    for (itr = mp.begin(); itr != mp.end(); itr++)
    {
        pack(out, itr->first);
        pack(out, itr->second);
    }
}

template <class T, class U>
void unpack(iReadable* in, std::map<T,U>& mp)
{
    u64 size; unpack(in, size);
    mp.clear();
    for (u64 i = 0; i < size; i++)
    {
        T t; unpack(in, t);
        U u; unpack(in, u);
        mp[t] = u;
    }
}


}   // namespace rho


#endif   // __rho_iPackable_h__
