#ifndef __rho_iReadable_h__
#define __rho_iReadable_h__


#include <rho/types.h>

#include <cstdio>
#include <string>
#include <vector>


namespace rho
{


class iReadable
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
         * Returns 'length', or less than 'length' when eof is reached,
         * or -1 if the stream is closed.
         *
         * Will read exactly 'length' bytes unless eof is reached or
         * the stream is closed. That is, it will block until 'length'
         * bytes are available.
         */
        virtual i32 readAll(u8* buffer, i32 length) = 0;

        virtual ~iReadable() { }
};


class tBufferedReadable : public iReadable
{
    public:

        tBufferedReadable(iReadable* internalStream, u32 bufSize=4096);

        ~tBufferedReadable();

        i32 read(u8* buffer, i32 length);
        i32 readAll(u8* buffer, i32 length);

    private:

        bool refill();

    private:

        iReadable* m_stream;
        u8* m_buf;
        u32 m_bufSize;
        u32 m_bufUsed;
        u32 m_pos;
};


class tFileReadable : public iReadable
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


class tByteReadable : public iReadable
{
    public:

        tByteReadable(std::vector<u8> inputBuf)
            : m_buf(inputBuf), m_pos(0), m_eof(false)
        {
        }

        i32 read(u8* buffer, i32 length)
        {
            return readAll(buffer, length);
        }

        i32 readAll(u8* buffer, i32 length)
        {
            if (m_pos >= m_buf.size())
                return m_eof ? -1 : ((m_eof = true), 0);
            i32 i;
            for (i = 0; i < length && m_pos < m_buf.size(); i++)
                buffer[i] = m_buf[m_pos++];
            return i;
        }

    private:

        std::vector<u8> m_buf;
        size_t m_pos;
        bool m_eof;
};


}     // namespace rho


#endif    // __rho_iReadable_h__
