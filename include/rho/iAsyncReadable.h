#ifndef __rho_iAsyncReadable_h__
#define __rho_iAsyncReadable_h__


#include <rho/types.h>


namespace rho
{


/**
 * This represents an asynchronous readable object. This is like
 * an iReadable object turned up-side-down.
 *
 * With an iReadable object, you call read() at the top of a chain
 * of iReadables, and the call gets forwarded down the chain to the
 * bottom where it may block until bytes are available.
 *
 * With an iAsyncReadable object, when bytes are available they
 * are pushed from the bottom of the iAsyncReadable chain up
 * through the chain to the top.
 */
class iAsyncReadable
{
    public:

        /**
         * Injects input into this asynchronous readable object.
         *
         * This object should consume all the input, either by
         * processing it or by storing it for later processing.
         * Do not store the 'buffer' pointer itself, as it may
         * become stale after this method returns.
         *
         * This method might only process part of the data, and
         * pass the remaining (or transformed) data up the chain
         * to the next iAsyncReadable. This allows protocols
         * to be layered, where each layer is handled by one
         * iAsyncReadable object.
         */
        virtual void takeInput(const u8* buffer, i32 length) = 0;

        /**
         * This method is called to indicate the end of stream (EOS).
         *
         * This method will only be called once. No more calls to
         * takeInput() will happen after this call.
         *
         * If this object is part of a chain, you should forward this
         * call up the chain.
         */
        virtual void endStream() = 0;

        virtual ~iAsyncReadable() { }
};


/**
 * This is the asynchronous version of the tZlibReadable. See that
 * class for info.
 */
class tZlibAsyncReadable : public iAsyncReadable, public bNonCopyable
{
    public:

        tZlibAsyncReadable(iAsyncReadable* nextReadable);

        ~tZlibAsyncReadable();

        void takeInput(const u8* buffer, i32 length);
        void endStream();

    private:

        iAsyncReadable* m_nextReadable;

        void* m_zlibContext;
        u8* m_inBuf;
        u8* m_outBuf;
        u32 m_outUsed;
        u32 m_outPos;
        bool m_eof;
};


}   // namespace rho


#endif // __rho_iAsyncReadable_h__
