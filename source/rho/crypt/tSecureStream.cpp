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
static const u32 kH4Iters = 100;

// 'h5_iters' in the diagram
static const u32 kH5Iters = 100;


static
vector<u8> H1(const vector<u8>& ps,
              const vector<u8>& rc,
              const vector<u8>& rs)
{
    vector<u8> ret;

    const char* const kPads[3] = { "A", "BB", "CCC" };

    for (i32 i = 0; i < 3; i++)
    {
        tSHA1 s;
        s.write((const u8*)kPads[i], i+1);
        s.write(&ps[0], (i32)ps.size());
        s.write(&rc[0], (i32)rc.size());
        s.write(&rs[0], (i32)rs.size());
        vector<u8> sHash = s.getHash();

        tMD5 m;
        m.write(&ps[0], (i32)ps.size());
        m.write(&sHash[0], (i32)sHash.size());
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
vector<u8> H4(const vector<u8>& ps,
              const vector<u8>& s,
              const vector<u8>& rc,
              const vector<u8>& rs)
{
    vector<u8> pass = ps;
    pass.insert(pass.end(), s.begin(), s.end());
    pass.insert(pass.end(), rc.begin(), rc.end());
    pass.insert(pass.end(), rs.begin(), rs.end());

    return pbkdf2(hmac_whirlpool,
                  pass,
                  vector<u8>(kSalt4.begin(), kSalt4.end()),
                  kH4Iters,
                  kAESKeyLen);
}


static
vector<u8> H5(const vector<u8>& ps,
              const vector<u8>& s,
              const vector<u8>& rc,
              const vector<u8>& rs)
{
    vector<u8> pass = ps;
    pass.insert(pass.end(), s.begin(), s.end());
    pass.insert(pass.end(), rs.begin(), rs.end());
    pass.insert(pass.end(), rc.begin(), rc.end());

    return pbkdf2(hmac_whirlpool,
                  pass,
                  vector<u8>(kSalt5.begin(), kSalt5.end()),
                  kH5Iters,
                  kAESKeyLen);
}


static
vector<u8> s_genRand(u32 numbytes)
{
    vector<u8> vect(numbytes, 0);
    secureRand_readAll(&vect[0], (i32)vect.size());
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

static
void s_failConnection(iWritable* writable, string reason)
{
    pack(writable, kFailedGreeting);
    s_flush(writable);
    throw eRuntimeError(reason);
}

void tSecureStream::m_setupServer(const tRSA& rsa, string appGreeting)
{
    // Read the greeting (part 1) from the client.
    string receivedLibrhoGreeting;
    try {
        unpack(m_internal_readable, receivedLibrhoGreeting, (u32)kLibrhoGreeting.size());
    } catch (ebObject& e) {
        s_failConnection(m_internal_writable, "The secure client did not greet me properly.");
    }
    if (receivedLibrhoGreeting != kLibrhoGreeting)
        s_failConnection(m_internal_writable, "The secure client did not greet me properly.");

    // Read the greeting (part 2) from the client.
    string receivedAppGreeting;
    try {
        unpack(m_internal_readable, receivedAppGreeting, (u32)appGreeting.size());
    } catch (ebObject& e) {
        s_failConnection(m_internal_writable, "The secure client requested a different application.");
    }
    if (receivedAppGreeting != appGreeting)
        s_failConnection(m_internal_writable, "The secure client requested a different application.");

    // Read the client's random bytes.
    vector<u8> rand_c;
    try {
        unpack(m_internal_readable, rand_c, kRandVectLen);
    } catch (ebObject& e) {
        s_failConnection(m_internal_writable, "The secure client sent a random byte vector of the wrong length.");
    }
    if (rand_c.size() != kRandVectLen)
        s_failConnection(m_internal_writable, "The secure client sent a random byte vector of the wrong length.");

    // Read the encrypted pre-secret from the client, decrypt it, and make sure it looks okay.
    vector<u8> pre_secret;
    try {
        vector<u8> enc;
        unpack(m_internal_readable, enc, rsa.maxMessageLength()+5);
        pre_secret = rsa.decrypt(enc);
    } catch (ebObject& e) {
        s_failConnection(m_internal_writable, "The secure client failed to send an encrypted pre-secret.");
    }
    if (pre_secret.size() != kPreSecretLen)
        s_failConnection(m_internal_writable, "The secure client gave a pre-secret that is the wrong length.");

    // Generate the server random byte vector.
    vector<u8> rand_s = s_genRand(kRandVectLen);

    // Calculated the shared secret (from the pre-secret).
    vector<u8> secret = H1(pre_secret, rand_c, rand_s);

    // Calculate the proof-of-server hash. (The convinces the client that we are the actual server.)
    vector<u8> f = H2(secret, rand_c, rand_s);

    // Write back to the client all this stuff.
    pack(m_internal_writable, kSuccessfulGreeting);
    pack(m_internal_writable, rand_s);
    pack(m_internal_writable, f);
    s_flush(m_internal_writable);

    // Have the client prove that it is a real client, not a reply attack.
    vector<u8> gPrime = H3(secret, rand_c, rand_s);
    vector<u8> g;
    try {
        unpack(m_internal_readable, g, (u32)gPrime.size());
    } catch (ebObject& e) {
        throw eRuntimeError("The secure client failed to show proof that it is real.");
    }
    if (g != gPrime)
        throw eRuntimeError("The secure client failed to show proof that it is real.");

    // Setup secure streams with the client.
    vector<u8> ksw = H4(secret, rand_c, rand_s);   // <-- the Key for the Server Writer
    vector<u8> kcw = H5(secret, rand_c, rand_s);   // <-- the Key for the Client Writer
    m_readable = new tReadableAES(m_internal_readable, kOpModeCBC,
                                  &kcw[0], s_toKeyLen(kcw.size()));
    m_writable = new tWritableAES(m_internal_writable, kOpModeCBC,
                                  &ksw[0], s_toKeyLen(ksw.size()));
}

void tSecureStream::m_setupClient(const tRSA& rsa, string appGreeting)
{
    // Generate the client random vector.
    vector<u8> rand_c = s_genRand(kRandVectLen);

    // Generate the pre-secret random vector.
    vector<u8> pre_secret = s_genRand(kPreSecretLen);

    // Encrypt the pre-secret under the server's RSA public key.
    vector<u8> enc = rsa.encrypt(pre_secret);

    // Send all this to the server.
    pack(m_internal_writable, kLibrhoGreeting);
    pack(m_internal_writable, appGreeting);
    pack(m_internal_writable, rand_c);
    pack(m_internal_writable, enc);
    s_flush(m_internal_writable);

    // Read the greeting from the server.
    string greetingResponse;
    try {
        unpack(m_internal_readable, greetingResponse,
               (u32)(std::max(kSuccessfulGreeting.length(), kFailedGreeting.length())));
    } catch (ebObject& e) {
        throw eRuntimeError("The secure server didn't reply with its greeting.");
    }
    if (greetingResponse != kSuccessfulGreeting)
        throw eRuntimeError("The secure server sent a failure greeting.");

    // Read the random server bytes.
    vector<u8> rand_s;
    try {
        unpack(m_internal_readable, rand_s, kRandVectLen);
    } catch (ebObject& e) {
        throw eRuntimeError("The secure server sent a random vector of the wrong length.");
    }
    if (rand_s.size() != kRandVectLen)
        throw eRuntimeError("The secure server sent a random vector of the wrong length.");

    // Calculated the shared secret (from the pre-secret).
    vector<u8> secret = H1(pre_secret, rand_c, rand_s);

    // Read the proof-of-server hash.
    vector<u8> fPrime = H2(secret, rand_c, rand_s);
    vector<u8> f;
    try {
        unpack(m_internal_readable, f, (u32)fPrime.size());
    } catch (ebObject& e) {
        throw eRuntimeError("The secure server failed to verify itself.");
    }
    if (f != fPrime)
        throw eRuntimeError("The secure server failed to verify itself.");

    // Generate the proof-of-client. (That is, prove we are not just replaying some other connection.)
    vector<u8> g = H3(secret, rand_c, rand_s);
    pack(m_internal_writable, g);
    s_flush(m_internal_writable);

    // Setup secure streams with the server.
    vector<u8> ksw = H4(secret, rand_c, rand_s);   // <-- the Key for the Server Writer
    vector<u8> kcw = H5(secret, rand_c, rand_s);   // <-- the Key for the Client Writer
    m_readable = new tReadableAES(m_internal_readable, kOpModeCBC,
                                  &ksw[0], s_toKeyLen(ksw.size()));
    m_writable = new tWritableAES(m_internal_writable, kOpModeCBC,
                                  &kcw[0], s_toKeyLen(kcw.size()));
}


}   // namespace crypt
}   // namespace rho
