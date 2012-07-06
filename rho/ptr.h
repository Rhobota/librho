#ifndef __rho_ptr_h__
#define __rho_ptr_h__


#include "bNonCopyable.h"
#include "ebObject.h"

#include <cstdlib>


namespace rho
{


template <class T>
class ptr : public bNonCopyable
{
    public:

        ptr();
        ptr(T* p);

        const ptr& operator= (T* p);

        T& operator* ();
        T* operator-> ();

        const T& operator* () const;
        const T* operator-> () const;

        bool operator== (const ptr& other) const;
        bool operator!= (const ptr& other) const;
        bool operator<  (const ptr& other) const;

        ~ptr();

    private:

        T* m_p;
};


template <class T>
ptr<T>::ptr()
    : m_p(NULL)
{
}

template <class T>
ptr<T>::ptr(T* p)
    : m_p(p)
{
}

template <class T>
const ptr<T>& ptr<T>::operator= (T* p)
{
    delete m_p;
    m_p = p;
    return *this;
}

template <class T>
T& ptr<T>::operator* ()
{
    if (!m_p)
        throw std::logic_error("Dereferencing NULL, oh no!");
    return *m_p;
}

template <class T>
const T& ptr<T>::operator* () const
{
    if (!m_p)
        throw std::logic_error("Dereferencing NULL, oh no!");
    return *m_p;
}

template <class T>
T* ptr<T>::operator-> ()
{
    if (!m_p)
        throw std::logic_error("Dereferencing NULL, oh no!");
    return m_p;
}

template <class T>
const T* ptr<T>::operator-> () const
{
    if (!m_p)
        throw std::logic_error("Dereferencing NULL, oh no!");
    return m_p;
}

template <class T>
bool ptr<T>::operator== (const ptr<T>& other) const
{
    return m_p == other.m_p;
}

template <class T>
bool ptr<T>::operator!= (const ptr<T>& other) const
{
    return m_p != other.m_p;
}

template <class T>
bool ptr<T>::operator<  (const ptr<T>& other) const
{
    return m_p < other.m_p;
}

template <class T>
ptr<T>::~ptr()
{
    delete m_p;
    m_p = NULL;
}


}    // namespace rho


#endif   // __rho_ptr_h__
