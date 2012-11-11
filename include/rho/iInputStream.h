#ifndef __rho_iInputStream_h__
#define __rho_iInputStream_h__


#include <rho/types.h>


namespace rho
{


class iInputStream
{
    public:

        /**
         * Returns the number of bytes read, or 0 when eof is reached,
         * or -1 if the stream is closed.
         */
        virtual int read(u8* buffer, int length) = 0;

        virtual ~iInputStream() { }
};


}     // namespace rho


#endif    // __rho_iInputStream_h__
