#ifndef __rho_sync_tThreadLocal_h__
#define __rho_sync_tThreadLocal_h__


#include <rho/bNonCopyable.h>
#include <rho/eRho.h>

#include <pthread.h>    // posix header


namespace rho
{
namespace sync
{


template <class T>
class tThreadLocal : public bNonCopyable
{
    public:

        tThreadLocal();

        operator bool ();

        void operator= (T* object);

        T* get();

        T* operator-> ();

        T& operator* ();

        ~tThreadLocal();

    private:

        pthread_key_t  m_key;
};


template <class T>
tThreadLocal<T>::tThreadLocal()
{
    if (pthread_key_create(&m_key, NULL) != 0)
        throw eResourceAcquisitionError("Dude, this is bad: can't create tls key.");
}

template <class T>
tThreadLocal<T>::operator bool ()
{
    return pthread_getspecific(m_key) != NULL;
}

template <class T>
void tThreadLocal<T>::operator= (T* object)
{
    T* oldObj = get();
    if (oldObj)
        delete oldObj;
    if (pthread_setspecific(m_key, object) != 0)
        throw eRuntimeError("Can't store tls object.");
}

template <class T>
T* tThreadLocal<T>::get()
{
    return static_cast<T*>(pthread_getspecific(m_key));
}

template <class T>
T* tThreadLocal<T>::operator-> ()
{
    return static_cast<T*>(pthread_getspecific(m_key));
}

template <class T>
T& tThreadLocal<T>::operator* ()
{
    return *(static_cast<T*>(pthread_getspecific(m_key)));
}

template <class T>
tThreadLocal<T>::~tThreadLocal()
{
    T* oldObj = get();
    if (oldObj)
        delete oldObj;
    if (pthread_key_delete(m_key) != 0)
        throw eRuntimeError("Can't delete tls key.");
}


}     // namespace sync
}     // namespace rho


#endif    // __rho_sync_tThreadLocal_h__
