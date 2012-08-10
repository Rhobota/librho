#include <rho/sync/tThreadLocal.h>
#include <rho/sync/tThread.h>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>


using namespace rho;
using std::cout;
using std::endl;
using std::ostringstream;


class tFoo
{
    public:

        int x;
};


/*
 * The following is how you declare a thread-local object, meaning that each
 * thread can have its own instance of the object:
 */
sync::tThreadLocal<tFoo> gMyFoo;


/*
 * And here's a thread to demonstrate it:
 */
class tRunnable : public sync::iRunnable
{
    public:

        void run()
        {
            gMyFoo = new tFoo;       // <--- Allocs a tFoo for this thread only.

            gMyFoo->x = rand() % 100;

            sync::tThread::msleep(1000);

            ostringstream o;
            o << "My thread-local value of 'x' is: " << gMyFoo->x << endl;
            cout << o.str();

            gMyFoo = NULL;    // <--- Deletes this thread's instance of tFoo.
        }
};


int main()
{
    srand(time(0));

    sync::tThread t1(new tRunnable);
    sync::tThread t2(new tRunnable);
    sync::tThread t3(new tRunnable);
    sync::tThread t4(new tRunnable);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    return 0;
}
