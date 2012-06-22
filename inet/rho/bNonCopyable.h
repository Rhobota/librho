#ifndef __bNonCopyable_h__
#define __bNonCopyable_h__


namespace rho
{


class bNonCopyable
{
    public:

        bNonCopyable() { }

    private:

        /*
         * All the following methods are private, which prevents subclasses
         * and the outside world from calling them.
         * Therefore, anything which derives from this class will not
         * be copyable.
         */

        bNonCopyable(const bNonCopyable&);
        bNonCopyable& operator=(const bNonCopyable&);
};


}    // namespace rho


#endif   // __bNonCopyable_h__
