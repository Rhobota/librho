#ifndef __rho_tCrashReporter_h__
#define __rho_tCrashReporter_h__


#include "ebObject.h"
#include "tStacktrace.h"

#include <signal.h>

#include <iostream>


namespace rho
{


/**
 * Call tCrashReporter::init() at the very beginning of your program to setup
 * crash reporting. This gives you stacktraces when:
 *   1. An exception is thrown and has no handler,
 *   2. An unexpected exception is thrown (see 'exception specifications'),
 *   3. A segmentation fault occurs.
 */
class tCrashReporter
{
    public:

        static void init();
};


void printInFlightException()
{
    try
    {
        throw;     // throws the exception that is already in-flight
    }
    catch (ebObject& e)
    {
        std::cerr << typeid(e).name() << std::endl;
        std::cerr << "Reason: " << e.reason() << std::endl;
        e.printStacktrace(std::cerr);
    }
    catch (std::exception& e)
    {
        std::cerr << typeid(e).name() << std::endl;
        std::cerr << "Reason: " << e.what() << std::endl;
        tStacktrace().print(std::cerr);
    }
    catch (...)
    {
        std::cerr << "???" << std::endl;
        tStacktrace().print(std::cerr);
    }
}

void cleanExitWithFailure()
{
    // Abort() does no cleanup at all. Exit() doesn't unwinds the stack,
    // but it *does* call the destructors of all the global objects.
    // So, exit() is better.
    exit(1);
}

void terminate()
{
    std::cerr << std::endl;
    std::cerr << "Unhandled exception: ";
    printInFlightException();
    cleanExitWithFailure();
}

void unexpected()
{
    std::cerr << std::endl;
    std::cerr << "Unexpected exception was thrown: ";
    printInFlightException();
    cleanExitWithFailure();
}

void segmentationFault(int sig)
{
    if (sig == SIGSEGV || sig == SIGILL)
    {
        std::cerr << std::endl;
        std::cerr << "Segmentation fault!" << std::endl;
        tStacktrace().print(std::cerr);
    }
    cleanExitWithFailure();
}

void tCrashReporter::init()
{
    std::set_terminate(terminate);
    std::set_unexpected(unexpected);
    signal(SIGSEGV, segmentationFault);
    signal(SIGILL, segmentationFault);
}


}    // namespace rho


#endif   // __rho_tCrashReporter_h__
