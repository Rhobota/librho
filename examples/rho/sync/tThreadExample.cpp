#include "rho/sync/tThread.h"

#include <iostream>
#include <vector>


using namespace rho;
using std::cout;
using std::endl;
using std::vector;


class tMyAwesomeRunnable : public sync::iRunnable
{
    public:

        void run()
        {
            cout << "I'm a thread!\n";
        }
};


void simpleThreadCreation()
{
    /*
     * This function will show how to do *simple* thread creation.
     *
     * This can be done whenever the number of threads you need is
     * "hardcoded", such as below where exactly two threads are always created.
     */

    /*
     * Don't worry, the heap-allocated objects here will be put into a refc's by
     * tThread and each of their ref-counts will be kept >0 until each thread
     * terminates.
     */
    sync::tThread t1(new tMyAwesomeRunnable);
    sync::tThread t2(new tMyAwesomeRunnable);

    /*
     * You don't have to join() with your threads; you are allowed to let the
     * tThread objects go out of scope without joining if you want.
     *
     * If you *do* let a tThread object go out of scope without joining,
     * the thread will become detached (meaning the OS will clean-up its record
     * of the thread whenever the thread terminates). Additionally, the
     * iRunnable object will be automatically cleaned-up after the
     * thread terminates because the refc holding it will reach zero.
     *
     * All this is to say: handle the creation of threads like shown here
     * and you'll be totally fine (no matter if you join() with them or not).
     */
    t1.join();
    t2.join();
}


void complexThreadCreation()
{
    /*
     * If you need to create a variable number of threads, do the following.
     * The following will not leak resources.
     *
     * Note: Like in simpleThreadCreation(), you do not need to join()
     *       with each thread if you don't want to. All will be well if
     *       you let the tThread objects go out of scope without joining.
     */

    int numThreads = 43;

    vector< refc<sync::tThread> > threads;

    for (int i = 0; i < numThreads; i++)
        threads.push_back(new sync::tThread(new tMyAwesomeRunnable));

    for (int i = 0; i < numThreads; i++)
        threads[i]->join();
}


int main()
{
    simpleThreadCreation();
    complexThreadCreation();

    return 0;
}
