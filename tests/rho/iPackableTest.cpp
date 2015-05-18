#include <rho/iPackable.h>
#include <rho/tTest.h>
#include <rho/tCrashReporter.h>
#include <rho/sync/tTimer.h>

#include <cstdlib>
#include <cmath>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>

using namespace rho;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::map;


static const int kRandIters = 50000;


class tMyPackable : public iPackable
{
    public:

        tMyPackable()
            : a(0), b(0), c(0), d(0), e(0), f(0)
        {
        }

        void pack(iWritable* out) const
        {
            rho::pack(out, a);
            rho::pack(out, b);
            rho::pack(out, c);
            rho::pack(out, d);
            rho::pack(out, e);
            rho::pack(out, f);
            rho::pack(out, str);
            rho::pack(out, byteVector);
            rho::pack(out, intVector);
            rho::pack(out, stringVector);
            rho::pack(out, stringMatrix);
            rho::pack(out, stringMap);
        }

        void unpack(iReadable* in)
        {
            rho::unpack(in, a);
            rho::unpack(in, b);
            rho::unpack(in, c);
            rho::unpack(in, d);
            rho::unpack(in, e);
            rho::unpack(in, f);
            rho::unpack(in, str);
            rho::unpack(in, byteVector);
            rho::unpack(in, intVector);
            rho::unpack(in, stringVector);
            rho::unpack(in, stringMatrix);
            rho::unpack(in, stringMap);
        }

        void randomize()
        {
            a = (u8)  (rand() % (1<<8));
            b = (i8)  ((rand() % (1<<8)) - (1<<7));
            c = (u16) (rand() % (1<<16));
            d = (i16) ((rand() % (1<<16)) - (1<<15));
            e = (u32) (rand() % (1LL<<32));
            f = (i32) ((rand() % (1LL<<32)) - (1<<31));

            int len = rand() % 500;
            for (int i = 0; i < len; i++)
                str += (char)((rand() % 26) + 'a');

            len = rand() % 500;
            for (int i = 0; i < len; i++)
                byteVector.push_back((u8) (rand() % 256));

            len = rand() % 500;
            for (int i = 0; i < len; i++)
                intVector.push_back((i16) ((rand() % (1<<16)) - (1<<15)));

            len = rand() % 100;
            for (int i = 0; i < len; i++)
            {
                string s;
                int len2 = rand() % 10;
                for (int i = 0; i < len2; i++)
                    s += (char)((rand() % 26) + 'a');
                stringVector.push_back(s);
            }

            len = rand() % 10;
            for (int i = 0; i < len; i++)
            {
                vector<string> vs;
                int len2 = rand() % 10;
                for (int j = 0; j < len2; j++)
                {
                    string s;
                    int len3 = rand() % 10;
                    for (int i = 0; i < len3; i++)
                        s += (char) (((rand() % 26) + 'a'));
                    vs.push_back(s);
                }
                stringMatrix.push_back(vs);
            }

            len = rand() % 10;
            for (int i = 0; i < len; i++)
            {
                vector<string> vs;
                for (int j = 0; j < 2; j++)
                {
                    string s;
                    int len3 = rand() % 10;
                    for (int i = 0; i < len3; i++)
                        s += (char) (((rand() % 26) + 'a'));
                    vs.push_back(s);
                }
                stringMap[vs[0]] = vs[1];
            }
        }

        bool operator== (const tMyPackable& other) const
        {
            if (a != other.a) return false;
            if (b != other.b) return false;
            if (c != other.c) return false;
            if (d != other.d) return false;
            if (e != other.e) return false;
            if (f != other.f) return false;

            if (str != other.str) return false;

            if (byteVector != other.byteVector) return false;

            if (intVector != other.intVector) return false;

            if (stringVector != other.stringVector) return false;

            if (stringMatrix != other.stringMatrix) return false;

            if (stringMap != other.stringMap) return false;

            return true;
        }

        void print(std::ostream& out) const
        {
            out << "a = " << (int)a << endl;
            out << "b = " << (int)b << endl;
            out << "c = " << c << endl;
            out << "d = " << d << endl;
            out << "e = " << e << endl;
            out << "f = " << f << endl;

            out << "str = " << str << endl;

            out << "byteVector =";
            for (size_t i = 0; i < byteVector.size(); i++)
                out << " " << (int)byteVector[i];
            out << endl;

            out << "intVector =";
            for (size_t i = 0; i < intVector.size(); i++)
                out << " " << intVector[i];
            out << endl;

            out << "stringVector =";
            for (size_t i = 0; i < stringVector.size(); i++)
                out << " " << stringVector[i];
            out << endl;

            out << "stringMatrix =";
            for (size_t i = 0; i < stringMatrix.size(); i++)
            {
                for (size_t j = 0; j < stringMatrix[i].size(); j++)
                    out << " " << stringMatrix[i][j];
                out << endl;
            }
            out << endl;

            out << "stringMap =";
            map<string,string>::const_iterator itr;
            for (itr = stringMap.begin(); itr != stringMap.end(); itr++)
            {
                out << itr->first << " --> " << itr->second << endl;
            }
            out << endl;
        }

    private:

        u8 a;
        i8 b;
        u16 c;
        i16 d;
        u32 e;
        i32 f;

        string str;

        vector<u8> byteVector;
        vector<i16> intVector;
        vector<string> stringVector;
        vector< vector<string> > stringMatrix;

        map<string,string> stringMap;
};


void u8test(const tTest& t)
{
    u8 v1 = 98;    // binary 0110 0010
    u8 v2 = 148;   // binary 1001 0100
    u8 v3 = 0, v4 = 0;
    tByteWritable out;
    pack(&out, v1);
    pack(&out, v2);
    t.assert(v1 == 98);
    t.assert(v2 == 148);
    vector<u8> buf = out.getBuf();
    t.assert(buf.size() == 2);
    t.assert(buf[0] == 98);
    t.assert(buf[1] == 148);
    tByteReadable in(buf);
    unpack(&in, v3);
    unpack(&in, v4);
    t.assert(v3 == 98);
    t.assert(v4 == 148);
}

void i8test(const tTest& t)
{
    i8 v1 = 98;    // binary 0110 0010
    i8 v2 = -38;   // binary 1101 1010
    i8 v3 = 0, v4 = 0;
    tByteWritable out;
    pack(&out, v1);
    pack(&out, v2);
    t.assert(v1 == 98);
    t.assert(v2 == -38);
    vector<u8> buf = out.getBuf();
    t.assert(buf.size() == 2);
    t.assert(buf[0] == 98);
    t.assert(buf[1] == 218);
    tByteReadable in(buf);
    unpack(&in, v3);
    unpack(&in, v4);
    t.assert(v3 == 98);
    t.assert(v4 == -38);
}

void u16test(const tTest& t)
{
    u16 v1 = 17114;    // binary 0100 0010 1101 1010
    u16 v2 = 49886;    // binary 1100 0010 1101 1110
    u16 v3 = 0, v4 = 0;
    tByteWritable out;
    pack(&out, v1);
    pack(&out, v2);
    t.assert(v1 == 17114);
    t.assert(v2 == 49886);
    vector<u8> buf = out.getBuf();
    t.assert(buf.size() == 4);
    t.assert(buf[0] == 0x42);    // 0100 0010
    t.assert(buf[1] == 0xDA);    // 1101 1010
    t.assert(buf[2] == 0xC2);    // 1100 0010
    t.assert(buf[3] == 0xDE);    // 1101 1110
    tByteReadable in(buf);
    unpack(&in, v3);
    unpack(&in, v4);
    t.assert(v3 == 17114);
    t.assert(v4 == 49886);
}

void i16test(const tTest& t)
{
    i16 v1 = 17114;     // binary 0100 0010 1101 1010
    i16 v2 = -15650;    // binary 1100 0010 1101 1110
    i16 v3 = 0, v4 = 0;
    tByteWritable out;
    pack(&out, v1);
    pack(&out, v2);
    t.assert(v1 == 17114);
    t.assert(v2 == -15650);
    vector<u8> buf = out.getBuf();
    t.assert(buf.size() == 4);
    t.assert(buf[0] == 0x42);    // 0100 0010
    t.assert(buf[1] == 0xDA);    // 1101 1010
    t.assert(buf[2] == 0xC2);    // 1100 0010
    t.assert(buf[3] == 0xDE);    // 1101 1110
    tByteReadable in(buf);
    unpack(&in, v3);
    unpack(&in, v4);
    t.assert(v3 == 17114);
    t.assert(v4 == -15650);
}

void u32test(const tTest& t)
{
    u32 v1 = 1413227351u; // binary 0101 0100 0011 1100 0010 0011 0101 0111
    u32 v2 = 2991341219u; // binary 1011 0010 0100 1100 0011 1110 1010 0011
    u32 v3 = 0, v4 = 0;
    tByteWritable out;
    pack(&out, v1);
    pack(&out, v2);
    t.assert(v1 == 1413227351u);
    t.assert(v2 == 2991341219u);
    vector<u8> buf = out.getBuf();
    t.assert(buf.size() == 8);
    t.assert(buf[0] == 0x54);    // 0101 0100
    t.assert(buf[1] == 0x3C);    // 0011 1100
    t.assert(buf[2] == 0x23);    // 0010 0011
    t.assert(buf[3] == 0x57);    // 0101 0111
    t.assert(buf[4] == 0xB2);    // 1011 0010
    t.assert(buf[5] == 0x4C);    // 0100 1100
    t.assert(buf[6] == 0x3E);    // 0011 1110
    t.assert(buf[7] == 0xA3);    // 1010 0011
    tByteReadable in(buf);
    unpack(&in, v3);
    unpack(&in, v4);
    t.assert(v3 == 1413227351u);
    t.assert(v4 == 2991341219u);
}

void i32test(const tTest& t)
{
    i32 v1 = 1413227351;  // binary 0101 0100 0011 1100 0010 0011 0101 0111
    i32 v2 = -1303626077; // binary 1011 0010 0100 1100 0011 1110 1010 0011
    i32 v3 = 0, v4 = 0;
    tByteWritable out;
    pack(&out, v1);
    pack(&out, v2);
    t.assert(v1 == 1413227351);
    t.assert(v2 == -1303626077);
    vector<u8> buf = out.getBuf();
    t.assert(buf.size() == 8);
    t.assert(buf[0] == 0x54);    // 0101 0100
    t.assert(buf[1] == 0x3C);    // 0011 1100
    t.assert(buf[2] == 0x23);    // 0010 0011
    t.assert(buf[3] == 0x57);    // 0101 0111
    t.assert(buf[4] == 0xB2);    // 1011 0010
    t.assert(buf[5] == 0x4C);    // 0100 1100
    t.assert(buf[6] == 0x3E);    // 0011 1110
    t.assert(buf[7] == 0xA3);    // 1010 0011
    tByteReadable in(buf);
    unpack(&in, v3);
    unpack(&in, v4);
    t.assert(v3 == 1413227351);
    t.assert(v4 == -1303626077);
}

void u64test(const tTest& t)
{
    u64 v1 = 1413227351u + (0x43uLL << 56);   // same as the u32 test, but with some extra
    u64 v2 = 2991341219u + (0xDAuLL << 56);   // same as the u32 test, but with some extra
    u64 v3 = 0, v4 = 0;
    tByteWritable out;
    pack(&out, v1);
    pack(&out, v2);
    t.assert(v1 == 1413227351u + (0x43uLL << 56));
    t.assert(v2 == 2991341219u + (0xDAuLL << 56));
    vector<u8> buf = out.getBuf();
    t.assert(buf.size() == 16);
    t.assert(buf[0]  == 0x43);    // 0100 0011
    t.assert(buf[1]  == 0x00);    // 0000 0000
    t.assert(buf[2]  == 0x00);    // 0000 0000
    t.assert(buf[3]  == 0x00);    // 0000 0000
    t.assert(buf[4]  == 0x54);    // 0101 0100
    t.assert(buf[5]  == 0x3C);    // 0011 1100
    t.assert(buf[6]  == 0x23);    // 0010 0011
    t.assert(buf[7]  == 0x57);    // 0101 0111
    t.assert(buf[8]  == 0xDA);    // 1101 1010
    t.assert(buf[9]  == 0x00);    // 0000 0000
    t.assert(buf[10] == 0x00);    // 0000 0000
    t.assert(buf[11] == 0x00);    // 0000 0000
    t.assert(buf[12] == 0xB2);    // 1011 0010
    t.assert(buf[13] == 0x4C);    // 0100 1100
    t.assert(buf[14] == 0x3E);    // 0011 1110
    t.assert(buf[15] == 0xA3);    // 1010 0011
    tByteReadable in(buf);
    unpack(&in, v3);
    unpack(&in, v4);
    t.assert(v3 == 1413227351u + (0x43uLL << 56));
    t.assert(v4 == 2991341219u + (0xDAuLL << 56));
}

void i64test(const tTest& t)
{
    // It should work if the u64 test works...
    u64test(t);
}

void packunpackf32test(const tTest& t, f32 x, f32& y)
{
    tByteWritable out;
    pack(&out, x);
    vector<u8> buf = out.getBuf();
    t.assert(buf.size() == 8);
    tByteReadable in(buf);
    unpack(&in, y);
}

void packunpackf64test(const tTest& t, f64 x, f64& y)
{
    tByteWritable out;
    pack(&out, x);
    vector<u8> buf = out.getBuf();
    t.assert(buf.size() == 12);
    tByteReadable in(buf);
    unpack(&in, y);
}

void f32test(const tTest& t)
{
    // Nan test
    {
        f32 nan = NAN;
        f32 nan2 = 0.0f;
        t.assert(std::isnan(nan));
        t.reject(std::isnan(nan2));
        packunpackf32test(t, nan, nan2);
        t.assert(std::isnan(nan));
        t.assert(std::isnan(nan2));
    }

    // Infinity test
    {
        f32 inf = INFINITY;
        f32 inf2 = 0.0f;
        t.reject(std::isfinite(inf));
        t.assert(std::isfinite(inf2));
        t.assert(inf == INFINITY);
        t.reject(inf2 == INFINITY);
        packunpackf32test(t, inf, inf2);
        t.reject(std::isfinite(inf));
        t.reject(std::isfinite(inf2));
        t.assert(inf == INFINITY);
        t.assert(inf2 == INFINITY);
    }

    // Negative infinity test
    {
        f32 ninf = -INFINITY;
        f32 ninf2 = 0.0f;
        t.reject(std::isfinite(ninf));
        t.assert(std::isfinite(ninf2));
        t.assert(ninf == -INFINITY);
        t.reject(ninf2 == -INFINITY);
        packunpackf32test(t, ninf, ninf2);
        t.reject(std::isfinite(ninf));
        t.reject(std::isfinite(ninf2));
        t.assert(ninf == -INFINITY);
        t.assert(ninf2 == -INFINITY);
    }

    // Zero test
    {
        f32 zero = 0.0f;
        f32 zero2 = NAN;
        t.assert(std::isfinite(zero));
        t.reject(std::isfinite(zero2));
        t.assert(zero == 0.0f);
        t.reject(zero2 == 0.0f);
        t.reject(std::isnan(zero));
        t.assert(std::isnan(zero2));
        packunpackf32test(t, zero, zero2);
        t.assert(std::isfinite(zero));
        t.assert(std::isfinite(zero2));
        t.assert(zero == 0.0f);
        t.assert(zero2 == 0.0f);
        t.reject(std::isnan(zero));
        t.reject(std::isnan(zero2));
    }

    // Randomized tests.
    for (int i = 0; i < kRandIters; i++)
    {
        f32 x;
        u8* p = (u8*)(&x);
        for (int j = 0; j < 4; j++)
            p[j] = (u8) (rand() % 256);
        if (std::isnan(x))
            continue;
        f32 y = NAN;
        t.assert(x != y);
        packunpackf32test(t, x, y);
        t.assert(x == y);
    }
}

void f32posinftest(const tTest& t)
{
    f32 inf = INFINITY;
    t.assert(inf == INFINITY);
    t.reject(std::isfinite(inf));

    f32 big = 1e30f;
    f32 smal = 1.0f;
    t.assert(std::isfinite(big));
    t.assert(std::isfinite(smal));

    t.assert(inf > big);
    t.assert(inf > smal);
    t.assert(big < inf);
    t.assert(smal < inf);
    t.assert(big != inf);
    t.assert(big != inf);

    t.reject(inf < big);
    t.reject(inf < smal);
    t.reject(big > inf);
    t.reject(smal > inf);
    t.reject(big == inf);
    t.reject(smal == inf);

    t.reject(inf > inf);
    t.reject(inf < inf);
    t.reject(inf != inf);
    t.assert(inf == inf);
    t.assert(inf >= inf);
    t.assert(inf <= inf);

    t.assert(inf*big == INFINITY);
    t.reject(std::isfinite(inf*big));
    t.assert(inf*4.0f == INFINITY);
    t.reject(std::isfinite(inf*4.0f));
    t.assert(inf+big == INFINITY);
    t.reject(std::isfinite(inf+big));
    t.assert(inf+4.0f == INFINITY);
    t.reject(std::isfinite(inf+4.0f));
    t.assert(inf-big == INFINITY);
    t.reject(std::isfinite(inf-big));
    t.assert(inf-4.0f == INFINITY);
    t.reject(std::isfinite(inf-4.0f));
}

void f32neginftest(const tTest& t)
{
    f32 ninf = -INFINITY;
    t.assert(ninf == -INFINITY);
    t.reject(std::isfinite(ninf));

    t.assert(ninf < INFINITY);
    t.assert(ninf <= INFINITY);
    t.assert(ninf != INFINITY);
    t.reject(ninf > INFINITY);
    t.reject(ninf >= INFINITY);
    t.reject(ninf == INFINITY);

    f32 big = 1e30f;
    f32 smal = 1.0f;
    t.assert(std::isfinite(big));
    t.assert(std::isfinite(smal));

    t.assert(ninf < big);
    t.assert(ninf < smal);
    t.assert(big > ninf);
    t.assert(smal > ninf);
    t.assert(big != ninf);
    t.assert(big != ninf);

    t.reject(ninf > big);
    t.reject(ninf > smal);
    t.reject(big < ninf);
    t.reject(smal < ninf);
    t.reject(big == ninf);
    t.reject(smal == ninf);

    t.reject(ninf > ninf);
    t.reject(ninf < ninf);
    t.reject(ninf != ninf);
    t.assert(ninf == ninf);
    t.assert(ninf >= ninf);
    t.assert(ninf <= ninf);

    t.assert(ninf*big == -INFINITY);
    t.reject(std::isfinite(ninf*big));
    t.assert(ninf*4.0f == -INFINITY);
    t.reject(std::isfinite(ninf*4.0f));
    t.assert(ninf+big == -INFINITY);
    t.reject(std::isfinite(ninf+big));
    t.assert(ninf+4.0f == -INFINITY);
    t.reject(std::isfinite(ninf+4.0f));
    t.assert(ninf-big == -INFINITY);
    t.reject(std::isfinite(ninf-big));
    t.assert(ninf-4.0f == -INFINITY);
    t.reject(std::isfinite(ninf-4.0f));
}

void f32nantest(const tTest& t)
{
    f32 nan = NAN;
    t.assert(std::isnan(nan));
    t.assert(std::isnan(sqrtf(-1.0f)));
    t.reject(std::isfinite(nan));

    t.reject(nan == nan);
    t.assert(nan != nan);  // <-- this is the only one that evaluates to true!
    t.reject(nan > nan);
    t.reject(nan < nan);
    t.reject(nan >= nan);
    t.reject(nan <= nan);
    t.reject(nan < INFINITY);
    t.reject(nan > INFINITY);
    t.reject(nan < -INFINITY);
    t.reject(nan > -INFINITY);

    f32 big = 1e30f;
    f32 smal = 1.0f;
    t.reject(std::isnan(big));
    t.reject(std::isnan(smal));

    t.assert(std::isnan(nan*big));
    t.assert(std::isnan(nan*smal));
    t.assert(std::isnan(nan*nan));
    t.assert(std::isnan(nan/big));
    t.assert(std::isnan(nan/smal));
    t.assert(std::isnan(nan/nan));
    t.assert(std::isnan(nan+big));
    t.assert(std::isnan(nan+smal));
    t.assert(std::isnan(nan+nan));
    t.assert(std::isnan(nan-big));
    t.assert(std::isnan(nan-smal));
    t.assert(std::isnan(nan-nan));
}

void f64test(const tTest& t)
{
    // Nan test
    {
        f64 nan = NAN;
        f64 nan2 = 0.0;
        t.assert(std::isnan(nan));
        t.reject(std::isnan(nan2));
        packunpackf64test(t, nan, nan2);
        t.assert(std::isnan(nan));
        t.assert(std::isnan(nan2));
    }

    // Infinity test
    {
        f64 inf = INFINITY;
        f64 inf2 = 0.0;
        t.reject(std::isfinite(inf));
        t.assert(std::isfinite(inf2));
        t.assert(inf == (f64)INFINITY);
        t.reject(inf2 == (f64)INFINITY);
        packunpackf64test(t, inf, inf2);
        t.reject(std::isfinite(inf));
        t.reject(std::isfinite(inf2));
        t.assert(inf == (f64)INFINITY);
        t.assert(inf2 == (f64)INFINITY);
    }

    // Negative infinity test
    {
        f64 ninf = -INFINITY;
        f64 ninf2 = 0.0;
        t.reject(std::isfinite(ninf));
        t.assert(std::isfinite(ninf2));
        t.assert(ninf == (f64)(-INFINITY));
        t.reject(ninf2 == (f64)(-INFINITY));
        packunpackf64test(t, ninf, ninf2);
        t.reject(std::isfinite(ninf));
        t.reject(std::isfinite(ninf2));
        t.assert(ninf == (f64)(-INFINITY));
        t.assert(ninf2 == (f64)(-INFINITY));
    }

    // Zero test
    {
        f64 zero = 0.0;
        f64 zero2 = NAN;
        t.assert(std::isfinite(zero));
        t.reject(std::isfinite(zero2));
        t.assert(zero == 0.0);
        t.reject(zero2 == 0.0);
        t.reject(std::isnan(zero));
        t.assert(std::isnan(zero2));
        packunpackf64test(t, zero, zero2);
        t.assert(std::isfinite(zero));
        t.assert(std::isfinite(zero2));
        t.assert(zero == 0.0);
        t.assert(zero2 == 0.0);
        t.reject(std::isnan(zero));
        t.reject(std::isnan(zero2));
    }

    // Randomized tests.
    for (int i = 0; i < kRandIters; i++)
    {
        f64 x;
        u8* p = (u8*)(&x);
        for (int j = 0; j < 8; j++)
            p[j] = (u8) (rand() % 256);
        if (std::isnan(x))
            continue;
        f64 y = NAN;
        t.assert(x != y);
        packunpackf64test(t, x, y);
        t.assert(x == y);
    }
}

void f64posinftest(const tTest& t)
{
    f64 inf = INFINITY;
    t.assert(inf == (f64)INFINITY);
    t.reject(std::isfinite(inf));

    f64 big = 1e99;
    f64 smal = 1.0;
    t.assert(std::isfinite(big));
    t.assert(std::isfinite(smal));

    t.assert(inf > big);
    t.assert(inf > smal);
    t.assert(big < inf);
    t.assert(smal < inf);
    t.assert(big != inf);
    t.assert(big != inf);

    t.reject(inf < big);
    t.reject(inf < smal);
    t.reject(big > inf);
    t.reject(smal > inf);
    t.reject(big == inf);
    t.reject(smal == inf);

    t.reject(inf > inf);
    t.reject(inf < inf);
    t.reject(inf != inf);
    t.assert(inf == inf);
    t.assert(inf >= inf);
    t.assert(inf <= inf);

    t.assert(inf*big == (f64)INFINITY);
    t.reject(std::isfinite(inf*big));
    t.assert(inf*4.0 == (f64)INFINITY);
    t.reject(std::isfinite(inf*4.0));
    t.assert(inf+big == (f64)INFINITY);
    t.reject(std::isfinite(inf+big));
    t.assert(inf+4.0 == (f64)INFINITY);
    t.reject(std::isfinite(inf+4.0));
    t.assert(inf-big == (f64)INFINITY);
    t.reject(std::isfinite(inf-big));
    t.assert(inf-4.0 == (f64)INFINITY);
    t.reject(std::isfinite(inf-4.0));
}

void f64neginftest(const tTest& t)
{
    f64 ninf = (f64)(-INFINITY);
    t.assert(ninf == (f64)(-INFINITY));
    t.reject(std::isfinite(ninf));

    t.assert(ninf < (f64)(INFINITY));
    t.assert(ninf <= (f64)(INFINITY));
    t.assert(ninf != (f64)(INFINITY));
    t.reject(ninf > (f64)(INFINITY));
    t.reject(ninf >= (f64)(INFINITY));
    t.reject(ninf == (f64)(INFINITY));

    f64 big = 1e99;
    f64 smal = 1.0;
    t.assert(std::isfinite(big));
    t.assert(std::isfinite(smal));

    t.assert(ninf < big);
    t.assert(ninf < smal);
    t.assert(big > ninf);
    t.assert(smal > ninf);
    t.assert(big != ninf);
    t.assert(big != ninf);

    t.reject(ninf > big);
    t.reject(ninf > smal);
    t.reject(big < ninf);
    t.reject(smal < ninf);
    t.reject(big == ninf);
    t.reject(smal == ninf);

    t.reject(ninf > ninf);
    t.reject(ninf < ninf);
    t.reject(ninf != ninf);
    t.assert(ninf == ninf);
    t.assert(ninf >= ninf);
    t.assert(ninf <= ninf);

    t.assert(ninf*big == (f64)(-INFINITY));
    t.reject(std::isfinite(ninf*big));
    t.assert(ninf*4.0 == (f64)(-INFINITY));
    t.reject(std::isfinite(ninf*4.0));
    t.assert(ninf+big == -(f64)(INFINITY));
    t.reject(std::isfinite(ninf+big));
    t.assert(ninf+4.0 == (f64)(-INFINITY));
    t.reject(std::isfinite(ninf+4.0));
    t.assert(ninf-big == (f64)(-INFINITY));
    t.reject(std::isfinite(ninf-big));
    t.assert(ninf-4.0 == (f64)(-INFINITY));
    t.reject(std::isfinite(ninf-4.0));
}

void f64nantest(const tTest& t)
{
    f64 nan = NAN;
    t.assert(std::isnan(nan));
    t.assert(std::isnan(sqrt(-1.0)));
    t.reject(std::isfinite(nan));

    t.reject(nan == nan);
    t.assert(nan != nan);  // <-- this is the only one that evaluates to true!
    t.reject(nan > nan);
    t.reject(nan < nan);
    t.reject(nan >= nan);
    t.reject(nan <= nan);
    t.reject(nan < (f64)INFINITY);
    t.reject(nan > (f64)INFINITY);
    t.reject(nan < (f64)(-INFINITY));
    t.reject(nan > (f64)(-INFINITY));

    f64 big = 1e99;
    f64 smal = 1.0;
    t.reject(std::isnan(big));
    t.reject(std::isnan(smal));

    t.assert(std::isnan(nan*big));
    t.assert(std::isnan(nan*smal));
    t.assert(std::isnan(nan*nan));
    t.assert(std::isnan(nan/big));
    t.assert(std::isnan(nan/smal));
    t.assert(std::isnan(nan/nan));
    t.assert(std::isnan(nan+big));
    t.assert(std::isnan(nan+smal));
    t.assert(std::isnan(nan+nan));
    t.assert(std::isnan(nan-big));
    t.assert(std::isnan(nan-smal));
    t.assert(std::isnan(nan-nan));
}

void stringtest(const tTest& t)
{
    string str1 = "Hi. Ryan speaking. Hope your day is going well.";
    string orig = str1;
    string str2;
    tByteWritable out;
    pack(&out, str1);
    tByteReadable in(out.getBuf());
    unpack(&in, str2);
    t.assert(str1 == orig);
    t.assert(str1 == str2);
}

void supertest(const tTest& t)
{
    vector<tMyPackable> packableVector1(2000);
    for (size_t i = 0; i < packableVector1.size(); i++)
        packableVector1[i].randomize();
    vector<tMyPackable> orig = packableVector1;
    vector<tMyPackable> packableVector2;

    tByteWritable out;
    pack(&out, packableVector1);

    tByteReadable in(out.getBuf());
    unpack(&in, packableVector2);

    t.assert(packableVector1.size() == orig.size());
    t.assert(packableVector1.size() == packableVector2.size());

    for (size_t i = 0; i < packableVector1.size(); i++)
    {
        t.assert(packableVector1[i] == orig[i]);
        t.assert(packableVector1[i] == packableVector2[i]);
    }
}

void vector_u8_emptytest(const tTest& t)
{
    vector<u8> buf;

    tByteWritable bw;

    rho::pack(&bw, buf);

    tByteReadable br(bw.getBuf());
    vector<u8> buf2;
    buf2.push_back(192);
    buf2.push_back(238);

    rho::unpack(&br, buf2);

    t.assert(buf == buf2);
}

void vector_u8_test(const tTest& t)
{
    const int kBufSize = rand() % 1000;
    vector<u8> buf(kBufSize);

    tByteWritable bw;

    rho::pack(&bw, buf);

    tByteReadable br(bw.getBuf());
    vector<u8> buf2;
    buf2.push_back(192);
    buf2.push_back(238);

    rho::unpack(&br, buf2);

    t.assert(buf == buf2);
}

void vector_u8_speedtest(const tTest& t)
{
    const int kBufSize = 100000000;
    vector<u8> buf(kBufSize);

    tByteWritable bw;

    f64 start = sync::tTimer::usecTime();
    rho::pack(&bw, buf);
    f64 end = sync::tTimer::usecTime();
    cout << (end - start) / 1000000 << " seconds" << endl;

    tByteReadable br(bw.getBuf());
    vector<u8> buf2;
    buf2.push_back(192);
    buf2.push_back(238);

    start = sync::tTimer::usecTime();
    rho::unpack(&br, buf2);
    end = sync::tTimer::usecTime();
    cout << (end - start) / 1000000 << " seconds" << endl;

    t.assert(buf == buf2);
}

int main()
{
    tCrashReporter::init();
    srand((u32)time(0));

    tTest("u8 test", u8test);
    tTest("i8 test", i8test);
    tTest("u16 test", u16test);
    tTest("i16 test", i16test);
    tTest("u32 test", u32test);
    tTest("i32 test", i32test);
    tTest("u64 test", u64test);
    tTest("i64 test", i64test);

    tTest("f32 test", f32test);
    tTest("f32 pos inf test", f32posinftest);
    tTest("f32 neg inf test", f32neginftest);
    tTest("f32 nan test", f32nantest);

    tTest("f64 test", f64test);
    tTest("f64 pos inf test", f64posinftest);
    tTest("f64 neg inf test", f64neginftest);
    tTest("f64 nan test", f64nantest);

    tTest("string test", stringtest);

    tTest("super test", supertest);

    tTest("vector<u8> empty test", vector_u8_emptytest);
    tTest("vector<u8> test", vector_u8_test, 10000);
    //tTest("vector<u8> speed test", vector_u8_speedtest);

    return 0;
}
