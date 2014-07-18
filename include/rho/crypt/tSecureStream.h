#ifndef __rho_crypt_tSecureStream_h__
#define __rho_crypt_tSecureStream_h__


#include <rho/ppcheck.h>
#include <rho/refc.h>

#include <rho/iReadable.h>
#include <rho/iWritable.h>
#include <rho/iFlushable.h>
#include <rho/bNonCopyable.h>

#include <rho/crypt/tReadableAES.h>
#include <rho/crypt/tWritableAES.h>
#include <rho/crypt/tRSA.h>


namespace rho
{
namespace crypt
{


/**
 * This class sets up a secure stream over a potentially insecure
 * channel.
 *
 * The protocol used here is a slimmed-down version of SSL. It
 * is just as secure, but doesn't support a few of SSL's features,
 * such as:
 *   - client authentication
 *   - anonymous server sessions
 *   - cypher selection / switching
 *   - Diffie-Hellman Key Exchange
 *   - FORTEZZA Key Exchange
 *
 * This protocol forces an authenticated server session and key
 * exchange via RSA. The client must know the server's RSA public
 * key in advance, and the server must know its own private key
 * in advance.
 *
 * This protocol is summarized in tSecureStream.pdf (in the 'source'
 * directory of this library). There are two phases:
 *   1) the handshake phase, where secrets are exchanged
 *   2) the application data phase, runs over AES
 *
 * This protocol assumes the following:
 *   - only the server knows its private key
 *   - the "RSA problem" will remain unsolved
 *   - no one-way hash can be reversed
 *
 * Under those assumptions, this protocol provides the following
 * guarantees:
 *   - server verification:
 *        * the server proves it has the private key before
 *          the client will communicate with it further
 *   - the pre_secret, secret, and AES key are known
 *     only to the client and server:
 *        * Eve will not be able to derive them from
 *          listening to the communication channel
 *   - all man-in-the-middle attacks will be discovered
 *     during the handshake:
 *        * neither the client nor the server will continue
 *          the connection if the handshake messages
 *          are modified in any way
 *   - a false server performing a replay attack will
 *     be discovered
 *   - a false client performing a replay attack will
 *     be discovered
 *
 * After the handshake phase, AES streams are setup.
 * These streams are managed by the tReadableAES and
 * tWritableAES classes. Read about the guarantees that
 * these classes make in their header files.
 */
class tSecureStream : public iReadable, public iWritable,
                      public iFlushable, public bNonCopyable
{
    public:

        tSecureStream(iReadable* internalReadable,
                      iWritable* internalWritable,
                      const tRSA& rsa,
                      std::string appGreeting);

        ~tSecureStream();

        i32 read(u8* buffer, i32 length);
        i32 readAll(u8* buffer, i32 length);

        i32 write(const u8* buffer, i32 length);
        i32 writeAll(const u8* buffer, i32 length);

        bool flush();

    private:

        void m_setupServer(const tRSA& rsa, std::string appGreeting);
        void m_setupClient(const tRSA& rsa, std::string appGreeting);

    private:

        iReadable* m_internal_readable;
        iWritable* m_internal_writable;

        refc<tReadableAES> m_readable;
        refc<tWritableAES> m_writable;
};


}   // namespace crypt
}   // namespace rho


#endif   // __rho_crypt_tSecureStream_h__
