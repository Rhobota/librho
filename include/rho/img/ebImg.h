#ifndef __rho_img_ebImg_h__
#define __rho_img_ebImg_h__


#include <rho/eRho.h>


namespace rho
{
namespace img
{


class ebImg : public ebObject
{
    public:

        ebImg(std::string reason) : ebObject(reason) { }

        ~ebImg() throw() { }
};


class eDeviceOpenError : public ebImg
{
    public:

        eDeviceOpenError(std::string reason) : ebImg(reason) { }

        ~eDeviceOpenError() throw() { }
};


class eInvalidDeviceCapabilities : public ebImg
{
    public:

        eInvalidDeviceCapabilities(std::string reason) : ebImg(reason) { }

        ~eInvalidDeviceCapabilities() throw() { }
};


class eInvalidDeviceAttributes : public ebImg
{
    public:

        eInvalidDeviceAttributes(std::string reason) : ebImg(reason) { }

        ~eInvalidDeviceAttributes() throw() { }
};


class eColorspaceConversionError : public ebImg
{
    public:

        eColorspaceConversionError(std::string reason) : ebImg(reason) { }

        ~eColorspaceConversionError() throw() { }
};


}    // namespace img
}    // namespace rho


#endif    // __rho_img_ebImg_h__
