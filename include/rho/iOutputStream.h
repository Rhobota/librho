#ifndef __rho_iOutputStream_h__
#define __rho_iOutputStream_h__


#include <rho/types.h>


namespace rho
{


class iOutputStream
{
    public:

        /**
         * Writes from 'buffer' up to 'length' number of bytes.
         * Returns the number of bytes actually written,
         * or -1 if the stream is closed.
         */
        virtual int write(const u8* buffer, int length) = 0;

        virtual ~iOutputStream() { }
};


}    // namespace rho


#endif    // __rho_iOutputStream_h__
