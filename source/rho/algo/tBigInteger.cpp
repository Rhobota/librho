#pragma GCC optimize 3

#include <rho/algo/tBigInteger.h>
#include <rho/crypt/tSecureRandom.h>
#include <sstream>
using namespace std;

#ifndef KARATSUBA_CUTOFF
#define KARATSUBA_CUTOFF 200     // must be >0
#endif


namespace rho
{
namespace algo
{


///////////////////////////////////////////////////////////////////////////////
// Util function
///////////////////////////////////////////////////////////////////////////////

template <class T>
static
bool isLess(const tArray<T>& a, const tArray<T>& b)
{
    if (a.size() != b.size()) return a.size() < b.size();
    for (int i = (int)a.size()-1; i >= 0; i--)
        if (a[i] != b[i])
            return a[i] < b[i];
    return false;
}

template <class T>
static
bool isEqual(const tArray<T>& a, const tArray<T>& b)
{
    if (a.size() != b.size()) return false;
    for (int i = (int)a.size()-1; i >= 0; i--)
        if (a[i] != b[i])
            return false;
    return true;
}

template <class T>
static
void cleanup(tArray<T>& v)
{
    while (v.size() > 0 && v.back() == 0) v.pop_back();
}

static
void u8to32(const tArray<u8>& from, tArray<u32>& to)
{
    to.clear();
    for (size_t i = 0; i < from.size(); i += 4)
    {
        u32 val = 0;
        size_t remaining = from.size() - i;
        switch (remaining)
        {
            case 1:
                val = from[i];
                break;
            case 2:
                val = (from[i+1] << 8) | (from[i]);
                break;
            case 3:
                val = (from[i+2] << 16) | (from[i+1] << 8) | (from[i]);
                break;
            default:
                val = (from[i+3] << 24) | (from[i+2] << 16) | (from[i+1] << 8) | (from[i]);
                break;
        }
        to.push_back(val);
    }
}

static
void u32to8(const tArray<u32>& from, tArray<u8>& to)
{
    to.clear();
    for (size_t i = 0; i < from.size(); i++)
    {
        u32 val = from[i];
        to.push_back((u8)(val & 0xFF)); val >>= 8;
        to.push_back((u8)(val & 0xFF)); val >>= 8;
        to.push_back((u8)(val & 0xFF)); val >>= 8;
        to.push_back((u8)(val & 0xFF));
    }
    cleanup(to);
}

static
size_t numbits(const tArray<u32>& v)
{
    if (v.size() == 0)
        return 0;

    u32 b = v.back();
    size_t i = 0;
    while (b != 0) { b /= 2; i++; }

    return (v.size()-1)*32 + i;
}

static
void shiftRight(tArray<u32>& v, size_t n)
{
    size_t wordOff = n / 32;
    size_t bitOff = n % 32;

    u32 mask = 0;
    switch (bitOff)
    {
        case 0: break;
        case 1: mask = 0x1; break;
        case 2: mask = 0x3; break;
        case 3: mask = 0x7; break;
        case 4: mask = 0xF; break;
        case 5: mask = 0x1F; break;
        case 6: mask = 0x3F; break;
        case 7: mask = 0x7F; break;
        case 8: mask = 0xFF; break;
        case 9: mask = 0x1FF; break;
        case 10: mask = 0x3FF; break;
        case 11: mask = 0x7FF; break;
        case 12: mask = 0xFFF; break;
        case 13: mask = 0x1FFF; break;
        case 14: mask = 0x3FFF; break;
        case 15: mask = 0x7FFF; break;
        case 16: mask = 0xFFFF; break;
        case 17: mask = 0x1FFFF; break;
        case 18: mask = 0x3FFFF; break;
        case 19: mask = 0x7FFFF; break;
        case 20: mask = 0xFFFFF; break;
        case 21: mask = 0x1FFFFF; break;
        case 22: mask = 0x3FFFFF; break;
        case 23: mask = 0x7FFFFF; break;
        case 24: mask = 0xFFFFFF; break;
        case 25: mask = 0x1FFFFFF; break;
        case 26: mask = 0x3FFFFFF; break;
        case 27: mask = 0x7FFFFFF; break;
        case 28: mask = 0xFFFFFFF; break;
        case 29: mask = 0x1FFFFFFF; break;
        case 30: mask = 0x3FFFFFFF; break;
        case 31: mask = 0x7FFFFFFF; break;
        default: break;
    }

    for (size_t i = 0; i < v.size(); i++)
    {
        if (i + wordOff >= v.size())
        {
            v[i] = 0;
            continue;
        }

        v[i] = v[i+wordOff];

        if (i > 0)
        {
            u32 r = mask & v[i];
            r <<= 32-bitOff;
            v[i-1] |= r;
        }

        v[i] >>= bitOff;
    }

    cleanup(v);
}

static
void shiftLeft(tArray<u32>& v, size_t n)
{
    size_t wordOff = n / 32;
    size_t bitOff = n % 32;

    u32 mask = 0;
    switch (bitOff)
    {
        case 0: break;
        case 1: mask = 0x80000000; break;
        case 2: mask = 0xC0000000; break;
        case 3: mask = 0xE0000000; break;
        case 4: mask = 0xF0000000; break;
        case 5: mask = 0xF8000000; break;
        case 6: mask = 0xFC000000; break;
        case 7: mask = 0xFE000000; break;
        case 8: mask = 0xFF000000; break;
        case 9: mask = 0xFF800000; break;
        case 10: mask = 0xFFC00000; break;
        case 11: mask = 0xFFE00000; break;
        case 12: mask = 0xFFF00000; break;
        case 13: mask = 0xFFF80000; break;
        case 14: mask = 0xFFFC0000; break;
        case 15: mask = 0xFFFE0000; break;
        case 16: mask = 0xFFFF0000; break;
        case 17: mask = 0xFFFF8000; break;
        case 18: mask = 0xFFFFC000; break;
        case 19: mask = 0xFFFFE000; break;
        case 20: mask = 0xFFFFF000; break;
        case 21: mask = 0xFFFFF800; break;
        case 22: mask = 0xFFFFFC00; break;
        case 23: mask = 0xFFFFFE00; break;
        case 24: mask = 0xFFFFFF00; break;
        case 25: mask = 0xFFFFFF80; break;
        case 26: mask = 0xFFFFFFC0; break;
        case 27: mask = 0xFFFFFFE0; break;
        case 28: mask = 0xFFFFFFF0; break;
        case 29: mask = 0xFFFFFFF8; break;
        case 30: mask = 0xFFFFFFFC; break;
        case 31: mask = 0xFFFFFFFE; break;
        default: break;
    }

    for (size_t i = 0; i < wordOff+1; i++)
        v.push_back(0);

    for (int i = (int)v.size()-1; i >= 0; i--)
    {
        if ((size_t)i < wordOff)
        {
            v[i] = 0;
            continue;
        }

        v[i] = v[i-wordOff];

        if (i < (int)v.size()-1)
        {
            u32 r = mask & v[i];
            r >>= 32-bitOff;
            v[i+1] |= r;
        }

        v[i] <<= bitOff;
    }

    cleanup(v);
}

static
void keepOnly(tArray<u32>& v, size_t n)
{
    size_t wordOff = n / 32;
    size_t bitOff = n % 32;

    if (wordOff >= v.size())
        return;

    u32 mask = 0;
    switch (bitOff)
    {
        case 0: break;
        case 1: mask = 0x1; break;
        case 2: mask = 0x3; break;
        case 3: mask = 0x7; break;
        case 4: mask = 0xF; break;
        case 5: mask = 0x1F; break;
        case 6: mask = 0x3F; break;
        case 7: mask = 0x7F; break;
        case 8: mask = 0xFF; break;
        case 9: mask = 0x1FF; break;
        case 10: mask = 0x3FF; break;
        case 11: mask = 0x7FF; break;
        case 12: mask = 0xFFF; break;
        case 13: mask = 0x1FFF; break;
        case 14: mask = 0x3FFF; break;
        case 15: mask = 0x7FFF; break;
        case 16: mask = 0xFFFF; break;
        case 17: mask = 0x1FFFF; break;
        case 18: mask = 0x3FFFF; break;
        case 19: mask = 0x7FFFF; break;
        case 20: mask = 0xFFFFF; break;
        case 21: mask = 0x1FFFFF; break;
        case 22: mask = 0x3FFFFF; break;
        case 23: mask = 0x7FFFFF; break;
        case 24: mask = 0xFFFFFF; break;
        case 25: mask = 0x1FFFFFF; break;
        case 26: mask = 0x3FFFFFF; break;
        case 27: mask = 0x7FFFFFF; break;
        case 28: mask = 0xFFFFFFF; break;
        case 29: mask = 0x1FFFFFFF; break;
        case 30: mask = 0x3FFFFFFF; break;
        case 31: mask = 0x7FFFFFFF; break;
        default: break;
    }

    v[wordOff] = v[wordOff] & mask;

    for (size_t i = wordOff+1; i < v.size(); i++)
        v[i] = 0;

    cleanup(v);
}

static
void add(tArray<u32>& a, const tArray<u32>& b, size_t bShift = 0)  // bShift is in u32 chunks
{
    while (a.size() < b.size()+bShift)
        a.push_back(0);

    u32 carry = 0;
    size_t i;
    for (i = bShift; i < a.size() && (i-bShift) < b.size(); i++)
    {
        u64 sum = ((u64)a[i]) + b[(i-bShift)] + carry;
        a[i] = (u32)(sum & 0xFFFFFFFF);
        carry = (u32)((sum >> 32) & 0xFFFFFFFF);
    }

    for (; carry > 0; i++)
    {
        if (i == a.size())
        {
            a.push_back(carry);
            break;
        }
        else
        {
            u64 sum = ((u64)a[i]) + carry;
            a[i] = (u32)(sum & 0xFFFFFFFF);
            carry = (u32)((sum >> 32) & 0xFFFFFFFF);
        }
    }

    cleanup(a);
}

static
void subtract(tArray<u32>& a, const tArray<u32>& b)
{
    if (isLess(a, b))
        throw eLogicError("Do not call subtract with a<b.");

    for (size_t i = 0; i < a.size() && i < b.size(); i++)
    {
        u64 top = (u64) a[i];
        if (a[i] < b[i])     // <-- if need to borrow
        {
            size_t j = i+1;
            while (a[j] == 0) a[j++] = 0xFFFFFFFF;
            a[j]--;
            top = (u64)(top + 0x100000000);
        }
        a[i] = (u32)(top - b[i]);
    }

    cleanup(a);
}

static
void multiplyNaive(const tArray<u32>& a, const tArray<u32>& b,  // does "grade school multiplication"
                   tArray<u32>& result,
                   tArray<u32>& aux1, tArray<u32>& aux2)
{
    result.clear();

    if (a.size() == 0 || b.size() == 0)
        return;

    aux1.setSize(b.size()+1);

    for (size_t i = 0; i < a.size(); i++)
    {
        u32 carry = 0;
        for (size_t j = 0; j < b.size(); j++)
        {
            u64 mult = ((u64)a[i]) * b[j] + carry;
            aux1[j] = (u32) (mult & 0xFFFFFFFF);
            carry = (u32) ((mult >> 32) & 0xFFFFFFFF);
        }
        aux1[b.size()] = carry;
        add(result, aux1, i);
    }
}

static
void multiplyWord(tArray<u32>& a, u32 b)
{
    if (a.size() == 0)
        return;

    if (b == 0)
    {
        a.clear();
        return;
    }

    u32 carry = 0;
    for (size_t i = 0; i < a.size(); i++)
    {
        u64 mult = ((u64)a[i]) * b + carry;
        a[i] = (u32) (mult & 0xFFFFFFFF);
        carry = (u32) ((mult >> 32) & 0xFFFFFFFF);
    }
    if (carry > 0)
        a.push_back(carry);
}

static
void multiplyKaratsuba(const tArray<u32>& x, const tArray<u32>& y,
                       tArray<u32>& result, tArray<u32>& aux1, tArray<u32>& aux2)
{
    if (x.size() <= KARATSUBA_CUTOFF || y.size() <= KARATSUBA_CUTOFF)
    {
        multiplyNaive(x, y, result, aux1, aux2);
        return;
    }

    size_t n = std::max(numbits(x), numbits(y));
    n = (n / 2) + (n % 2);   // rounds up

    // x = 2^n b + a
    tArray<u32> a = x; keepOnly(a, n);
    tArray<u32> b = x; shiftRight(b, n);

    // y = 2^n d + c
    tArray<u32> c = y; keepOnly(c, n);
    tArray<u32> d = y; shiftRight(d, n);

    // Recurse!
    tArray<u32> ac;   multiplyKaratsuba(a, c, ac, aux1, aux2);
    tArray<u32> bd;   multiplyKaratsuba(b, d, bd, aux1, aux2);
    add(a, b);
    add(c, d);
    tArray<u32> abcd; multiplyKaratsuba(a, c, abcd, aux1, aux2);

    // K... For convenience
    tArray<u32> k = abcd;
    subtract(k, ac);
    subtract(k, bd);

    // Build results.
    result = bd;
    shiftLeft(result, n);
    add(result, k);
    shiftLeft(result, n);
    add(result, ac);
}

static
void multiply(const tArray<u32>& a, const tArray<u32>& b,
              tArray<u32>& result, tArray<u32>& aux1, tArray<u32>& aux2)
{
    multiplyKaratsuba(a, b, result, aux1, aux2);
}

static
void divideNaive(const tArray<u32>& a, const tArray<u32>& b,      // "long division"
            tArray<u32>& quotient, tArray<u32>& remainder,
            tArray<u32>& aux1, tArray<u32>& aux2)
{
    quotient.clear();
    remainder.clear();

    // If b is zero, that is bad.
    if (b.size() == 0)
        throw eInvalidArgument("You may not divide by zero!");

    // Long division:
    tArray<u32>& mult = aux1;
    tArray<u32>& sub = aux2;
    for (int i = (int)a.size()-1; i >= 0; i--)
    {
        if (remainder.size() > 0 || a[i] != 0)
        {
            remainder.push_back(0);
            for (size_t k = remainder.size()-1; k > 0; k--)
                remainder[k] = remainder[k-1];
            remainder[0] = a[i];
        }

        if (isLess(remainder, b))
        {
            quotient.push_back(0);
            continue;
        }

        u64 left = 0, right = 0xFFFFFFFF, mid;
        while (true)
        {
            mid = (left + right) / 2;
            mult = b;
            multiplyWord(mult, (u32)mid);
            if (isLess(remainder, mult))
                right = mid - 1;
            else
            {
                sub = remainder;
                subtract(sub, mult);
                if (isLess(b, sub) || b == sub)
                    left = mid + 1;
                else
                    break;
            }
        }

        quotient.push_back((u32)mid);
        subtract(remainder, mult);
    }

    quotient.reverse();
    cleanup(quotient);
}

static
void divide(const tArray<u32>& a, const tArray<u32>& b,
            tArray<u32>& quotient, tArray<u32>& remainder,
            tArray<u32>& aux1, tArray<u32>& aux2)
{
    divideNaive(a, b, quotient, remainder, aux1, aux2);
}

static
void gcd(tArray<u32> a, tArray<u32> b, tArray<u32>& result)
{
    tArray<u32> aQuo, aRem;
    tArray<u32> aux1, aux2;

    while (b.size() > 0)     // while (b != 0)
    {
        divide(a, b, aQuo, aRem, aux1, aux2);
        a = b;
        b = aRem;
    }

    result = a;
}

static
void modMultiplyNaive(const tArray<u32>& a, const tArray<u32>& b, const tArray<u32>& m,
                      tArray<u32>& result,
                      tArray<u32>& aux1, tArray<u32>& aux2, tArray<u32>& aux3, tArray<u32>& aux4)
{
    multiply(a, b, aux1, aux2, aux3);
    divide(aux1, m, aux2, result, aux3, aux4);
}

static
void modMultiplyBlakley(const tArray<u32>& a, const tArray<u32>& b, const tArray<u32>& m,
                      tArray<u32>& result,
                      tArray<u32>& aux1, tArray<u32>& aux2, tArray<u32>& aux3, tArray<u32>& aux4)
{
    aux1.clear();

    for (int i = (int)a.size()-1; i >= 0; i--)
    {
        u32 word = a[i];

        for (int j = 0; j < 32; j++)
        {
            multiplyWord(aux1, 2);

            if (word & 0x80000000)
                add(aux1, b);

            while (!isLess(aux1, m))
                subtract(aux1, m);

            word <<= 1;
        }
    }

    result = aux1;
}

static
void modMultiply(const tArray<u32>& a, const tArray<u32>& b, const tArray<u32>& m,
                 tArray<u32>& result,
                 tArray<u32>& aux1, tArray<u32>& aux2, tArray<u32>& aux3, tArray<u32>& aux4)
{
    if (false)
        modMultiplyBlakley(a, b, m, result, aux1, aux2, aux3, aux4);
    else
        modMultiplyNaive(a, b, m, result, aux1, aux2, aux3, aux4);
}

static
void modPowNaive(const tArray<u32>& a, const tArray<u32>& e, const tArray<u32>& m,
            tArray<u32>& result)
{
    result.clear();
    result.push_back(1);

    tArray<u32> aux1, aux2, aux3, aux4;
    tArray<u32> aa = a;

    for (size_t i = 0; i < e.size(); i++)
    {
        u32 word = e[i];

        for (int j = 0; j < 32; j++)
        {
            if (word & 1)
                modMultiply(result, aa, m, result, aux1, aux2, aux3, aux4);
            word >>= 1;
            modMultiply(aa, aa, m, aa, aux1, aux2, aux3, aux4);
        }
    }
}

static
void modPowMary(const tArray<u32>& a, const tArray<u32>& e, const tArray<u32>& m,
            tArray<u32>& result)   // "m-ary method" (here m=4)
{
    result.clear();
    result.push_back(1);

    tArray<u32> aux1, aux2, aux3, aux4;

    tArray<u32> tab[16];
    tab[0].push_back(1);
    for (int i = 1; i < 16; i++)
        modMultiply(tab[i-1], a, m, tab[i], aux1, aux2, aux3, aux4);

    for (int i = (int)e.size()-1; i >= 0; i--)
    {
        u32 word = e[i];

        for (int i = 0; i < 8; i++)
        {
            u32 high = (word & 0xF0000000) >> 28;
            word <<= 4;
            modMultiply(result, result, m, result, aux1, aux2, aux3, aux4);
            modMultiply(result, result, m, result, aux1, aux2, aux3, aux4);
            modMultiply(result, result, m, result, aux1, aux2, aux3, aux4);
            modMultiply(result, result, m, result, aux1, aux2, aux3, aux4);
            modMultiply(result, tab[high], m, result, aux1, aux2, aux3, aux4);
        }
    }
}

static
void modPowCLNW(const tArray<u32>& a, const tArray<u32>& e, const tArray<u32>& m,
            tArray<u32>& result)   // "Constant Length Nonzero Windows"
{
    static const u32 kWindowSize = 4;

    result.clear();
    result.push_back(1);

    tArray<u32> aux1, aux2, aux3, aux4;

    tArray<u32> tab[16];                  // <--- 2^kWindowSize
    tab[0].push_back(1);
    for (int i = 1; i < 16; i++)          // <--- 2^kWindowSize
        modMultiply(tab[i-1], a, m, tab[i], aux1, aux2, aux3, aux4);

    bool zw = true;  // in "zero window"?
    u32 d = 0;       // the number of bits collected from a nonzero-window
    u32 col = 0;     // the d collected bits

    for (int i = (int)e.size()-1; i >= 0; i--)
    {
        u32 word = e[i];

        for (int i = 0; i < 32; i++)
        {
            u32 bit = ((word & 0x80000000) >> 31);
            word <<= 1;

            modMultiply(result, result, m, result, aux1, aux2, aux3, aux4);

            if (bit == 0 && zw)
                continue;

            col <<= 1;
            col |= bit;
            ++d;
            zw = false;

            if (d == kWindowSize)
            {
                modMultiply(result, tab[col], m, result, aux1, aux2, aux3, aux4);
                d = 0;
                col = 0;
                zw = true;
            }
        }
    }

    if (!zw)
    {
        modMultiply(result, tab[col], m, result, aux1, aux2, aux3, aux4);
        d = 0;
        col = 0;
        zw = true;
    }
}

static
void montgomeryCalculateNprim(const tArray<u32>& n, tArray<u32>& nPrim, tArray<u32>& r)
{
    size_t k = numbits(n);
    r.clear(); r.push_back(1); shiftLeft(r, k);   // r = 2^k

    tArray<u8> n8; u32to8(n, n8);
    tArray<u8> r8; u32to8(r, r8);
    tBigInteger nBI(vector<u8>(&n8[0], &n8[n8.size()]));
    tBigInteger rBI(vector<u8>(&r8[0], &r8[r8.size()]));
    tBigInteger nPrimBI(0);
    tBigInteger rInvBI(0);
    extendedGCD(nBI, rBI, nPrimBI, rInvBI);
    while (rInvBI.isNegative())
    {
        rInvBI += nBI;
        nPrimBI -= rBI;
    }

    tArray<u8> nPrim8;
    vector<u8> bytes = nPrimBI.getBytes();
    for (size_t i = 0; i < bytes.size(); i++)
        nPrim8.push_back(bytes[i]);
    u8to32(nPrim8, nPrim);
}

static
void montgomeryProduct(const tArray<u32>& a_hat, const tArray<u32>& b_hat,
                       const tArray<u32>& n, const tArray<u32>& nPrim,
                       const tArray<u32>& r,
                       tArray<u32>& result,
                       tArray<u32>& aux1, tArray<u32>& aux2, tArray<u32>& aux3, tArray<u32>& aux4)
{
    tArray<u32>& t = aux1;
    multiply(a_hat, b_hat, t, aux3, aux4);

    tArray<u32>& m = aux2;
    multiply(t, nPrim, m, aux3, aux4);
    keepOnly(m, numbits(r)-1);

    tArray<u32>& u = result;
    multiply(m, n, u, aux3, aux4);
    add(u, t);
    shiftRight(u, numbits(r)-1);

    if (!isLess(u, n))    // if (u >= n)
        subtract(u, n);
}

static
void modPowMontgomery(const tArray<u32>& M, const tArray<u32>& e, const tArray<u32>& n,
            tArray<u32>& result)
{
    if (n.size() == 0 || (n[0] & 1) == 0)
        throw eInvalidArgument("The modulus may not be even in Montgomery modPow at this time.");

    tArray<u32> aux1, aux2, aux3, aux4;

    tArray<u32> nPrim;
    tArray<u32> r;
    montgomeryCalculateNprim(n, nPrim, r);

    tArray<u32> M_hat;
    modMultiply(M, r, n, M_hat, aux1, aux2, aux3, aux4);

    tArray<u32> x_hat;
    divide(r, n, aux1, x_hat, aux2, aux3);

    for (int i = (int)e.size()-1; i >= 0; i--)
    {
        u32 word = e[i];

        for (int i = 0; i < 32; i++)
        {
            bool bit = (word & 0x80000000) > 0;
            word <<= 1;

            montgomeryProduct(x_hat, x_hat, n, nPrim, r, x_hat,
                              aux1, aux2, aux3, aux4);

            if (bit)
            {
                montgomeryProduct(M_hat, x_hat, n, nPrim, r, x_hat,
                                  aux1, aux2, aux3, aux4);
            }
        }
    }

    tArray<u32> one; one.push_back(1);
    montgomeryProduct(x_hat, one, n, nPrim, r, result,
                      aux1, aux2, aux3, aux4);
}

static
void modPow(const tArray<u32>& a, const tArray<u32>& e, const tArray<u32>& m,
            tArray<u32>& result)
{
    if (e.size() < 5)
        modPowNaive(a, e, m, result);
    else if (e.size() < 10)
        modPowMary(a, e, m, result);
    else if (e.size() < 15)
        modPowCLNW(a, e, m, result);
    else if (m.size() > 0 && (m[0] & 1) == 1)
        modPowMontgomery(a, e, m, result);      // <-- m must be odd for this method
    else
        modPowCLNW(a, e, m, result);
}


///////////////////////////////////////////////////////////////////////////////
// Methods
///////////////////////////////////////////////////////////////////////////////

tBigInteger::tBigInteger(i32 val)
{
    m_neg = val < 0;
    u32 uval = m_neg ? (u32)(-val) : (u32)val;
    if (uval > 0)
        m_array.push_back(uval);
}

static
void throwInvalidValStringException(char c, int radix)
{
    ostringstream o;
    o << "Invalid 'val' string character: " << c
      << " for radix: " << radix;
    throw eInvalidArgument(o.str());
}

tBigInteger::tBigInteger(string val, int radix)
    : m_neg(false)
{
    vector<u32> ints;

    switch (radix)
    {
        case 2:
        case 8:
        case 10:
        {
            char upperBound = (char) ((radix-1) + '0');
            for (size_t i = 0; i < val.size(); i++)
            {
                if (i == 0 && val[i] == '-')
                {
                    m_neg = true;
                    continue;
                }
                if (val[i] < '0' || val[i] > upperBound)
                    throwInvalidValStringException(val[i], radix);
                ints.push_back(val[i] - '0');
            }
        }
        break;

        case 16:
        {
            for (size_t i = 0; i < val.size(); i++)
            {
                if (i == 0 && val[i] == '-')
                {
                    m_neg = true;
                    continue;
                }
                if ((val[i] < '0' || val[i] > '9') &&
                    (val[i] < 'a' || val[i] > 'f') &&
                    (val[i] < 'A' || val[i] > 'F'))
                    throwInvalidValStringException(val[i], radix);
                if (val[i] >= '0' && val[i] <= '9')
                    ints.push_back(val[i] - '0');
                else if (val[i] >= 'a' && val[i] <= 'f')
                    ints.push_back(val[i] - 'a');
                else if (val[i] >= 'A' && val[i] <= 'F')
                    ints.push_back(val[i] - 'A');
                else
                    throw eImpossiblePath();
            }
        }
        break;

        default:
            throw eInvalidArgument(string() + "Unsupported radix (must be 2, 8, 10, or 16)");
    }

    tArray<u32> mult(1, 1);
    tArray<u32> toadd;
    for (int i = (int)ints.size()-1; i >= 0; i--)
    {
        if (ints[i] != 0)
        {
            toadd = mult;
            multiplyWord(toadd, ints[i]);
            add(m_array, toadd);
        }
        multiplyWord(mult, (u32)radix);
    }
}

tBigInteger::tBigInteger(vector<u8> bytes)
    : m_neg(false)
{
    tArray<u8> array8;
    for (size_t i = 0; i < bytes.size(); i++)
        array8.push_back(bytes[i]);
    cleanup(array8);
    u8to32(array8, m_array);
}

static
char toChar(u32 b)
{
    if (b <= 9)
        return (char)('0' + b);
    if (b <= 15)
        return (char)('A' + b);
    return '?';
}

string tBigInteger::toString(int radix) const
{
    switch (radix)
    {
        case 2:
        case 8:
        case 10:
        case 16:
            break;
        default:
            throw eInvalidArgument(string() + "Unsupported radix (must be 2, 8, 10, or 16)");
    }

    string str;

    tArray<u32> n = m_array;
    tArray<u32> quo, rem;
    tArray<u32> radixVect(1, (u32)radix);
    tArray<u32> aux1, aux2;
    while (n.size() > 0)
    {
        divide(n, radixVect, quo, rem, aux1, aux2);
        n = quo;
        str += (rem.size() == 0) ? '0' : toChar(rem[0]);
    }

    if (str == "")    str = "0";
    if (isNegative()) str += "-";

    return string(str.rbegin(), str.rend());
}

vector<u8> tBigInteger::getBytes() const
{
    tArray<u8> array8; u32to8(m_array, array8);
    return vector<u8>(&array8[0], &array8[array8.size()]);
}

bool tBigInteger::isNegative() const
{
    return !isZero() && m_neg;
}

tBigInteger& tBigInteger::setIsNegative(bool n)
{
    m_neg = n;
    return *this;
}

bool tBigInteger::isZero() const
{
    return (m_array.size() == 0);   // (m_array.size() == 0)  <==>  (*this == 0)
}

bool tBigInteger::isOdd() const
{
    return !isZero() && ((m_array[0] & 1) > 0);
}

bool tBigInteger::isEven() const
{
    return !isOdd();
}

tBigInteger tBigInteger::abs() const
{
    if (isNegative())
        return -(*this);
    else
        return (*this);
}

tBigInteger tBigInteger::operator- () const
{
    tBigInteger o = *this;
    o.m_neg = !o.m_neg;
    return o;
}

void tBigInteger::operator+= (const tBigInteger& o)
{
    if (m_neg && !o.m_neg)
    {
        *this = o - (-(*this));
        return;
    }

    if (!m_neg && o.m_neg)
    {
        *this -= -o;
        return;
    }

    // At this point, *this and o have the same sign.

    add(m_array, o.m_array);
}

void tBigInteger::operator-= (const tBigInteger& o)
{
    if (m_neg && !o.m_neg)
    {
        *this += -o;
        return;
    }

    if (!m_neg && o.m_neg)
    {
        *this += -o;
        return;
    }

    if (m_neg && o.m_neg)
    {
        *this = -o - -(*this);
        return;
    }

    if (*this < o)
    {
        *this = -(o - *this);
        return;
    }

    // At this point, *this and o are both non-negative and (*this >= o).

    subtract(m_array, o.m_array);
}

void tBigInteger::operator*= (const tBigInteger& o)
{
    tArray<u32> result;
    tArray<u32> aux1, aux2;
    multiply(m_array, o.m_array, result, aux1, aux2);
    m_array = result;
    m_neg = (isNegative() && !o.isNegative()) || (!isNegative() && o.isNegative());
}

void tBigInteger::operator/= (const tBigInteger& o)
{
    tArray<u32> quotient;
    tArray<u32> remainder;
    tArray<u32> aux1, aux2;
    divide(m_array, o.m_array, quotient, remainder, aux1, aux2);
    m_array = quotient;
    m_neg = (isNegative() && !o.isNegative()) || (!isNegative() && o.isNegative());
}

void tBigInteger::operator%= (const tBigInteger& o)
{
    tArray<u32> quotient;
    tArray<u32> remainder;
    tArray<u32> aux1, aux2;
    divide(m_array, o.m_array, quotient, remainder, aux1, aux2);
    m_array = remainder;
}

void tBigInteger::div(const tBigInteger& o, tBigInteger& quotient, tBigInteger& remainder) const
{
    tArray<u32> quotientArray;
    tArray<u32> remainderArray;
    tArray<u32> aux1, aux2;
    divide(m_array, o.m_array, quotientArray, remainderArray, aux1, aux2);
    quotient.m_neg = (isNegative() && !o.isNegative()) || (!isNegative() && o.isNegative());
    quotient.m_array = quotientArray;
    remainder.m_neg = this->m_neg;
    remainder.m_array = remainderArray;
}

tBigInteger tBigInteger::modPow(const tBigInteger& e, const tBigInteger& m) const
{
    if (isNegative())
        throw eInvalidArgument("Cannot handle modPow() on a negative integer at this time.");
    tArray<u32> result;
    rho::algo::modPow(m_array, e.m_array, m.m_array, result);
    tBigInteger bi(0);
    bi.m_array = result;
    return bi;
}

tBigInteger tBigInteger::operator+  (const tBigInteger& o) const
{
    tBigInteger n = *this;
    n += o;
    return n;
}

tBigInteger tBigInteger::operator-  (const tBigInteger& o) const
{
    tBigInteger n = *this;
    n -= o;
    return n;
}

tBigInteger tBigInteger::operator*  (const tBigInteger& o) const
{
    tBigInteger n = *this;
    n *= o;
    return n;
}

tBigInteger tBigInteger::operator/  (const tBigInteger& o) const
{
    tBigInteger n = *this;
    n /= o;
    return n;
}

tBigInteger tBigInteger::operator%  (const tBigInteger& o) const
{
    tBigInteger n = *this;
    n %= o;
    return n;
}

bool tBigInteger::operator== (const tBigInteger& o) const
{
    return (m_array == o.m_array) && (isNegative() == o.isNegative());
}

bool tBigInteger::operator!= (const tBigInteger& o) const
{
    return !(*this == o);
}

bool tBigInteger::operator<  (const tBigInteger& o) const
{
    if (isNegative() && !o.isNegative())
        return true;
    if (!isNegative() && o.isNegative())
        return false;

    // The sign of the two vals must be the same.

    if (m_array.size() != o.m_array.size())
    {
        bool less = m_array.size() < o.m_array.size();
        return isNegative() ? !less : less;
    }
    else
    {
        for (int i = (int)m_array.size()-1; i >=0; i--)
            if (m_array[i] != o.m_array[i])
                return isNegative() ? o.m_array[i] < m_array[i] : m_array[i] < o.m_array[i];

        return false;  // they must be equal
    }
}

bool tBigInteger::operator>  (const tBigInteger& o) const
{
    return (o < *this);
}

bool tBigInteger::operator<= (const tBigInteger& o) const
{
    return (*this < o) || (*this == o);
}

bool tBigInteger::operator>= (const tBigInteger& o) const
{
    return (*this > o) || (*this == o);
}

tBigInteger operator+ (i32 a, const tBigInteger& b)
{
    return b + a;
}

tBigInteger operator- (i32 a, const tBigInteger& b)
{
    return -b + a;
}

tBigInteger operator* (i32 a, const tBigInteger& b)
{
    return b * a;
}

istream& operator>> (istream& in, tBigInteger& b)
{
    string str;
    in >> str;
    b = tBigInteger(str);
    return in;
}

ostream& operator<< (ostream& out, const tBigInteger& b)
{
    out << b.toString();
    return out;
}

tBigInteger GCD(const tBigInteger& a, const tBigInteger& b)
{
    if (a.isNegative() || b.isNegative())
    {
        throw eInvalidArgument("At this point, neither a nor b may be negative in a call "
                "to GCD()");
    }

    tBigInteger result(0);
    gcd(a.m_array, b.m_array, result.m_array);
    return result;
}

void extendedGCD(const tBigInteger& a, const tBigInteger& b, tBigInteger& x, tBigInteger& y)
{
    if (a.isNegative() || b.isNegative())
    {
        throw eInvalidArgument("At this point, neither a nor b may be negative in a call "
                "to extendedGCD()");
    }

    x = tBigInteger(0);
    tBigInteger lastX(1);

    y = tBigInteger(1);
    tBigInteger lastY(0);

    tBigInteger aa = a;
    tBigInteger bb = b;

    tBigInteger aQuo(0);
    tBigInteger aRem(0);
    tBigInteger temp(0);

    while (!bb.isZero())
    {
        aa.div(bb, aQuo, aRem);
        aa = bb;
        bb = aRem;

        temp = x;
        x = lastX - aQuo * x;
        lastX = temp;

        temp = y;
        y = lastY - aQuo * y;
        lastY = temp;
    }

    x = lastX;
    y = lastY;
}

static
void s_genRandNumWithBits(u32 numBits, tArray<u32>& array)
{
    u32 numWords = numBits / 32;
    if ((numBits % 32) > 0)
        numWords++;

    array.setSize(numWords);
    if (numWords == 0)
        return;

    crypt::secureRand_readAll((u8*)(&array[0]), (i32)(numWords*4));

    keepOnly(array, (size_t)numBits);
}

static
void s_genRandNumLessThan(const tArray<u32>& maxValue, tArray<u32>& array)
{
    u32 numBits = (u32) numbits(maxValue);

    while (true)
    {
        s_genRandNumWithBits(numBits, array);
        if (isLess(array, maxValue))
            break;
    }
}

static
bool s_miller_rabin_witness(const tArray<u32>& a, const tArray<u32>& n)
{
    static const tArray<u32> one(1, 1);

    tArray<u32> nmo = n; subtract(nmo, one);
    tArray<u32> u = nmo;

    u32 t = 0;     // <-- num zeros on end of u
    u32 mask = 1;
    u32 index = 0;
    while ((u[index] & mask) == 0)
    {
        t++;
        mask <<= 1;
        if (mask == 0)
        {
            mask = 1;
            index++;
        }
    }

    shiftRight(u, t);

    tArray<u32> xCurr, xPrev;
    tArray<u32> aux1, aux2, aux3, aux4;

    modPow(a, u, n, xCurr);

    for (u32 i = 0; i < t; i++)
    {
        xPrev = xCurr;
        multiply(xPrev, xPrev, aux1, aux2, aux3);
        divide(aux1, n, aux2, xCurr, aux3, aux4);

        if (isEqual(xCurr, one) && !isEqual(xPrev, one) && !isEqual(xPrev, nmo))
            return true;
    }

    if (!isEqual(xCurr, one))
        return true;

    return false;
}

bool tBigInteger::isPrime(const tBigInteger& n, u32 numRounds)
{
    if (numRounds == 0)
        throw eInvalidArgument("numRounds must be > 0");

    if (n.isNegative())
        return false;
    if (n.isZero())
        return false;
    if (n.isEven())
        return false;
    if (n == 1)
        return false;

    tArray<u32> a;
    for (; numRounds > 0; numRounds--)
    {
        do { s_genRandNumLessThan(n.m_array, a); } while (a.size() == 0);
        if (s_miller_rabin_witness(a, n.m_array))
            return false;
    }
    return true;
}

tBigInteger tBigInteger::genPseudoPrime(u32 numBits, u32 numRounds)
{
    if (numBits < 2)
        throw eInvalidArgument("numBits must be >= 2");

    tBigInteger n(0);
    while (true)
    {
        s_genRandNumWithBits(numBits, n.m_array);
        if (n.isEven())
            n += 1;
        if (isPrime(n, numRounds))
            return n;
    }
}

tBigInteger tBigInteger::genPseudoPrime(u32 numBits)
{
    if (numBits < 2)
        throw eInvalidArgument("numBits must be >= 2");

    static const tBigInteger one(1);
    static const tBigInteger two(2);

    tBigInteger n(0);
    while (true)
    {
        s_genRandNumWithBits(numBits, n.m_array);
        if (n.isZero())
            continue;
        if (n == one)
            continue;
        if (n.isEven())
            n += 1;
        if (two.modPow(n-1,n) == one)
            return n;
    }
}


}    // namespace algo
}    // namespace rho
