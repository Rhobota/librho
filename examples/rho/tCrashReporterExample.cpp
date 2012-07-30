#include <rho/tCrashReporter.h>

#include <cstdlib>
#include <ctime>
#include <iostream>


using namespace rho;
using std::cout;
using std::endl;


void unhandledExceptionExample()
{
    cout << "About to throw an unhandled exception..." << endl;
    throw std::exception();     // This exeption has no handler.
}

void unexpectedExceptionExample() throw(int)
{
    cout << "About to throw an unexpected exception..." << endl;
    throw 'a';        // Throwing 'char' when only throwing 'int' is allowed.
}

void segFaultExample()
{
    cout << "About to access bad memory..." << endl;
    int* p = NULL;
    *p = 47;         // Dereferencing NULL.
}

int main()
{
    tCrashReporter::init();

    srand(time(0));

    switch (rand() % 3)
    {
        case 0:
            unhandledExceptionExample();
            break;
        case 1:
            unexpectedExceptionExample();
            break;
        case 2:
            segFaultExample();
            break;
    }

    return 0;
}
