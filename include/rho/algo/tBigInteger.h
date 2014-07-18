#ifndef __rho_algo_tBigInteger_h__
#define __rho_algo_tBigInteger_h__


#include <rho/ppcheck.h>
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

    public:

        /**
         * Uses Miller-Rabin.
         *
         * If returns 'false', then n is definitely composite.
         *
         * If returns 'true', then n is prime with probability at least (1 - 2^(-numRounds)).
         *
         * Reference used: "Introduction to Algorithms (3rd edition)", pages 968-975
         */
        static bool isPrime(const tBigInteger& n, u32 numRounds);

        /**
         * Finds a random prime number that is at most 'numBits' long.
         * This method uses Miller-Rabin, so a prime is found with
         * probability at least (1 - 2^(-numRounds)).
         */
        static tBigInteger genPseudoPrime(u32 numBits, u32 numRounds);

        /**
         * Finds a random prime number that is at most 'numBits' long.
         * This uses the pseudo-prime method described in "Introduction
         * to Algorithms (3rd edition)" page 967. This method is not as good
         * as Miller-Rabin, but is faster and good enough for most applications.
         */
        static tBigInteger genPseudoPrime(u32 numBits);

    private:

        tArray<u32> m_array;   // <-- stored little endian
                               // (aka, m_array[0] is the least significant word)
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
