#ifndef __rho_iPackable_h__
#define __rho_iPackable_h__


#include <rho/iOutputStream.h>
#include <rho/iInputStream.h>
#include <rho/types.h>

#include <string>
#include <vector>


namespace rho
{


class iPackable
{
    public:

        virtual void pack(iOutputStream* out) const = 0;
        virtual void unpack(iInputStream* in) = 0;

        virtual ~iPackable() { }
};


void pack(iOutputStream* out, u8  x);
void unpack(iInputStream* in, u8& x);

void pack(iOutputStream* out, i8  x);
void unpack(iInputStream* in, i8& x);

void pack(iOutputStream* out, u16  x);
void unpack(iInputStream* in, u16& x);

void pack(iOutputStream* out, i16  x);
void unpack(iInputStream* in, i16& x);

void pack(iOutputStream* out, u32  x);
void unpack(iInputStream* in, u32& x);

void pack(iOutputStream* out, i32  x);
void unpack(iInputStream* in, i32& x);

void pack(iOutputStream* out, const std::string& str);
void unpack(iInputStream* in, std::string& str);

void pack(iOutputStream* out, const iPackable& packable);
void unpack(iInputStream* in, iPackable& packable);

template <class T>
void pack(iOutputStream* out, const std::vector<T>& vtr)
{
    pack(out, (u32)vtr.size());
    for (size_t i = 0; i < vtr.size(); i++)
        pack(out, vtr[i]);
}

template <class T>
void unpack(iInputStream* in, std::vector<T>& vtr)
{
    u32 size; unpack(in, size);
    vtr = std::vector<T>(size);
    for (u32 i = 0; i < size; i++)
        unpack(in, vtr[i]);
}


}   // namespace rho


#endif   // __rho_iPackable_h__
