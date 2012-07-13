#include "rho/tStacktrace.h"

#include <sstream>


/*
 * This is just a compilation test.
 *
 * This class can't be tested very well in an automated way.
 * To see it in action, see the example in the examples/ directory.
 */


int main()
{
    const rho::tStacktrace s;

    std::ostringstream o;

    s.print(o);

    return 0;
}
