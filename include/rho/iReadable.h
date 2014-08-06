#ifndef __rho_iReadable_h__
#define __rho_iReadable_h__


#include <rho/ppcheck.h>
#include <rho/bNonCopyable.h>
#include <rho/types.h>
#include <rho/eRho.h>

#include <cstdio>
#include <string>
#include <vector>


namespace rho
{


class iReadable
{
    public:

        /**
         * Reads only the amount of data immediately available, unless
         * there is no data available (in which case it blocks).
         *
         * Returns the number of bytes read, or 0 when eof is reached,
         * or -1 if the stream is closed.
         *
         * A well-behaved readable will return 0 exactly once (when eof
         * is reached), then will return -1 on all subsequent calls.
         */
        virtual i32 read(u8* buffer, i32 length) = 0;

        /**
         * Reads exactly 'length' bytes unless eof is reached or
         * the stream is closed. That is, it will block until 'length'
         * bytes are available.
         *
         * Returns 'length', or less than 'length' when eof is reached,
         * or -1 if the stream is closed.
         *
         * A well-behaved readable will only return less than 'length'
         * once (when the eof is first reached), then it will return
         * -1 on all subsequent calls. (Notice this is different from
         * how read() is defined. In read(), 0 is always returned exactly
         * once, while here less-than-'length' is returned exactly once.)
         */
        virtual i32 readAll(u8* buffer, i32 length) = 0;

        virtual ~iReadable() { }
};


class tBufferedReadable : public iReadable, public bNonCopyable
{
    public:

        /**
         * Creates a buffered stream using the given stream as the source.
         * This buffered stream does not take ownership of the given stream,
         * so you must make sure that it does not get deleted until after
         * this buffered stream has been deleted. Also, you must delete
         * the given stream yourself; it is not automatically managed by
         * this object.
         */
        tBufferedReadable(iReadable* internalStream, u32 bufSize=4096);

        ~tBufferedReadable();

        i32 read(u8* buffer, i32 length);
        i32 readAll(u8* buffer, i32 length);

        iReadable* getInternalStream() { return m_stream; }

    private:

        bool m_refill();

    private:

        iReadable* m_stream;
        u8* m_buf;
        u32 m_bufSize;
        u32 m_bufUsed;
        u32 m_pos;
};


class tFileReadable : public iReadable, public bNonCopyable
{
    public:

        tFileReadable(std::string filename);

        ~tFileReadable();

        i32 read(u8* buffer, i32 length);
        i32 readAll(u8* buffer, i32 length);

        std::string getFilename() const;

    private:

        std::string m_filename;
        FILE* m_file;
        bool m_eof;
};


class tZlibReadable : public iReadable, public bNonCopyable
{
    public:

        /**
         * Reads from the 'internalStream' and inflates what
         * is read. The 'internalStream' is assumed to be a valid
         * stream of zlib formatted deflated data.
         *
         * Errors are thrown if the stream is not in the zlib format.
         */
        tZlibReadable(iReadable* internalStream);

        ~tZlibReadable();

        i32 read(u8* buffer, i32 length);
        i32 readAll(u8* buffer, i32 length);

    private:

        iReadable* m_stream;

        void* m_zlibContext;
        u8* m_inBuf;
        u8* m_outBuf;
};


class tByteReadable : public iReadable, public bNonCopyable
{
    public:

        tByteReadable()
            : m_buf(), m_pos(0), m_eof(false) { }

        tByteReadable(const std::vector<u8>& inputBuf)
            : m_buf(inputBuf), m_pos(0), m_eof(false)
        {
        }

        i32 read(u8* buffer, i32 length)
        {
            if (length <= 0)
                throw eInvalidArgument("Stream read/write length must be >0");

            if (m_pos >= m_buf.size())
                return m_eof ? -1 : ((m_eof = true), 0);
            i32 i;
            for (i = 0; i < length && m_pos < m_buf.size(); i++)
                buffer[i] = m_buf[m_pos++];
            return i;
        }

        i32 readAll(u8* buffer, i32 length)
        {
            i32 i = read(buffer, length);
            if (i < length)       // readAll() is defined to have different behavior than read(),
                m_eof = true;     // thus this extra logic here.
            return i;
        }

        void reset(const std::vector<u8>& inputBuf)
        {
            m_buf = inputBuf;
            m_pos = 0;
            m_eof = false;
        }

    private:

        std::vector<u8> m_buf;
        size_t m_pos;
        bool m_eof;
};


}     // namespace rho


#endif    // __rho_iReadable_h__
