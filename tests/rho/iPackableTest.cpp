#include <rho/iPackable.h>
#include <rho/tTest.h>
#include <rho/tCrashReporter.h>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>

using namespace rho;
using std::cout;
using std::endl;
using std::string;
using std::vector;


class tByteOutputStream : public iOutputStream
{
    public:

        tByteOutputStream()
            : m_buf()
        {
        }

        i32 write(const u8* buf, i32 len)
        {
            return writeAll(buf, len);
        }

        i32 writeAll(const u8* buf, i32 len)
        {
            for (i32 i = 0; i < len; i++)
                m_buf.push_back(buf[i]);
            return len;
        }

        vector<u8> getBuf() const
        {
            return m_buf;
        }

    private:

        vector<u8> m_buf;
};


class tByteInputStream : public iInputStream
{
    public:

        tByteInputStream(vector<u8> inputBuf)
            : m_buf(inputBuf), m_pos(0)
        {
        }

        i32 read(u8* buf, i32 len)
        {
            return readAll(buf, len);
        }

        i32 readAll(u8* buf, i32 len)
        {
            if (m_pos >= m_buf.size())
                return -1;
            i32 i;
            for (i = 0; i < len && m_pos < m_buf.size(); i++)
                buf[i] = m_buf[m_pos++];
            return i;
        }

    private:

        vector<u8> m_buf;
        size_t m_pos;
};


class tMyPackable : public iPackable
{
    public:

        tMyPackable()
            : a(0), b(0), c(0), d(0), e(0), f(0)
        {
        }

        void pack(iOutputStream* out) const
        {
            rho::pack(out, a);
            rho::pack(out, b);
            rho::pack(out, c);
            rho::pack(out, d);
            rho::pack(out, e);
            rho::pack(out, f);
            rho::pack(out, str);
            rho::pack(out, intVector);
            rho::pack(out, stringVector);
            rho::pack(out, stringMatrix);
        }

        void unpack(iInputStream* in)
        {
            rho::unpack(in, a);
            rho::unpack(in, b);
            rho::unpack(in, c);
            rho::unpack(in, d);
            rho::unpack(in, e);
            rho::unpack(in, f);
            rho::unpack(in, str);
            rho::unpack(in, intVector);
            rho::unpack(in, stringVector);
            rho::unpack(in, stringMatrix);
        }

        void randomize()
        {
            a = rand() % (1<<8);
            b = (rand() % (1<<8)) - (1<<7);
            c = rand() % (1<<16);
            d = (rand() % (1<<16)) - (1<<15);
            e = rand() % (1LL<<32);
            f = (rand() % (1LL<<32)) - (1<<31);

            int len = rand() % 500;
            for (int i = 0; i < len; i++)
                str += (char)((rand() % 26) + 'a');

            len = rand() % 500;
            for (int i = 0; i < len; i++)
                intVector.push_back((rand() % (1<<16)) - (1<<15));

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
                        s += ((rand() % 26) + 'a');
                    vs.push_back(s);
                }
                stringMatrix.push_back(vs);
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

            if (intVector.size() != other.intVector.size()) return false;
            for (size_t i = 0; i < intVector.size(); i++)
                if (intVector[i] != other.intVector[i])
                    return false;

            if (stringVector.size() != other.stringVector.size()) return false;
            for (size_t i = 0; i < stringVector.size(); i++)
                if (stringVector[i] != other.stringVector[i])
                    return false;

            if (stringMatrix.size() != other.stringMatrix.size()) return false;
            for (size_t i = 0; i < stringMatrix.size(); i++)
            {
                if (stringMatrix[i].size() != other.stringMatrix[i].size())
                    return false;
                for (size_t j = 0; j < stringMatrix[i].size(); j++)
                    if (stringMatrix[i][j] != other.stringMatrix[i][j])
                        return false;
            }

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
        }

    private:

        u8 a;
        i8 b;
        u16 c;
        i16 d;
        u32 e;
        i32 f;

        string str;

        vector<i16> intVector;
        vector<string> stringVector;
        vector< vector<string> > stringMatrix;
};


void u8test(const tTest& t)
{
    u8 v1 = 98;    // binary 0110 0010
    u8 v2 = 148;   // binary 1001 0100
    u8 v3 = 0, v4 = 0;
    tByteOutputStream out;
    pack(&out, v1);
    pack(&out, v2);
    t.assert(v1 == 98);
    t.assert(v2 == 148);
    vector<u8> buf = out.getBuf();
    t.assert(buf.size() == 2);
    t.assert(buf[0] == 98);
    t.assert(buf[1] == 148);
    tByteInputStream in(buf);
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
    tByteOutputStream out;
    pack(&out, v1);
    pack(&out, v2);
    t.assert(v1 == 98);
    t.assert(v2 == -38);
    vector<u8> buf = out.getBuf();
    t.assert(buf.size() == 2);
    t.assert(buf[0] == 98);
    t.assert(buf[1] == 218);
    tByteInputStream in(buf);
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
    tByteOutputStream out;
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
    tByteInputStream in(buf);
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
    tByteOutputStream out;
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
    tByteInputStream in(buf);
    unpack(&in, v3);
    unpack(&in, v4);
    t.assert(v3 == 17114);
    t.assert(v4 == -15650);
}

void u32test(const tTest& t)
{
    u32 v1 = 1413227351; // binary 0101 0100 0011 1100 0010 0011 0101 0111
    u32 v2 = 2991341219; // binary 1011 0010 0100 1100 0011 1110 1010 0011
    u32 v3 = 0, v4 = 0;
    tByteOutputStream out;
    pack(&out, v1);
    pack(&out, v2);
    t.assert(v1 == 1413227351);
    t.assert(v2 == 2991341219);
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
    tByteInputStream in(buf);
    unpack(&in, v3);
    unpack(&in, v4);
    t.assert(v3 == 1413227351);
    t.assert(v4 == 2991341219);
}

void i32test(const tTest& t)
{
    i32 v1 = 1413227351;  // binary 0101 0100 0011 1100 0010 0011 0101 0111
    i32 v2 = -1303626077; // binary 1011 0010 0100 1100 0011 1110 1010 0011
    i32 v3 = 0, v4 = 0;
    tByteOutputStream out;
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
    tByteInputStream in(buf);
    unpack(&in, v3);
    unpack(&in, v4);
    t.assert(v3 == 1413227351);
    t.assert(v4 == -1303626077);
}

void stringtest(const tTest& t)
{
    string str1 = "Hi. Ryan speaking. Hope your day is going well.";
    string orig = str1;
    string str2;
    tByteOutputStream out;
    pack(&out, str1);
    tByteInputStream in(out.getBuf());
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

    tByteOutputStream out;
    pack(&out, packableVector1);

    tByteInputStream in(out.getBuf());
    unpack(&in, packableVector2);

    t.assert(packableVector1.size() == orig.size());
    t.assert(packableVector1.size() == packableVector2.size());

    for (size_t i = 0; i < packableVector1.size(); i++)
    {
        t.assert(packableVector1[i] == orig[i]);
        t.assert(packableVector1[i] == packableVector2[i]);
    }
}


int main()
{
    tCrashReporter::init();
    srand(time(0));

    tTest("u8 test", u8test);
    tTest("i8 test", i8test);
    tTest("u16 test", u16test);
    tTest("i16 test", i16test);
    tTest("u32 test", u32test);
    tTest("i32 test", i32test);

    tTest("string test", stringtest);

    tTest("super test", supertest);

    return 0;
}
