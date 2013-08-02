#include <rho/crypt/tSecureStream.h>
#include <rho/crypt/tSecureRandom.h>
#include <rho/crypt/tMD5.h>

#include <vector>

using std::vector;
using std::string;


namespace rho
{
namespace crypt
{


tSecureStream::tSecureStream(iReadable* internalReadable,
                             iWritable* internalWritable,
                             const tRSA& rsa,
                             string appGreeting)
    : m_internal_readable(internalReadable),
      m_internal_writable(internalWritable)
{
    if (internalReadable == NULL || internalWritable == NULL)
        throw eInvalidArgument("The internal streams may not be null.");
    if (rsa.hasPrivateKey())
        m_setupServer(rsa, appGreeting);
    else
        m_setupClient(rsa, appGreeting);
}

tSecureStream::~tSecureStream()
{
    m_readable = NULL;
    m_writable = NULL;
    m_internal_readable = NULL;
    m_internal_writable = NULL;
}

i32 tSecureStream::read(u8* buffer, i32 length)
{
    return m_readable->read(buffer, length);
}

i32 tSecureStream::readAll(u8* buffer, i32 length)
{
    return m_readable->readAll(buffer, length);
}

i32 tSecureStream::write(const u8* buffer, i32 length)
{
    return m_writable->write(buffer, length);
}

i32 tSecureStream::writeAll(const u8* buffer, i32 length)
{
    return m_writable->writeAll(buffer, length);
}

bool tSecureStream::flush()
{
    return m_writable->flush();
}

static
vector<u8> s_hash(vector<u8> vect)
{
    tMD5 md5;
    md5.writeAll(&vect[0], (i32)vect.size());
    return md5.getHash();
}

static
vector<u8> s_reverse(vector<u8> vect)
{
    if (vect.size() == 0)
        return vect;
    size_t a = 0, b = vect.size()-1;
    while (a < b)
        std::swap(vect[a++], vect[b--]);
    return vect;
}

static
void s_flush(iWritable* writable)
{
    iFlushable* flushable = dynamic_cast<iFlushable*>(writable);
    if (flushable)
        if (! flushable->flush())
            throw eRuntimeError("Couldn't flush internal writable.");
}

static
nKeyLengthAES s_toKeyLen(size_t vectsize)
{
    switch (vectsize)
    {
        case 128/8: return k128bit;
        case 192/8: return k192bit;
        case 256/8: return k256bit;
        default: throw eInvalidArgument("Invalid aes key length!");
    }
}

void tSecureStream::m_setupServer(const tRSA& rsa, string appGreeting)
{
    // Greeting.
    const string librhoGreeting = "Hello librho secure server version 1.";
    string receivedLibrhoGreeting, receivedAppGreeting;
    rho::unpack(m_internal_readable, receivedLibrhoGreeting, (u32)librhoGreeting.size());
    rho::unpack(m_internal_readable, receivedAppGreeting, (u32)appGreeting.size());
    if (receivedLibrhoGreeting != librhoGreeting || receivedAppGreeting != appGreeting)
    {
        rho::pack(m_internal_writable, string("No."));
        s_flush(m_internal_writable);
        throw eRuntimeError("The secure client did not greet me properly... :(");
    }
    rho::pack(m_internal_writable, string("Hi!"));
    s_flush(m_internal_writable);

    // Read the encrypted aes key for this connection.
    vector<u8> aeskeyEncrypted;
    rho::unpack(m_internal_readable, aeskeyEncrypted, rsa.maxMessageLength()+5);

    // Decrypt the aes key.
    vector<u8> aeskey = rsa.decrypt(aeskeyEncrypted);
    if (aeskey.size() != 128/8 && aeskey.size() != 192/8 && aeskey.size() != 256/8)
        throw eRuntimeError("The secure client gave an aes key that is an invalid size.");

    // Send the signed key back to the client.
    vector<u8> signature = rsa.sign(s_hash(aeskey));
    rho::pack(m_internal_writable, signature);
    s_flush(m_internal_writable);

    // Setup secure streams with the client.
    refc<tReadableAES> secureReadable;
    refc<tWritableAES> secureWritable;
    secureReadable = new tReadableAES(m_internal_readable, kOpModeCBC,
                                      &aeskey[0], s_toKeyLen(aeskey.size()));
    secureWritable = new tWritableAES(m_internal_writable, kOpModeCBC,
                                      &aeskey[0], s_toKeyLen(aeskey.size()));

    // Let myself randomize communication with the client.
    u32 randLen = secureRand_u8() + 20;
    vector<u8> randVect(randLen);
    secureRand_readAll(&randVect[0], (i32)randLen);
    rho::pack(secureWritable, randVect);
    if (!secureWritable->flush()) throw eRuntimeError("Couldn't flush secure stream.");
    vector<u8> receivedRand;
    rho::unpack(secureReadable, receivedRand, randLen);
    if (s_reverse(receivedRand) != randVect)
        throw eRuntimeError("The secure client failed the random challenge.");

    // Let the client randomize communication with me.
    rho::unpack(secureReadable, receivedRand, 300);
    receivedRand = s_reverse(receivedRand);
    rho::pack(secureWritable, receivedRand);
    if (!secureWritable->flush()) throw eRuntimeError("Couldn't flush secure stream.");

    // All is well, so make this official!
    m_readable = secureReadable;
    m_writable = secureWritable;
}

void tSecureStream::m_setupClient(const tRSA& rsa, string appGreeting)
{
    // Greeting.
    const string librhoGreeting = "Hello librho secure server version 1.";
    rho::pack(m_internal_writable, librhoGreeting);
    rho::pack(m_internal_writable, appGreeting);
    s_flush(m_internal_writable);

    // Read that the server liked the greeting.
    string greetingResponse;
    rho::unpack(m_internal_readable, greetingResponse, 100);
    if (greetingResponse != "Hi!")
    {
        throw eRuntimeError(string() +
                "Didn't get proper greeting from the secure server. Received: " +
                greetingResponse);
    }

    // Create an aes key for this connection.
    vector<u8> aeskey(192/8, 0);
    secureRand_readAll(&aeskey[0], (i32)aeskey.size());

    // Encrypt the aes key and send to the server.
    vector<u8> aeskeyEncrypted = rsa.encrypt(aeskey);
    rho::pack(m_internal_writable, aeskeyEncrypted);
    s_flush(m_internal_writable);

    // Read the signed hash back from the server.
    vector<u8> signature;
    rho::unpack(m_internal_readable, signature, rsa.maxMessageLength()+5);
    if (! rsa.verify(s_hash(aeskey), signature))
        throw eRuntimeError("The secure server could not verify itself.");

    // Setup secure streams with the server.
    refc<tReadableAES> secureReadable;
    refc<tWritableAES> secureWritable;
    secureReadable = new tReadableAES(m_internal_readable, kOpModeCBC,
                                      &aeskey[0], s_toKeyLen(aeskey.size()));
    secureWritable = new tWritableAES(m_internal_writable, kOpModeCBC,
                                      &aeskey[0], s_toKeyLen(aeskey.size()));

    // Let the server randomize communication with me.
    vector<u8> receivedRand;
    rho::unpack(secureReadable, receivedRand, 300);
    receivedRand = s_reverse(receivedRand);
    rho::pack(secureWritable, receivedRand);
    if (!secureWritable->flush()) throw eRuntimeError("Couldn't flush secure stream.");

    // Let myself randomize communication with the server.
    u32 randLen = secureRand_u8() + 20;
    vector<u8> randVect(randLen);
    secureRand_readAll(&randVect[0], (i32)randLen);
    rho::pack(secureWritable, randVect);
    if (!secureWritable->flush()) throw eRuntimeError("Couldn't flush secure stream.");
    rho::unpack(secureReadable, receivedRand, randLen);
    if (s_reverse(receivedRand) != randVect)
        throw eRuntimeError("The secure server failed the random challenge.");

    // All is well, so make this official!
    m_readable = secureReadable;
    m_writable = secureWritable;
}


}   // namespace crypt
}   // namespace rho
