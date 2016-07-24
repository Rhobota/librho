#include <rho/crypt/tRSA.h>
#include <iostream>

using namespace rho;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::vector;


int main()
{
    cout << "How many bits? " << flush;
    u32 numbits;
    cin >> numbits;
    cout << endl;

    {
        cout << "Generating..." << flush;
        tFileWritable outfile("keys.bin");
        crypt::tRSA::generate(numbits, 100, &outfile);
        cout << " DONE!" << endl;
        cout << endl;
    }

    {
        tFileReadable infile("keys.bin");
        vector<u8> bytes;

        unpack(&infile, bytes);
        cout << "n = " << algo::tBigInteger(bytes) << endl;

        unpack(&infile, bytes);
        cout << "e = " << algo::tBigInteger(bytes) << endl;

        unpack(&infile, bytes);
        cout << "d = " << algo::tBigInteger(bytes) << endl;

        unpack(&infile, bytes);
        cout << "p = " << algo::tBigInteger(bytes) << endl;

        unpack(&infile, bytes);
        cout << "q = " << algo::tBigInteger(bytes) << endl;
    }

    return 0;
}
