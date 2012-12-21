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
         *
         * Reads only the amount of data immediately available, unless
         * there is no data available (in which case it blocks).
         */
        virtual i32 read(u8* buffer, i32 length) = 0;

        /**
         * Returns the number of bytes read, or 0 when eof is reached,
         * or -1 if the stream is closed.
         *
         * Will read exactly 'length' bytes unless eof is reached or
         * the stream is closed. That is, it will block until 'length'
         * bytes are available.
         */
        virtual i32 readAll(u8* buffer, i32 length) = 0;

        virtual ~iInputStream() { }
};


class tBufferedInputStream : public iInputStream
{
    public:

        tBufferedInputStream(iInputStream* internalStream, u32 bufSize=4096);

        ~tBufferedInputStream();

        i32 read(u8* buffer, i32 length);
        i32 readAll(u8* buffer, i32 length);

    private:

        bool refill();

    private:

        iInputStream* m_stream;
        u8* m_buf;
        u32 m_bufSize;
        u32 m_bufUsed;
        u32 m_pos;
};


}     // namespace rho


#endif    // __rho_iInputStream_h__
