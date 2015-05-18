#ifndef __rho_iWritable_h__
#define __rho_iWritable_h__


#include <rho/ppcheck.h>
#include <rho/bNonCopyable.h>
#include <rho/iFlushable.h>
#include <rho/iClosable.h>
#include <rho/types.h>
#include <rho/eRho.h>

#include <cstdio>
#include <string>
#include <vector>


namespace rho
{


class iWritable
{
    public:

        /**
         * Writes up to 'length' number of bytes from 'buffer'
         * to the output stream.
         *
         * Writes only what will immediately fit into the output
         * stream's buffer unless no bytes will fit, in which
         * case this method blocks.
         *
         * Returns the number of bytes actually written,
         * or 0 if an error occurred and nothing was written.
         *
         * A well-behaved writable will return 0 on all subsequent
         * calls after it has returned 0 once.
         */
        virtual i32 write(const u8* buffer, i32 length) = 0;

        /**
         * Writes up to 'length' number of bytes from 'buffer'
         * to the output stream.
         *
         * Blocks until all of the 'length' bytes can be written.
         *
         * Returns 'length' if all bytes were written, or less
         * then 'length' if an error occurred while writing.
         *
         * A well-behaved writable will return 0 on all subsequent
         * calls after it has returned less-than-length once.
         */
        virtual i32 writeAll(const u8* buffer, i32 length) = 0;

        virtual ~iWritable() { }
};


class tBufferedWritable : public iWritable, public iFlushable,
                          public bNonCopyable
{
    public:

        /**
         * This buffered writable does not own the internal stream.
         * You must ensure the internal stream does not get deleted
         * while this buffered writable is alive. Also, you are
         * responsible for deleting the internal stream after this
         * writer is gone.
         */
        tBufferedWritable(iWritable* internalStream, u32 bufSize=4096);

        ~tBufferedWritable();

        i32 write(const u8* buffer, i32 length);
        i32 writeAll(const u8* buffer, i32 length);

        bool flush();

    private:

        iWritable* m_stream;
        u8* m_buf;
        u32 m_bufSize;
        u32 m_bufUsed;
};


class tFileWritable : public iWritable, public iFlushable,
                      public bNonCopyable
{
    public:

        tFileWritable(std::string filename);

        ~tFileWritable();

        i32 write(const u8* buffer, i32 length);
        i32 writeAll(const u8* buffer, i32 length);

        bool flush();

        std::string getFilename() const;

    private:

        std::string m_filename;
        FILE* m_file;
        bool m_writeEOF;
};


class tZlibWritable : public iWritable, public iFlushable, public iClosable,
                      public bNonCopyable
{
    public:

        /**
         * Writes to the 'internalStream' deflated data in the
         * zlib format.
         *
         * The compression level is in [0,9], just like you'd expect
         * with a zlib deflate stream.
         *
         * Warning: This class will not write the STREAM_END until it
         * is destructed. If you have a tZlibReadable which is waiting
         * to receive STREAM_END, you will need to allow this class to
         * destruct.
         */
        tZlibWritable(iWritable* internalStream, int compressionLevel=6);

        ~tZlibWritable();

        i32 write(const u8* buffer, i32 length);
        i32 writeAll(const u8* buffer, i32 length);

        bool flush();

        void close();

    private:

        iWritable* m_stream;

        void* m_zlibContext;
        u8* m_inBuf;
        u8* m_outBuf;
        bool m_broken;
        u32 m_inBufPos;
};


class tByteWritable : public iWritable, public bNonCopyable
{
    public:

        tByteWritable()
            : m_buf()
        {
        }

        i32 write(const u8* buffer, i32 length)
        {
            return writeAll(buffer, length);
        }

        i32 writeAll(const u8* buffer, i32 length)
        {
            if (length <= 0)
                throw eInvalidArgument("Stream read/write length must be >0");
            m_buf.insert(m_buf.end(), buffer, buffer+length);
            return length;
        }

        const std::vector<u8>& getBuf() const
        {
            return m_buf;
        }

        void reset()
        {
            m_buf.clear();
        }

    private:

        std::vector<u8> m_buf;
};


}    // namespace rho


#endif    // __rho_iWritable_h__
