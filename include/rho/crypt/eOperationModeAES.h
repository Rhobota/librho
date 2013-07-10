#ifndef __rho_crypt_eOperationModeAES_h__
#define __rho_crypt_eOperationModeAES_h__


namespace rho
{
namespace crypt
{


enum eOperationModeAES
{
    kOpModeECB,         // "electronic code book"
    kOpModeCBC          // "cipher-block chaining"
};


}    // namespace crypt
}    // namespace rho


#endif   // __rho_crypt_eOperationModeAES_h__
