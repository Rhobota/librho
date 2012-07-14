#include "rho/tCrashReporter.h"

#include "rho/ebObject.h"
#include "rho/tStacktrace.h"

#include <signal.h>

#include <iostream>
#include <typeinfo>


namespace rho
{


static void printInFlightException()
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

static void cleanExitWithFailure()
{
    // Abort() does no cleanup at all. Exit() doesn't unwinds the stack,
    // but it *does* call the destructors of all the global objects.
    // So, exit() is better.
    ::exit(1);
}

static void terminate()
{
    std::cerr << std::endl;
    std::cerr << "Unhandled exception: ";
    printInFlightException();
    cleanExitWithFailure();
}

static void unexpected()
{
    std::cerr << std::endl;
    std::cerr << "Unexpected exception was thrown: ";
    printInFlightException();
    cleanExitWithFailure();
}

static void segmentationFault(int sig)
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
    std::set_terminate(rho::terminate);
    std::set_unexpected(rho::unexpected);
    ::signal(SIGSEGV, rho::segmentationFault);
    ::signal(SIGILL, rho::segmentationFault);
}

void tCrashReporter::terminate()
{
    rho::terminate();
}


}    // namespace rho
