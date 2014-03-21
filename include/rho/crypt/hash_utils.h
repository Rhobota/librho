#ifndef __rho_crypt_hash_utils_h__
#define __rho_crypt_hash_utils_h__


#include <rho/types.h>

#include <string>
#include <vector>


namespace rho
{
namespace crypt
{


/**
 * Converts a hash (aka, a vector of bytes) to a string
 * representation (aka, a lowercase concatenation of bytes,
 * each byte represented by two hex characters).
 */
std::string hashToString(const std::vector<u8>& hash);


/**
 * HMAC over MD5 -- returns a 16byte hash
 */
std::vector<u8> hmac_md5(const std::vector<u8>& key, const std::vector<u8>& message);


/**
 * HMAC over SHA1 -- returns a 20byte hash
 */
std::vector<u8> hmac_sha1(const std::vector<u8>& key, const std::vector<u8>& message);


/**
 * HMAC over SHA224 -- returns a 28byte hash
 */
std::vector<u8> hmac_sha224(const std::vector<u8>& key, const std::vector<u8>& message);


/**
 * HMAC over SHA256 -- returns a 32byte hash
 */
std::vector<u8> hmac_sha256(const std::vector<u8>& key, const std::vector<u8>& message);


/**
 * HMAC over SHA384 -- returns a 48byte hash
 */
std::vector<u8> hmac_sha384(const std::vector<u8>& key, const std::vector<u8>& message);


/**
 * HMAC over SHA512 -- returns a 64byte hash
 */
std::vector<u8> hmac_sha512(const std::vector<u8>& key, const std::vector<u8>& message);


/**
 * A funtion pointer typedef for the hmac_<hasher> family of functions above.
 */
typedef std::vector<u8> (*hmac_func)(const std::vector<u8>& key, const std::vector<u8>& message);


/**
 * Does the "Password-Based Key Derivation Function 2" on the given password, using
 * the given salt and the given hmac_func.
 *
 * It does 'c' iterations and generates a hash of length 'dklen'. The returned
 * vector has length 'dklen'.
 *
 * This is part of the PKCS #5 series.
 * See RFC 2898.
 */
std::vector<u8> pbkdf2(hmac_func prf,
                       const std::vector<u8>& password, const std::vector<u8>& salt,
                       u32 c, u32 dklen);


}  // namespace crypt
}  // namespace rho


#endif // __rho_crypt_hash_utils_h__
