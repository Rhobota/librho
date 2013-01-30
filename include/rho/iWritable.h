#ifndef __rho_iWritable_h__
#define __rho_iWritable_h__


#include <rho/iFlushable.h>
#include <rho/types.h>


namespace rho
{


class iWritable
{
    public:

        /**
         * Writes from 'buffer' up to 'length' number of bytes.
         * Returns the number of bytes actually written,
         * or -1 if the stream is closed.
         *
         * Writes only what will immediately fit into the output
         * stream's buffer, but blocks if no bytes can be immediately
         * written.
         */
        virtual i32 write(const u8* buffer, i32 length) = 0;

        /**
         * Writes from 'buffer' up to 'length' number of bytes.
         * Returns the number of bytes actually written,
         * or -1 if the stream is closed.
         *
         * Will write exactly 'length' bytes to the stream,
         * unless eof is reached or the stream is closed.
         * That is, it will block until 'length' bytes can
         * be written.
         */
        virtual i32 writeAll(const u8* buffer, i32 length) = 0;

        virtual ~iWritable() { }
};


class tBufferedWritable : public iWritable, public iFlushable
{
    public:

        tBufferedWritable(iWritable* internalStream, u32 bufSize=4096);

        ~tBufferedWritable();

        i32 write(const u8* buffer, i32 length);
        i32 writeAll(const u8* buffer, i32 length);

        void flush();

    private:

        iWritable* m_stream;
        u8* m_buf;
        u32 m_bufSize;
        u32 m_bufUsed;
};


}    // namespace rho


#endif    // __rho_iWritable_h__
