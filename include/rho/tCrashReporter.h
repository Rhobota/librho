#ifndef __rho_tCrashReporter_h__
#define __rho_tCrashReporter_h__


#include <rho/ppcheck.h>
#include <rho/bNonCopyable.h>


namespace rho
{


class tCrashReporter : public bNonCopyable
{
    public:

        /**
         * Call tCrashReporter::init() at the very beginning of your program
         * to setup crash reporting. This gives you stacktraces when:
         *   1. An exception is thrown and has no handler,
         *   2. An unexpected exception is thrown
         *      (see 'exception specifications'),
         *   3. A segmentation fault occurs.
         */
        static void init();

        /**
         * Call tCrashReporter::terminate() if an unrecoverable error has
         * occurred in your program and you want to print diagnostic info.
         */
        static void terminate();

    private:

        tCrashReporter();
};


}    // namespace rho


#endif   // __rho_tCrashReporter_h__
