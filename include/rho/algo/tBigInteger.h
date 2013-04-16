#ifndef __rho_algo_tBigInteger_h__
#define __rho_algo_tBigInteger_h__


#include <rho/types.h>
#include <rho/eRho.h>

#include <iostream>
#include <string>
#include <vector>


namespace rho
{
namespace algo
{


class tBigInteger
{
    public:

        tBigInteger(i32 val);
        tBigInteger(std::string val, int radix=10);
        tBigInteger(std::vector<u8> bytes);     // <-- assumes positive

        std::string     toString(int radix=10) const;
        std::vector<u8> getBytes()             const;
        bool            isNegative()           const;
        bool            isOdd()                const;
        bool            isEven()               const;

        tBigInteger operator- () const;

        void        operator+= (const tBigInteger& o);
        tBigInteger operator+  (const tBigInteger& o) const;

        void        operator-= (const tBigInteger& o);
        tBigInteger operator-  (const tBigInteger& o) const;

        void        operator*= (const tBigInteger& o);
        tBigInteger operator*  (const tBigInteger& o) const;

    private:

        std::vector<u8> b;   // <-- stored little endian
                             // (aka, b[0] is the least significant byte)
        bool neg;
};


tBigInteger operator+ (i32 a, const tBigInteger& b);
tBigInteger operator- (i32 a, const tBigInteger& b);
tBigInteger operator* (i32 a, const tBigInteger& b);

std::istream& operator>> (std::istream& in, tBigInteger& b);
std::ostream& operator<< (std::ostream& out, const tBigInteger& b);


}   // namespace algo
}   // namespace rho


#endif    // __rho_algo_tBigInteger_h__
