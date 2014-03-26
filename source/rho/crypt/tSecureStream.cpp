#include <rho/crypt/tSecureStream.h>
#include <rho/crypt/tSecureRandom.h>
#include <rho/crypt/hash_utils.h>
#include <rho/crypt/tMD5.h>
#include <rho/crypt/tSHA1.h>

#include <vector>

using std::vector;
using std::string;


namespace rho
{
namespace crypt
{


// 'greeting' in the diagram
static const string kLibrhoGreeting     = "\x10\x55\xa9\x8b\xd3\xa3\x9f\x5b\xbb\xd9\x2b\x7c\x7a\x61\x5d\x49";

// 'greeting_reply' in the diagram
static const string kSuccessfulGreeting = "\xd2\x56\xf6\xd8\xb7\xb4\x82\x36\x20\x7b\xbe\x95\x81\x14\x12";
static const string kFailedGreeting     = "\x73\xf7\xd1\x79\x34\x46\xb2\xd7\xf2\xca\x05\x64\x1b\xea";

// 'x' in the diagram
static const u32 kRandVectLen  = 16;     // <-- in bytes

// 'y' in the diagram
static const u32 kPreSecretLen = 48;     // <-- in bytes

// 'aes_key_len' in the diagram
static const u32 kAESKeyLen    = 32;     // <-- in bytes

// 'pad2' in the diagram
static const string kPad2 = "\x38\x4c\x33\x04\x8b\x71\xba\x83\xd2\xec\x6e\x42\x03\x42\xc7\xbb\xbc\x85\x9c\x31\xba\xe3\x02\x64\x39\xef";

// 'pad3' in the diagram
static const string kPad3 = "\x34\xc7\x6a\xe5\xa4\x55\x55\xff\x9f\xf5\x27\xc9\x84\x10\xa6\x98\x4\xf3";

// 'salt4' in the diagram
static const string kSalt4 = "\xa7\x59\xe0\xf1\x13\xd5\x34\x15\x56\x3a\xc6\x2d\x0b\x2a\x3a\x47";

// 'salt5' in the diagram
static const string kSalt5 = "\xeb\x0f\xbb\x9d\xb9\x4c\xdd\xf3\x73\x7b\xac\x14\x92\x31\x55";

// 'h4_iters' in the diagram
static const u32 kH4Iters = 1000;

// 'h5_iters' in the diagram
static const u32 kH5Iters = 1000;


static
vector<u8> H1(const vector<u8>& ps,
              const vector<u8>& rc,
              const vector<u8>& rs)
{
    vector<u8> ret;

    const char*[3] kPads = { "A", "BB", "CCC" };

    for (i32 i = 0; i < 3; i++)
    {
        tSHA1 s;
        s.write(kPads[i], i+1);
        s.write(&ps[0], ps.length());
        s.write(&rc[0], rc.length());
        s.write(&rs[0], rs.length());
        vector<u8> sHash = s.getHash();

        tMD5 m;
        m.write(&ps[0], ps.length());
        m.write(&sHash[0], sHash.length());
        vector<u8> mHash = m.getHash();
        ret.insert(ret.end(), mHash.begin(), mHash.end());
    }

    return ret;
}


static
vector<u8> H2(const vector<u8>& s,
              const vector<u8>& rc,
              const vector<u8>& rs)
{
    vector<u8> message;
    message.insert(message.end(), rc.begin(), rc.end());
    message.insert(message.end(), rs.begin(), rs.end());
    message.insert(message.end(), kPad2.begin(), kPad2.end());

    vector<u8> ret;

    vector<u8> a = hmac_md5(s, message);
    ret.insert(ret.end(), a.begin(), a.end());

    vector<u8> b = hmac_sha1(s, message);
    ret.insert(ret.end(), b.begin(), b.end());

    vector<u8> c = hmac_whirlpool(s, message);
    ret.insert(ret.end(), c.begin(), c.end());

    return ret;
}


static
vector<u8> H3(const vector<u8>& s,
              const vector<u8>& rc,
              const vector<u8>& rs)
{
    vector<u8> message;
    message.insert(message.end(), rs.begin(), rs.end());
    message.insert(message.end(), rc.begin(), rc.end());
    message.insert(message.end(), kPad3.begin(), kPad3.end());

    vector<u8> ret;

    vector<u8> a = hmac_md5(s, message);
    ret.insert(ret.end(), a.begin(), a.end());

    vector<u8> b = hmac_sha1(s, message);
    ret.insert(ret.end(), b.begin(), b.end());

    vector<u8> c = hmac_whirlpool(s, message);
    ret.insert(ret.end(), c.begin(), c.end());

    return ret;
}


static
vector<u8> H4(const vector<u8>& s,
              const vector<u8>& rc,
              const vector<u8>& rs)
{
    return pbkdf2(hmac_whirlpool,
                  s,
                  vector<u8>(kSalt4.begin(), kSalt4.end()),
                  kH4Iters,
                  kAESKeyLen);
}


static
vector<u8> H5(const vector<u8>& s,
              const vector<u8>& rc,
              const vector<u8>& rs)
{
    return pbkdf2(hmac_whirlpool,
                  s,
                  vector<u8>(kSalt5.begin(), kSalt5.end()),
                  kH5Iters,
                  kAESKeyLen);
}


static
vector<u8> s_genRand(u32 numbytes)
{
    vector<u8> vect(numbytes, 0);
    secureRand_readAll(&vect[0], vect.size());
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

void tSecureStream::m_setupServer(const tRSA& rsa, string appGreeting)
{
    // Read the greeting from the client.
    string receivedLibrhoGreeting;
    rho::unpack(m_internal_readable, receivedLibrhoGreeting, (u32)kLibrhoGreeting.size());
    if (receivedLibrhoGreeting != kLibrhoGreeting)
    {
        rho::pack(m_internal_writable, string(kFailedGreeting));
        s_flush(m_internal_writable);
        throw eRuntimeError("The secure client did not greet me properly... :(");
    }
    string receivedAppGreeting;
    rho::unpack(m_internal_readable, receivedAppGreeting, (u32)appGreeting.size());
    if (receivedAppGreeting != appGreeting)
    {
        rho::pack(m_internal_writable, string(kFailedGreeting));
        s_flush(m_internal_writable);
        throw eRuntimeError(string("The secure client requested different application: ") + receivedAppGreeting);
    }

    // Read the client's random bytes.
    vector<u8> randClient;
    rho::unpack(m_internal_readable, randClient, kRandMessageLength);
    if (randClient.size() != kRandMessageLength)
    {
        throw eRuntimeError("Didn't get proper rand byte length from the secure server.");
    }

    // Read the encrypted secret from the client, decrypt it, and make sure it looks okay.
    vector<u8> encryptedSecret;
    rho::unpack(m_internal_readable, encryptedSecret, rsa.maxMessageLength()+5);
    vector<u8> secret = rsa.decrypt(encryptedSecret);
    if (secret.size() != kLengthOfSecret)
        throw eRuntimeError("The secure client gave a secret that is an invalid size.");

    // Write my greeting back to the client.
    rho::pack(m_internal_writable, kSuccessfulGreeting);

    // Generate the server's random bytes and send to the client.
    vector<u8> randServer(kRandMessageLength, 0);
    secureRand_readAll(&randServer[0], kRandMessageLength);
    rho::pack(m_internal_writable, randServer);

    // Calulate H1 and send it to the client.
    vector<u8> correctH1 = s_genVerificationHash(randClient, randServer, kPad1, secret);
    rho::pack(m_internal_writable, correctH1);

    // Flush all this to the client.
    s_flush(m_internal_writable);

    // Ensure that the client actually knows the secret (aka, ensure this is not a replay attack).
    vector<u8> correctH2 = s_genVerificationHash(randServer, randClient, kPad2, secret);
    vector<u8> receivedH2;
    rho::unpack(m_internal_readable, receivedH2, (u32)correctH2.size());
    if (receivedH2 != correctH2)
    {
        throw eRuntimeError("The client failed to verify itself.");
    }

    // Setup secure streams with the client.
    vector<u8> aeskey = s_genAESKey(secret);
    refc<tReadableAES> secureReadable;
    refc<tWritableAES> secureWritable;
    secureReadable = new tReadableAES(m_internal_readable, kOpModeCBC,
                                      &aeskey[0], s_toKeyLen(aeskey.size()));
    secureWritable = new tWritableAES(m_internal_writable, kOpModeCBC,
                                      &aeskey[0], s_toKeyLen(aeskey.size()));

    // All is well, so make this official!
    m_readable = secureReadable;
    m_writable = secureWritable;
}

void tSecureStream::m_setupClient(const tRSA& rsa, string appGreeting)
{
    // Send greeting.
    rho::pack(m_internal_writable, kLibrhoGreeting);
    rho::pack(m_internal_writable, appGreeting);

    // Generate and send the client random bytes.
    vector<u8> randClient(kRandMessageLength, 0);
    secureRand_readAll(&randClient[0], kRandMessageLength);
    rho::pack(m_internal_writable, randClient);

    // Generate, encrypt, and send the shared secret.
    vector<u8> secret(kLengthOfSecret, 0);
    secureRand_readAll(&secret[0], kLengthOfSecret);
    vector<u8> encryptedSecret = rsa.encrypt(secret);
    rho::pack(m_internal_writable, encryptedSecret);

    // Send all this to the server.
    s_flush(m_internal_writable);

    // Read the greeting from the server.
    string greetingResponse;
    rho::unpack(m_internal_readable, greetingResponse,
                (u32)(std::max(kSuccessfulGreeting.length(), kFailedGreeting.length())));
    if (greetingResponse != kSuccessfulGreeting)
    {
        throw eRuntimeError(string() +
                "Didn't get proper greeting from the secure server. Received: " +
                greetingResponse);
    }

    // Read the random server bytes.
    vector<u8> randServer;
    rho::unpack(m_internal_readable, randServer, kRandMessageLength);
    if (randServer.size() != kRandMessageLength)
    {
        throw eRuntimeError("Didn't get proper rand byte length from the secure server.");
    }

    // Read the H1 from the server.
    vector<u8> correctH1 = s_genVerificationHash(randClient, randServer, kPad1, secret);
    vector<u8> receivedH1;
    rho::unpack(m_internal_readable, receivedH1, (u32)correctH1.size());
    if (correctH1 != receivedH1)
    {
        throw eRuntimeError("The server failed to verify itself.");
    }

    // Generate and send H2 to the server.
    vector<u8> correctH2 = s_genVerificationHash(randServer, randClient, kPad2, secret);
    rho::pack(m_internal_writable, correctH2);
    s_flush(m_internal_writable);

    // Setup secure streams with the server.
    vector<u8> aeskey = s_genAESKey(secret);
    refc<tReadableAES> secureReadable;
    refc<tWritableAES> secureWritable;
    secureReadable = new tReadableAES(m_internal_readable, kOpModeCBC,
                                      &aeskey[0], s_toKeyLen(aeskey.size()));
    secureWritable = new tWritableAES(m_internal_writable, kOpModeCBC,
                                      &aeskey[0], s_toKeyLen(aeskey.size()));

    // All is well, so make this official!
    m_readable = secureReadable;
    m_writable = secureWritable;
}


}   // namespace crypt
}   // namespace rho
