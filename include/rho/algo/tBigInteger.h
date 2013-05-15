#ifndef __rho_algo_tBigInteger_h__
#define __rho_algo_tBigInteger_h__


#include <rho/algo/tArray.h>
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
        tBigInteger(std::vector<u8> bytes);            // <-- assumes positive

        std::string     toString(int radix=10) const;
        std::vector<u8> getBytes()             const;

        bool            isNegative()           const;
        tBigInteger&    setIsNegative(bool n);         // <-- returns *this

        bool            isZero()               const;
        bool            isOdd()                const;
        bool            isEven()               const;

        tBigInteger abs()        const;
        tBigInteger operator- () const;

        void        operator+= (const tBigInteger& o);
        tBigInteger operator+  (const tBigInteger& o) const;

        void        operator-= (const tBigInteger& o);
        tBigInteger operator-  (const tBigInteger& o) const;

        void        operator*= (const tBigInteger& o);
        tBigInteger operator*  (const tBigInteger& o) const;

        void        operator/= (const tBigInteger& o);
        tBigInteger operator/  (const tBigInteger& o) const;

        void        operator%= (const tBigInteger& o);
        tBigInteger operator%  (const tBigInteger& o) const;

        void div(const tBigInteger& o, tBigInteger& quotient, tBigInteger& remainder) const;

        tBigInteger modPow(const tBigInteger& e, const tBigInteger& m) const;
                                        // returns ((*this ^^ e) % m)

        bool        operator== (const tBigInteger& o) const;
        bool        operator!= (const tBigInteger& o) const;
        bool        operator<  (const tBigInteger& o) const;
        bool        operator>  (const tBigInteger& o) const;
        bool        operator<= (const tBigInteger& o) const;
        bool        operator>= (const tBigInteger& o) const;

    private:

        tArray m_array;   // <-- stored little endian
                          // (aka, m_array[0] is the least significant byte)
        bool m_neg;

    private:

        friend tBigInteger GCD(const tBigInteger& a, const tBigInteger& b);
};


tBigInteger operator+ (i32 a, const tBigInteger& b);
tBigInteger operator- (i32 a, const tBigInteger& b);
tBigInteger operator* (i32 a, const tBigInteger& b);

std::istream& operator>> (std::istream& in, tBigInteger& b);
std::ostream& operator<< (std::ostream& out, const tBigInteger& b);

/**
 * Returns the greatest common denominator of a and b.
 */
tBigInteger GCD(const tBigInteger& a, const tBigInteger& b);

/**
 * Calculates x and y such that: ax + by = gcd(a, b)
 */
void extendedGCD(const tBigInteger& a, const tBigInteger& b,
                 tBigInteger& x, tBigInteger& y);


}   // namespace algo
}   // namespace rho


#endif    // __rho_algo_tBigInteger_h__
