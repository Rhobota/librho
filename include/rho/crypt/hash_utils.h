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
 * HMAC over MD5
 */
std::vector<u8> hmac_md5(const std::vector<u8>& key, const std::vector<u8>& message);


/**
 * HMAC over SHA1
 */
std::vector<u8> hmac_sha1(const std::vector<u8>& key, const std::vector<u8>& message);


/**
 * HMAC over SHA256
 */
std::vector<u8> hmac_sha256(const std::vector<u8>& key, const std::vector<u8>& message);


}  // namespace crypt
}  // namespace rho


#endif // __rho_crypt_hash_utils_h__
