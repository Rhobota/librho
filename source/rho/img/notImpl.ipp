#include <rho/img/iImageCapParamsEnumerator.h>
#include <rho/img/iImageCap.h>
#include <rho/eRho.h>


namespace rho
{
namespace img
{


class tImageCapParamsEnumerator : public iImageCapParamsEnumerator
{
    public:

        tImageCapParamsEnumerator()
        {
            throw eNotImplemented("tImageCapParamsEnumerator is not implemented on this platorm.");
        }

        int size() const
        {
            throw eNotImplemented("tImageCapParamsEnumerator is not implemented on this platorm.");
        }

        const tImageCapParams& operator[] (int i)
        {
            throw eNotImplemented("tImageCapParamsEnumerator is not implemented on this platorm.");
        }
};


class tImageCap : public iImageCap
{
    public:

        tImageCap(const tImageCapParams& params)
        {
            throw eNotImplemented("tImageCap is not implemented on this platorm.");
        }

        const tImageCapParams&  getParams() const
        {
            throw eNotImplemented("tImageCap is not implemented on this platorm.");
        }

        u32                     getRequiredBufSize() const
        {
            throw eNotImplemented("tImageCap is not implemented on this platorm.");
        }

        void                    getFrame(tImage* image)
        {
            throw eNotImplemented("tImageCap is not implemented on this platorm.");
        }

};


}   // namespace img
}   // namespace rho
