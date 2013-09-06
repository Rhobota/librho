#ifndef __rho_algo_tLCG_h__
#define __rho_algo_tLCG_h__


#include <rho/iReadable.h>


namespace rho
{
namespace algo
{


/**
 * The LCG used by glibc.
 */
class tLCG : public iReadable
{
    public:

        tLCG(u32 seed = 1);

        i32 read(u8* buffer, i32 length);
        i32 readAll(u8* buffer, i32 length);

        u32 next();
        u32 randMax();

    private:

        u32 m_x;
};


/**
 * The LCG recommended by Knuth.
 */
class tKnuthLCG : public iReadable
{
    public:

        tKnuthLCG(u64 seed = 1);

        i32 read(u8* buffer, i32 length);
        i32 readAll(u8* buffer, i32 length);

        u64 next();
        u64 randMax();

    private:

        u64 m_x;
        u64 m_curr;
        u8 m_currLeft;
};


/**
 * Some default LCG objects.
 */
extern tLCG gDefaultLCG;
extern tKnuthLCG gKnuthLCG;


}   // namespace algo
}   // namespace rho


#endif   // __rho_algo_tLCG_h__
