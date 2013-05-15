#include <rho/algo/tBigInteger.h>
#include <sstream>
using namespace std;


#ifndef KARATSUBA_CUTOFF
#define KARATSUBA_CUTOFF 50      // must be >0
#endif

#ifndef USE_OPENSSL
#define USE_OPENSSL 0            // 0 for off, 1 for on
#endif

#ifndef OPENSSL_CUTOFF
#define OPENSSL_CUTOFF 50        // only applicable when USE_OPENSSL == 1
#endif


#if USE_OPENSSL
#include <openssl/bn.h>
#endif


namespace rho
{
namespace algo
{


///////////////////////////////////////////////////////////////////////////////
// Util function
///////////////////////////////////////////////////////////////////////////////

// static
// void print(const tArray& v)
// {
//     if (v.size() == 0)
//         cout << " 0";
//     for (int i = (int)v.size()-1; i >= 0; i--)
//         cout << " " << (u32)(v[i]);
//     cout << endl;
// }

static
void throwInvalidValStringException(char c, int radix)
{
    ostringstream o;
    o << "Invalid 'val' string character: " << c
      << " for radix: " + radix;
    throw eInvalidArgument(o.str());
}

static
char toChar(u8 b)
{
    if (b <= 9)
        return (char)('0' + b);
    if (b <= 15)
        return (char)('A' + b);
    return '?';
}

static
bool isLess(const tArray& a, const tArray& b)
{
    if (a.size() != b.size()) return a.size() < b.size();
    for (int i = (int)a.size()-1; i >= 0; i--)
        if (a[i] != b[i])
            return a[i] < b[i];
    return false;
}

static
void cleanup(tArray& v)
{
    while (v.size() > 0 && v.back() == 0) v.pop_back();
}

static
size_t numbits(const tArray& v)
{
    if (v.size() == 0)
        return 0;

    u8 b = v.back();
    size_t i = 0;
    while (b != 0) { b /= 2; i++; }

    return (v.size()-1)*8 + i;
}

static
void shiftRight(tArray& v, size_t n)
{
    size_t byteOff = n / 8;
    size_t bitOff = n % 8;

    u8 mask = 0;
    switch (bitOff)
    {
        case 0: break;
        case 1: mask = 1;   break;
        case 2: mask = 3;   break;
        case 3: mask = 7;   break;
        case 4: mask = 15;  break;
        case 5: mask = 31;  break;
        case 6: mask = 63;  break;
        case 7: mask = 127; break;
        default: throw eImpossiblePath();
    }

    for (size_t i = 0; i < v.size(); i++)
    {
        if (i + byteOff >= v.size())
        {
            v[i] = 0;
            continue;
        }

        v[i] = v[i+byteOff];

        if (i > 0)
        {
            u8 r = mask & v[i];
            r = (u8)(r << (8 - bitOff));
            v[i-1] |= r;
        }

        v[i] = (u8)(v[i] >> bitOff);
    }

    cleanup(v);
}

static
void shiftLeft(tArray& v, size_t n)
{
    size_t byteOff = n / 8;
    size_t bitOff = n % 8;

    u8 mask = 0;
    switch (bitOff)
    {
        case 0: break;
        case 1: mask = 128;   break;
        case 2: mask = 192;   break;
        case 3: mask = 224;   break;
        case 4: mask = 240;  break;
        case 5: mask = 248;  break;
        case 6: mask = 252;  break;
        case 7: mask = 254; break;
        default: throw eImpossiblePath();
    }

    for (size_t i = 0; i < byteOff+3; i++)
        v.push_back(0);

    for (int i = (int)v.size()-1; i >= 0; i--)
    {
        if ((size_t)i < byteOff)
        {
            v[i] = 0;
            continue;
        }

        v[i] = v[i-byteOff];

        if (i < (int)v.size()-1)
        {
            u8 r = mask & v[i];
            r = (u8)(r >> (8 - bitOff));
            v[i+1] |= r;
        }

        v[i] = (u8)(v[i] << bitOff);
    }

    cleanup(v);
}

static
void keepOnly(tArray& v, size_t n)
{
    size_t byteOff = n / 8;
    size_t bitOff = n % 8;

    if (byteOff >= v.size())
        return;

    u8 mask = 0;
    switch (bitOff)
    {
        case 0: break;
        case 1: mask = 1;   break;
        case 2: mask = 3;   break;
        case 3: mask = 7;   break;
        case 4: mask = 15;  break;
        case 5: mask = 31;  break;
        case 6: mask = 63;  break;
        case 7: mask = 127; break;
        default: throw eImpossiblePath();
    }

    v[byteOff] = v[byteOff] & mask;

    for (size_t i = byteOff+1; i < v.size(); i++)
        v[i] = 0;

    cleanup(v);
}

static
void add(tArray& a, const tArray& b, size_t bShift = 0)
{
    while (a.size() < b.size()+bShift)
        a.push_back(0);

    u8 carry = 0;
    size_t i;
    for (i = bShift; i < a.size() && (i-bShift) < b.size(); i++)
    {
        u16 sum = (u16) (a[i] + b[(i-bShift)] + carry);
        a[i] = (u8)(sum & 0xFF);
        carry = (u8)((sum >> 8) & 0xFF);
    }

    for (; carry > 0; i++)
    {
        if (i == a.size())
        {
            a.push_back(carry);
            carry = 0;
        }
        else
        {
            u16 sum = (u16) (a[i] + carry);
            a[i] = (u8)(sum & 0xFF);
            carry = (u8)((sum >> 8) & 0xFF);
        }
    }
}

static
void addByte(tArray& a, u8 b, size_t bShift = 0)
{
    if (b == 0)
        return;

    while (a.size() < 1+bShift)
        a.push_back(0);

    u8 carry = 0;
    size_t i;
    for (i = bShift; i < a.size() && (i-bShift) < 1; i++)
    {
        u16 sum = (u16) (a[i] + b + carry);
        a[i] = (u8)(sum & 0xFF);
        carry = (u8)((sum >> 8) & 0xFF);
    }

    for (; carry > 0; i++)
    {
        if (i == a.size())
        {
            a.push_back(carry);
            carry = 0;
        }
        else
        {
            u16 sum = (u16) (a[i] + carry);
            a[i] = (u8)(sum & 0xFF);
            carry = (u8)((sum >> 8) & 0xFF);
        }
    }
}

static
void subtract(tArray& a, const tArray& b)
{
    if (isLess(a, b))
        throw eLogicError("Do not call subtract with a<b.");

    for (size_t i = 0; i < a.size() && i < b.size(); i++)
    {
        u16 top = (u16) a[i];
        if (a[i] < b[i])     // <-- if need to borrow
        {
            size_t j = i+1;
            while (a[j] == 0) a[j++] = 255;
            a[j]--;
            top = (u16)(top + 256);
        }
        a[i] = (u8)(top - b[i]);
    }

    while (a.size() > 0 && a.back() == 0) a.pop_back();
}

static
void multiplyNaive(const tArray& a, const tArray& b,
                   tArray& result)          // does "grade school multiplication"
{
    result.clear();

    if (a.size() == 0 || b.size() == 0)
        return;

    for (size_t i = 0; i < a.size(); i++)
    {
        u8 carry = 0;
        for (size_t j = 0; j < b.size(); j++)
        {
            u16 mult = (u16)(a[i] * b[j] + carry);
            u8 val = (u8) (mult & 0xFF);
            addByte(result, val, i+j);
            carry = (u8) ((mult >> 8) & 0xFF);
        }
        if (carry > 0)
            addByte(result, carry, i+b.size());
    }
}

static
void multiplyByte(tArray& a, u8 b)
{
    if (a.size() == 0)
        return;

    if (b == 0)
    {
        a.clear();
        return;
    }

    u8 carry = 0;
    for (size_t i = 0; i < a.size(); i++)
    {
        u16 mult = (u16)(a[i] * b + carry);
        u8 val = (u8) (mult & 0xFF);
        carry = (u8) ((mult >> 8) & 0xFF);
        a[i] = val;
    }
    if (carry > 0)
        a.push_back(carry);
}

static
void multiplyKaratsuba(const tArray& x, const tArray& y,
                       tArray& result)
{
    if (x.size() <= KARATSUBA_CUTOFF || y.size() <= KARATSUBA_CUTOFF)
    {
        multiplyNaive(x, y, result);
        return;
    }

    size_t n = std::max(numbits(x), numbits(y));
    n = (n / 2) + (n % 2);   // rounds up

    // x = 2^n b + a
    tArray a = x; keepOnly(a, n);
    tArray b = x; shiftRight(b, n);

    // y = 2^n d + c
    tArray c = y; keepOnly(c, n);
    tArray d = y; shiftRight(d, n);


    // Recurse!
    tArray ac;   multiplyKaratsuba(a, c, ac);
    tArray bd;   multiplyKaratsuba(b, d, bd);
    add(a, b);
    add(c, d);
    tArray abcd; multiplyKaratsuba(a, c, abcd);

    // K... For convenience
    tArray k = abcd;
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
void multiply(const tArray& a, const tArray& b,
              tArray& result)
{
    multiplyKaratsuba(a, b, result);
}

static
void divideNaive(const tArray& a, const tArray& b,      // "long division"
            tArray& quotient, tArray& remainder,
            tArray& aux1, tArray& aux2)
{
    quotient.clear();
    remainder.clear();

    // If b is zero, that is bad.
    if (b.size() == 0)
        throw eInvalidArgument("You may not divide by zero!");

    // Long division:
    tArray& mult = aux1;
    tArray& sub = aux2;
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

        i32 left = 0, right = 255, mid;
        while (true)
        {
            mid = (left + right) / 2;
            mult = b;
            multiplyByte(mult, (u8)mid);
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

        quotient.push_back((u8)mid);
        subtract(remainder, mult);
    }

    quotient.reverse();
    while (quotient.size() > 0 && quotient.back() == 0) quotient.pop_back();
}

#if USE_OPENSSL
static
void divideOpenSSL(const tArray& a, const tArray& b,
            tArray& quotient, tArray& remainder,
            tArray& aux1, tArray& aux2)
{
    BIGNUM* bn_a = BN_new(); BN_init(bn_a);
    BIGNUM* bn_b = BN_new(); BN_init(bn_b);
    BIGNUM* bn_quo = BN_new(); BN_init(bn_quo);
    BIGNUM* bn_rem = BN_new(); BN_init(bn_rem);
    BN_CTX* ctx = BN_CTX_new(); BN_CTX_init(ctx);

    aux1 = a;
    aux1.reverse();
    BN_bin2bn(&aux1[0], (int)aux1.size(), bn_a);
    aux1 = b;
    aux1.reverse();
    BN_bin2bn(&aux1[0], (int)aux1.size(), bn_b);

    BN_div(bn_quo, bn_rem, bn_a, bn_b, ctx);

    {
        u8 quo_bf[10000];
        int quo_sz = BN_bn2bin(bn_quo, quo_bf);
        quotient.clear();
        for (int i = 0; i < quo_sz; i++)
            quotient.push_back(quo_bf[i]);
        quotient.reverse();
    }

    {
        u8 rem_bf[10000];
        int rem_sz = BN_bn2bin(bn_rem, rem_bf);
        remainder.clear();
        for (int i = 0; i < rem_sz; i++)
            remainder.push_back(rem_bf[i]);
        remainder.reverse();
    }

    BN_CTX_free(ctx);
    BN_free(bn_a);
    BN_free(bn_b);
    BN_free(bn_quo);
    BN_free(bn_rem);
}
#endif

static
void divide(const tArray& a, const tArray& b,
            tArray& quotient, tArray& remainder,
            tArray& aux1, tArray& aux2)
{
#if USE_OPENSSL
    if (a.size() >= OPENSSL_CUTOFF || b.size() >= OPENSSL_CUTOFF)
        divideOpenSSL(a, b, quotient, remainder, aux1, aux2);
    else
        divideNaive(a, b, quotient, remainder, aux1, aux2);
#else
    divideNaive(a, b, quotient, remainder, aux1, aux2);
#endif
}

static
void modMultiplyNaive(const tArray& a, const tArray& b, const tArray& m,
                      tArray& result,
                      tArray& aux1, tArray& aux2, tArray& aux3, tArray& aux4)
{
    multiply(a, b, aux1);
    divide(aux1, m, aux2, result, aux3, aux4);
}

static
void modMultiplyBlakley(const tArray& a, const tArray& b, const tArray& m,
                      tArray& result,
                      tArray& aux1, tArray& aux2, tArray& aux3, tArray& aux4)
{
    aux1.clear();

    for (int i = (int)a.size()-1; i >= 0; i--)
    {
        u8 byte  = a[i];

        for (int j = 0; j < 8; j++)
        {
            multiplyByte(aux1, 2);

            if (byte & 0x80)
                add(aux1, b);

            while (!isLess(aux1, m)) subtract(aux1, m);

            byte = (u8)(byte << 1);
        }
    }

    result = aux1;
}

static
void modMultiply(const tArray& a, const tArray& b, const tArray& m,
                 tArray& result,
                 tArray& aux1, tArray& aux2, tArray& aux3, tArray& aux4)
{
    if (false)
        modMultiplyNaive(a, b, m, result, aux1, aux2, aux3, aux4);
    modMultiplyBlakley(a, b, m, result, aux1, aux2, aux3, aux4);
}

static
void modPowNaive(const tArray& a, const tArray& e, const tArray& m,
            tArray& result)
{
    result.clear();
    result.push_back(1);

    tArray aux1, aux2, aux3, aux4;
    tArray aa = a;

    for (size_t i = 0; i < e.size(); i++)
    {
        u8 byte = e[i];

        for (int j = 0; j < 8; j++)
        {
            if (byte & 1)
                modMultiply(result, aa, m, result, aux1, aux2, aux3, aux4);
            byte = (u8)(byte >> 1);
            modMultiply(aa, aa, m, aa, aux1, aux2, aux3, aux4);
        }
    }
}

static
void modPowMary(const tArray& a, const tArray& e, const tArray& m,
            tArray& result)   // "m-ary method" (here m=4)
{
    result.clear();
    result.push_back(1);

    tArray aux1, aux2, aux3, aux4;

    tArray tab[16];
    tab[0].push_back(1);
    for (int i = 1; i < 16; i++)
        modMultiply(tab[i-1], a, m, tab[i], aux1, aux2, aux3, aux4);

    for (int i = (int)e.size()-1; i >= 0; i--)
    {
        u8 byte  = e[i];

        u32 high = (byte & 0xF0) >> 4;
        modMultiply(result, result, m, result, aux1, aux2, aux3, aux4);
        modMultiply(result, result, m, result, aux1, aux2, aux3, aux4);
        modMultiply(result, result, m, result, aux1, aux2, aux3, aux4);
        modMultiply(result, result, m, result, aux1, aux2, aux3, aux4);
        modMultiply(result, tab[high], m, result, aux1, aux2, aux3, aux4);

        u32 low  = byte & 0x0F;
        modMultiply(result, result, m, result, aux1, aux2, aux3, aux4);
        modMultiply(result, result, m, result, aux1, aux2, aux3, aux4);
        modMultiply(result, result, m, result, aux1, aux2, aux3, aux4);
        modMultiply(result, result, m, result, aux1, aux2, aux3, aux4);
        modMultiply(result, tab[low], m, result, aux1, aux2, aux3, aux4);
    }
}

static
void modPowCLNW(const tArray& a, const tArray& e, const tArray& m,
            tArray& result)   // "Constant Length Nonzero Windows"
{
    static const u32 kWindowSize = 4;

    result.clear();
    result.push_back(1);

    tArray aux1, aux2, aux3, aux4;

    tArray tab[16];                       // <--- 2^kWindowSize
    tab[0].push_back(1);
    for (int i = 1; i < 16; i++)          // <--- 2^kWindowSize
        modMultiply(tab[i-1], a, m, tab[i], aux1, aux2, aux3, aux4);

    bool zw = true;  // in "zero window"?
    u32 d = 0;       // the number of bits collected from a nonzero-window
    u32 col = 0;     // the d collected bits

    for (int i = (int)e.size()-1; i >= 0; i--)
    {
        u8 byte  = e[i];

        for (int i = 0; i < 8; i++)
        {
            u32 bit = ((byte & 0x80) >> 7);
            byte = (u8)(byte << 1);

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

#if USE_OPENSSL
static
void modPowOpenSSL(const tArray& a, const tArray& e, const tArray& m,
            tArray& result)
{
    BIGNUM* bn_a = BN_new(); BN_init(bn_a);
    BIGNUM* bn_e = BN_new(); BN_init(bn_e);
    BIGNUM* bn_m = BN_new(); BN_init(bn_m);
    BIGNUM* bn_res = BN_new(); BN_init(bn_res);
    BN_CTX* ctx = BN_CTX_new(); BN_CTX_init(ctx);

    tArray aux;
    aux = a;
    aux.reverse();
    BN_bin2bn(&aux[0], (int)aux.size(), bn_a);
    aux = e;
    aux.reverse();
    BN_bin2bn(&aux[0], (int)aux.size(), bn_e);
    aux = m;
    aux.reverse();
    BN_bin2bn(&aux[0], (int)aux.size(), bn_m);

    BN_mod_exp(bn_res, bn_a, bn_e, bn_m, ctx);

    {
        u8 res_bf[10000];
        int res_sz = BN_bn2bin(bn_res, res_bf);
        result.clear();
        for (int i = 0; i < res_sz; i++)
            result.push_back(res_bf[i]);
        result.reverse();
    }

    BN_CTX_free(ctx);
    BN_free(bn_a);
    BN_free(bn_e);
    BN_free(bn_m);
    BN_free(bn_res);
}
#endif

static
void modPow(const tArray& a, const tArray& e, const tArray& m,
            tArray& result)
{
#if USE_OPENSSL
    if (e.size() >= OPENSSL_CUTOFF || m.size() >= OPENSSL_CUTOFF)
        modPowOpenSSL(a, e, m, result);
    else
        modPowNaive(a, e, m, result);
#else
    if (e.size() < 5)
        modPowNaive(a, e, m, result);
    else if (e.size() < 10)
        modPowMary(a, e, m, result);
    else
        modPowCLNW(a, e, m, result);
#endif
}


///////////////////////////////////////////////////////////////////////////////
// Methods
///////////////////////////////////////////////////////////////////////////////

tBigInteger::tBigInteger(i32 val)
{
    neg = val < 0;
    u32 uval = neg ? (u32)(-val) : (u32)val;
    while (uval > 0)
    {
        u8 byte = (u8)(uval & 0xFF);
        uval >>= 8;
        b.push_back(byte);
    }
}

tBigInteger::tBigInteger(string val, int radix)
    : neg(false)
{
    vector<i32> ints;

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
                    neg = true;
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
                    neg = true;
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

    tArray mult(1, 1);
    tArray toadd;
    for (int i = (int)ints.size()-1; i >= 0; i--)
    {
        if (ints[i] != 0)
        {
            toadd = mult;
            multiplyByte(toadd, (u8)ints[i]);
            add(b, toadd);
        }
        multiplyByte(mult, (u8)radix);
    }
}

tBigInteger::tBigInteger(vector<u8> bytes)
    : b(), neg(false)
{
    for (size_t i = 0; i < bytes.size(); i++)
        b.push_back(bytes[i]);
    while (b.size() > 0 && b.back() == 0) b.pop_back();
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

    tArray n = b;
    tArray quo, rem;
    tArray radixVect(1, (u8)radix);
    tArray aux1, aux2;
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
    return vector<u8>(&b[0], &b[b.size()]);
}

bool tBigInteger::isNegative() const
{
    return !isZero() && neg;
}

tBigInteger& tBigInteger::setIsNegative(bool n)
{
    neg = n;
    return *this;
}

bool tBigInteger::isZero() const
{
    return (b.size() == 0);   // (b.size() == 0)  <==>  (*this == 0)
}

bool tBigInteger::isOdd() const
{
    return !isZero() && ((b[0] & 1) > 0);
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
    o.neg = !o.neg;
    return o;
}

void tBigInteger::operator+= (const tBigInteger& o)
{
    if (neg && !o.neg)
    {
        *this = o - (-(*this));
        return;
    }

    if (!neg && o.neg)
    {
        *this -= -o;
        return;
    }

    // At this point, *this and o have the same sign.

    add(b, o.b);
}

void tBigInteger::operator-= (const tBigInteger& o)
{
    if (neg && !o.neg)
    {
        *this += -o;
        return;
    }

    if (!neg && o.neg)
    {
        *this += -o;
        return;
    }

    if (neg && o.neg)
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

    subtract(b, o.b);
}

void tBigInteger::operator*= (const tBigInteger& o)
{
    tArray result;
    multiply(b, o.b, result);
    b = result;
    neg = (isNegative() && !o.isNegative()) || (!isNegative() && o.isNegative());
}

void tBigInteger::operator/= (const tBigInteger& o)
{
    tArray quotient;
    tArray remainder;
    divide(b, o.b, quotient, remainder, m_aux1, m_aux2);
    b = quotient;
    neg = (isNegative() && !o.isNegative()) || (!isNegative() && o.isNegative());
}

void tBigInteger::operator%= (const tBigInteger& o)
{
    tArray quotient;
    tArray remainder;
    divide(b, o.b, quotient, remainder, m_aux1, m_aux2);
    b = remainder;
}

tBigInteger tBigInteger::modPow(const tBigInteger& e, const tBigInteger& m) const
{
    tArray result;
    rho::algo::modPow(b, e.b, m.b, result);
    tBigInteger bi(0);
    bi.b = result;
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
    return (b == o.b) && (isNegative() == o.isNegative());
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

    if (b.size() != o.b.size())
    {
        bool less = b.size() < o.b.size();
        return isNegative() ? !less : less;
    }
    else
    {
        for (int i = (int)b.size()-1; i >=0; i--)
            if (b[i] != o.b[i])
                return isNegative() ? o.b[i] < b[i] : b[i] < o.b[i];

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


}    // namespace algo
}    // namespace rho
