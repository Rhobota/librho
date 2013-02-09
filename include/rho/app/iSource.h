#ifndef __rho_app_iSource_h__
#define __rho_app_iSource_h__


namespace rho
{
namespace app
{


class iSource
{
    public:

        /**
         * iSource has no real interface at this point, but it may
         * in the future.
         *
         * For now it is only used by iTarget and tDispatcher as
         * a polymorphic type.
         */

        virtual ~iSource() { }
};


}    // namespace app
}    // namespace rho


#endif    // __rho_app_iSource_h__
