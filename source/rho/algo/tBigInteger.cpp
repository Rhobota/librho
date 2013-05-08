#include <rho/algo/tBigInteger.h>
#include <sstream>
using namespace std;


#define KARATSUBA_CUTOFF 50


namespace rho
{
namespace algo
{


///////////////////////////////////////////////////////////////////////////////
// Util function
///////////////////////////////////////////////////////////////////////////////

void print(const vector<u8>& v)
{
    if (v.size() == 0)
        cout << " 0";
    for (int i = (int)v.size()-1; i >= 0; i--)
        cout << " " << (u32)(v[i]);
    cout << endl;
}

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
bool isLess(const vector<u8>& a, const vector<u8>& b)
{
    if (a.size() != b.size()) return a.size() < b.size();
    for (int i = (int)a.size()-1; i >= 0; i--)
        if (a[i] != b[i])
            return a[i] < b[i];
    return false;
}

static
void cleanup(vector<u8>& v)
{
    while (v.size() > 0 && v.back() == 0) v.pop_back();
}

static
size_t numbits(const vector<u8>& v)
{
    if (v.size() == 0)
        return 0;

    u8 b = v.back();
    size_t i = 0;
    while (b != 0) { b /= 2; i++; }

    return (v.size()-1)*8 + i;
}

static
void shiftRight(vector<u8>& v, size_t n)
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
void shiftLeft(vector<u8>& v, size_t n)
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
void keepOnly(vector<u8>& v, size_t n)
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
void add(vector<u8>& a, const vector<u8>& b, size_t bShift = 0)
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
void addByte(vector<u8>& a, u8 b, size_t bShift = 0)
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
void subtract(vector<u8>& a, const vector<u8>& b)
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
void multiplyNaive(const vector<u8>& a, const vector<u8>& b,
                   vector<u8>& result)                          // does "grade school multiplication"
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
void multiplyByte(vector<u8>& a, u8 b)
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
void multiplyKaratsuba(const vector<u8>& x, const vector<u8>& y,
                      vector<u8>& result)
{
    if (x.size() <= KARATSUBA_CUTOFF || y.size() <= KARATSUBA_CUTOFF)
    {
        multiplyNaive(x, y, result);
        return;
    }

    size_t n = std::max(numbits(x), numbits(y));
    n = (n / 2) + (n % 2);   // rounds up

    // x = 2^n b + a
    vector<u8> a = x; keepOnly(a, n);
    vector<u8> b = x; shiftRight(b, n);

    // y = 2^n d + c
    vector<u8> c = y; keepOnly(c, n);
    vector<u8> d = y; shiftRight(d, n);


    // Recurse!
    vector<u8> ac;   multiplyKaratsuba(a, c, ac);
    vector<u8> bd;   multiplyKaratsuba(b, d, bd);
    add(a, b);
    add(c, d);
    vector<u8> abcd; multiplyKaratsuba(a, c, abcd);

    // K... For convenience
    vector<u8> k = abcd;
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
void multiply(const vector<u8>& a, const vector<u8>& b,
              vector<u8>& result)
{
    multiplyKaratsuba(a, b, result);
}

static
void divide(const vector<u8>& a, const vector<u8>& b,
            vector<u8>& quotient, vector<u8>& remainder)      // "long division"
{
    quotient.clear();
    remainder.clear();

    // If b is zero, that is bad.
    if (b.size() == 0)
        throw eInvalidArgument("You may not divide by zero!");

    // Long division:
    vector<u8> mult;
    vector<u8> sub;
    for (int i = (int)a.size()-1; i >= 0; i--)
    {
        if (remainder.size() > 0 || a[i] != 0)
            remainder.insert(remainder.begin(), a[i]);
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

    quotient = vector<u8>(quotient.rbegin(), quotient.rend());
    while (quotient.size() > 0 && quotient.back() == 0) quotient.pop_back();
}

static
void modPow(const vector<u8>& a, const vector<u8>& e, const vector<u8>& m,
            vector<u8>& result)
{
    result.clear();
    result.push_back(1);

    vector<u8> temp;
    vector<u8> trash;

    vector<u8> aa = a;

    for (size_t i = 0; i < e.size(); i++)
    {
        u8 byte = e[i];

        for (int j = 0; j < 8; j++)
        {
            if (byte & 1)
            {
                temp = result;                    //
                multiply(temp, aa, result);       // result *= aa

                temp = result;                    //
                divide(temp, m, trash, result);   // result %= m
            }

            byte = (u8)(byte >> 1);

            if (byte == 0 && i == e.size()-1)
                break;

            multiply(aa, aa, temp);           //
            aa = temp;                        // aa = aa^^2

            divide(aa, m, trash, temp);       //
            aa = temp;                        // aa %= m
        }
    }
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

    vector<u8> mult(1, 1);
    vector<u8> toadd;
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
    : b(bytes), neg(false)
{
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

    vector<u8> n = b;
    vector<u8> quo, rem;
    vector<u8> radixVect(1, (u8)radix);
    while (n.size() > 0)
    {
        divide(n, radixVect, quo, rem);
        n = quo;
        str += (rem.size() == 0) ? '0' : toChar(rem[0]);
    }

    if (str == "")    str = "0";
    if (isNegative()) str += "-";

    return string(str.rbegin(), str.rend());
}

const vector<u8>& tBigInteger::getBytes() const
{
    return b;
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
    vector<u8> result;
    multiply(b, o.b, result);
    b = result;
    neg = (isNegative() && !o.isNegative()) || (!isNegative() && o.isNegative());
}

void tBigInteger::operator/= (const tBigInteger& o)
{
    vector<u8> quotient;
    vector<u8> remainder;
    divide(b, o.b, quotient, remainder);
    b = quotient;
    neg = (isNegative() && !o.isNegative()) || (!isNegative() && o.isNegative());
}

void tBigInteger::operator%= (const tBigInteger& o)
{
    vector<u8> quotient;
    vector<u8> remainder;
    divide(b, o.b, quotient, remainder);
    b = remainder;
}

tBigInteger tBigInteger::modPow(const tBigInteger& e, const tBigInteger& m) const
{
    vector<u8> result;
    rho::algo::modPow(b, e.b, m.b, result);
    return tBigInteger(result);
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
