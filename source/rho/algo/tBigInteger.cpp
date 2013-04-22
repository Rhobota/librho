#include <rho/algo/tBigInteger.h>

#include <sstream>

using namespace std;


namespace rho
{
namespace algo
{


///////////////////////////////////////////////////////////////////////////////
// Util function
///////////////////////////////////////////////////////////////////////////////

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
bool less(const vector<u8>& a, const vector<u8>& b)
{
    if (a.size() != b.size()) return a.size() < b.size();
    for (int i = (int)a.size()-1; i >= 0; i--)
        if (a[i] != b[i])
            return a[i] < b[i];
    return false;
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
void addbyte(vector<u8>& a, u8 b, size_t bShift = 0)
{
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
    if (less(a, b))
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
void multiply(const vector<u8>& a, const vector<u8>& b,
              vector<u8>& result)
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
            addbyte(result, val, i+j);
            carry = (u8) ((mult >> 8) & 0xFF);
        }
        if (carry > 0)
            addbyte(result, carry, i+b.size());
    }
}

static
void multiplybyte(vector<u8>& a, u8 b)
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
void divide(const vector<u8>& a, const vector<u8>& b,
            vector<u8>& quotient, vector<u8>& remainder)
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
        if (less(remainder, b))
        {
            quotient.push_back(0);
            continue;
        }

        i32 left = 0, right = 255, mid;
        while (true)
        {
            mid = (left + right) / 2;
            mult = b;
            multiplybyte(mult, (u8)mid);
            if (less(remainder, mult))
                right = mid - 1;
            else
            {
                sub = remainder;
                subtract(sub, mult);
                if (less(b, sub) || b == sub)
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
        {
            for (size_t i = 0; i < val.size(); i++)
            {
                if (i == 0 && val[i] == '-')
                {
                    neg = true;
                    continue;
                }
                if (val[i] < '0' || val[i] > '1')
                    throwInvalidValStringException(val[i], radix);
                ints.push_back(val[i] - '0');
            }
        }
        break;

        case 8:
        {
            for (size_t i = 0; i < val.size(); i++)
            {
                if (i == 0 && val[i] == '-')
                {
                    neg = true;
                    continue;
                }
                if (val[i] < '0' || val[i] > '7')
                    throwInvalidValStringException(val[i], radix);
                ints.push_back(val[i] - '0');
            }
        }
        break;

        case 10:
        {
            for (size_t i = 0; i < val.size(); i++)
            {
                if (i == 0 && val[i] == '-')
                {
                    neg = true;
                    continue;
                }
                if (val[i] < '0' || val[i] > '9')
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

    bool savedNeg = neg;   // it will be clobbered by the first '+=' below.

    tBigInteger mult(1);
    for (int i = (int)ints.size()-1; i >= 0; i--)
    {
        if (ints[i] != 0)
        {
            tBigInteger add = mult * ints[i];
            *this += add;
        }
        mult *= radix;
    }

    neg = savedNeg;
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

    tBigInteger n = isNegative() ? -(*this) : *this;
    while (n != 0)
    {
        tBigInteger m = n % radix;
        n /= radix;
        str += m.isZero() ? '0' : toChar(m.b[0]);
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
    tBigInteger a = *this;
    tBigInteger res(1);

    for (size_t i = 0; i < e.b.size(); i++)
    {
        u8 byte = e.b[i];

        for (int j = 0; j < 8; j++)
        {
            if (byte & 1)
            {
                res *= a;
                res %= m;
            }
            byte = (u8)(byte >> 1);

            if (byte == 0 && i == e.b.size()-1)
                break;

            a *= a;   // a = a^^2
            a %= m;
        }
    }

    return res;
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
