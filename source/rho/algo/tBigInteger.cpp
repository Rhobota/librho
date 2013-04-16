#include <rho/algo/tBigInteger.h>

#include <sstream>

using namespace std;


namespace rho
{
namespace algo
{


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

void throwInvalidValStringException(char c, int radix)
{
    ostringstream o;
    o << "Invalid 'val' string character: " << c
      << " for radix: " + radix;
    throw eInvalidArgument(o.str());
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
                if (val[i] == '-')
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
                if (val[i] == '-')
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
                if (val[i] == '-')
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
                if (val[i] == '-')
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
            throw eInvalidArgument(string() + "Unsupported radix");
    }

    tBigInteger mult(1);
    for (int i = (int)ints.size()-1; i >= 0; i--)
    {
        tBigInteger add = mult * ints[i];
        *this += add;
        mult *= radix;
    }
}

tBigInteger::tBigInteger(vector<u8> bytes)
    : b(bytes), neg(false)
{
}

vector<u8> tBigInteger::getBytes() const
{
    return b;
}

bool tBigInteger::isNegative() const
{
    return neg;
}

string tBigInteger::toString(int radix) const
{
    throw eNotImplemented("This method requires that operator/ be implemented...");
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

    while (o.b.size() > b.size())
        b.push_back(0);

    u8 carry = 0;
    size_t i;
    for (i = 0; i < b.size() && i < o.b.size(); i++)
    {
        u16 sum = (u16) (b[i] + o.b[i] + carry);
        b[i] = (u8)(sum & 0xFF);
        carry = (u8)((sum >> 8) & 0xFF);
    }

    for (; carry > 0; i++)
    {
        if (i == b.size())
        {
            b.push_back(carry);
            carry = 0;
        }
        else
        {
            u16 sum = (u16) (b[i] + carry);
            b[i] = (u8)(sum & 0xFF);
            carry = (u8)((sum >> 8) & 0xFF);
        }
    }
}

void tBigInteger::operator-= (const tBigInteger& o)
{
    if (neg && !o.neg)
    {
        *this = o + (-(*this));
        return;
    }

    if (!neg && o.neg)
    {
        *this += -o;
        return;
    }

    while (o.b.size() > b.size())
        b.push_back(0);

    for (size_t i = 0; i < b.size() && i < o.b.size(); i++)
    {
        u16 top = (u16) b[i];
        if (b[i] < o.b[i])     // <-- if need to borrow
        {
            size_t j = i+1;
            while (j < b.size() && b[j] == 0) b[j++] = 255;
            if (j == b.size())
                neg = true;
            else
                b[j]--;
            top = (u16)(top + 256);
        }
        b[i] = (u8)(top - o.b[i]);
    }
}

void tBigInteger::operator*= (const tBigInteger& o)
{
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


}    // namespace algo
}    // namespace rho
