#ifndef __rho_sync_tStreamPCQ_h__
#define __rho_sync_tStreamPCQ_h__


#include <rho/ppcheck.h>
#include <rho/refc.h>
#include <rho/iReadable.h>
#include <rho/iWritable.h>
#include <rho/iClosable.h>
#include <rho/bNonCopyable.h>
#include <rho/sync/tPCQ.h>


namespace rho
{
namespace sync
{


/**
 * This is a stream that is backed by a producer-consumer queue (PCQ). Use
 * this class when you want to treat a PCQ object as a stream, either for
 * reading or for writing or for both.
 *
 * This is especially useful if you want to create a stream for communicating
 * across threads. You can do something like this:
 *
 *     refc< sync::tPCQ<std::pair<u8*, u32> > > pcq(new sync::tPCQ<std::pair<u8*, u32> >(8, sync::kGrow));
 *     refc<iReadable> loopbackReadable(new sync::tStreamPCQ(pcq));
 *     refc<iWritable> loopbackWritable(new sync::tStreamPCQ(pcq));
 *
 * Then pass loopbackReadable to one thread and loopbackWritable to a second
 * thread. Note that written this way, the underlying PCQ object will grow
 * as needed (potentially very large), so consider limiting it by passing
 * sync::kBlock to the PCQ instead.
 */
class tStreamPCQ : public iReadable, public iWritable, public iClosable, public bNonCopyable
{
    public:

        tStreamPCQ(refc< tPCQ< std::pair<u8*, u32> > > pcq)
            : m_pcq(pcq),
              m_curr(NULL, 0),
              m_off(0),
              m_eof(false)
        {
        }

        ~tStreamPCQ()
        {
            delete [] m_curr.first;
            m_curr.first = NULL;
            m_curr.second = 0;
            m_off = 0;

            while (m_pcq->size() > 0)
            {
                m_curr = m_pcq->pop();
                delete [] m_curr.first;
                m_curr.first = NULL;
                m_curr.second = 0;
            }

            m_pcq = NULL;
        }

        i32 read(u8* buffer, i32 length)
        {
            if (length <= 0)
                throw eInvalidArgument("Stream read/write length must be >0");

            if (m_eof)
                return -1;

            if (m_curr.first == NULL || m_off >= m_curr.second)
            {
                delete [] m_curr.first;
                m_curr = m_pcq->pop();
                m_off = 0;
                if (m_curr.second <= 0)
                {
                    m_curr.second = 0;
                    m_eof = true;
                }
            }

            u32 rem = m_curr.second - m_off;
            if (rem > (u32)length)
                rem = (u32)length;

            memcpy(buffer, m_curr.first+m_off, rem);
            m_off += rem;

            return (i32)rem;
        }

        i32 readAll(u8* buffer, i32 length)
        {
            if (length <= 0)
                throw eInvalidArgument("Stream read/write length must be >0");

            i32 amountRead = 0;
            while (amountRead < length)
            {
                i32 n = read(buffer+amountRead, length-amountRead);
                if (n <= 0)
                    return (amountRead>0) ? amountRead : n;
                amountRead += n;
            }
            return amountRead;
        }

        i32 write(const u8* buffer, i32 length)
        {
            if (length <= 0)
                throw eInvalidArgument("Stream read/write length must be >0");

            u8* cbuf = new u8[length];
            for (i32 i = 0; i < length; i++)
                cbuf[i] = buffer[i];
            m_pcq->push(std::make_pair(cbuf, length));
            return length;
        }

        i32 writeAll(const u8* buffer, i32 length)
        {
            if (length <= 0)
                throw eInvalidArgument("Stream read/write length must be >0");

            i32 amountWritten = 0;
            while (amountWritten < length)
            {
                i32 n = write(buffer+amountWritten, length-amountWritten);
                if (n <= 0)
                    return (amountWritten>0) ? amountWritten : n;
                amountWritten += n;
            }
            return amountWritten;
        }

        void close()
        {
            u8* buf = NULL;
            u32 len = 0;      // <-- signals the end of file (eof)
            m_pcq->push(std::make_pair(buf, len));
        }

    private:

        refc< tPCQ< std::pair<u8*, u32> > > m_pcq;
        std::pair<u8*, u32> m_curr;
        u32 m_off;
        bool m_eof;
};


}  // namespace sync
}  // namespace rho


#endif  // __rho_sync_tStreamPCQ_h__
