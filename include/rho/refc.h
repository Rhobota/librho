#ifndef __rho_refc_h__
#define __rho_refc_h__


#include <rho/ppcheck.h>
#include <rho/eRho.h>

#include <rho/sync/tAtomicInt.h>
#include <rho/sync/tAutoSync.h>
#include <rho/sync/tMutex.h>

#include <cstdlib>
#include <map>


namespace rho
{


template <class T>
class refc
{
    public:

        refc();
        explicit refc(T* object);
        refc(const refc& other);

        const refc& operator= (T* object);
        const refc& operator= (const refc& other);

        T& operator* ();
        T* operator-> ();
        operator T* ();

        const T& operator* () const;
        const T* operator-> () const;
        operator const T* () const;

        bool operator== (const refc& other) const;
        bool operator!= (const refc& other) const;
        bool operator<  (const refc& other) const;

        u32 count() const;

        ~refc();

    private:

        T*          m_object;
        sync::au32* m_ref_count;
};


extern std::map<void*, sync::au32*> gAllKnownRefcObjectsMap;

extern sync::tMutex                 gAllKnownRefcObjectsSync;


template <class T>
refc<T>::refc()
    : m_object(NULL),
      m_ref_count(NULL)
{
}

template <class T>
refc<T>::refc(T* object)
    : m_object(NULL),
      m_ref_count(NULL)
{
    if (object == NULL)
    {
        return;
    }

    sync::tAutoSync as(gAllKnownRefcObjectsSync);
    if (gAllKnownRefcObjectsMap.find(object) != gAllKnownRefcObjectsMap.end())
    {
        m_object = object;
        m_ref_count = gAllKnownRefcObjectsMap[object];
        ++(*m_ref_count);
    }
    else
    {
        m_object = object;
        m_ref_count = new sync::au32(1);
        gAllKnownRefcObjectsMap.insert(
                std::make_pair((void*)m_object, m_ref_count));
    }
}

template <class T>
refc<T>::refc(const refc<T>& other)
    : m_object(NULL),
      m_ref_count(NULL)
{
    m_object = other.m_object;
    m_ref_count = other.m_ref_count;
    if (m_ref_count)
        ++(*m_ref_count);
}

template <class T>
const refc<T>& refc<T>::operator= (const refc<T>& other)
{
    if (m_object == other.m_object)
        return *this;

    if ((m_ref_count) && --(*m_ref_count) == 0)
    {
        {
            sync::tAutoSync as(gAllKnownRefcObjectsSync);
            gAllKnownRefcObjectsMap.erase(m_object);
        }
        delete m_object;
        delete m_ref_count;
    }

    m_object = other.m_object;
    m_ref_count = other.m_ref_count;
    if (m_ref_count)
        ++(*m_ref_count);

    return *this;
}

template <class T>
const refc<T>& refc<T>::operator= (T* object)
{
    if (m_object == object)
        return *this;

    if ((m_ref_count) && --(*m_ref_count) == 0)
    {
        {
            sync::tAutoSync as(gAllKnownRefcObjectsSync);
            gAllKnownRefcObjectsMap.erase(m_object);
        }
        delete m_object;
        delete m_ref_count;
    }

    if (object == NULL)
    {
        m_object = NULL;
        m_ref_count = NULL;
        return *this;
    }

    sync::tAutoSync as(gAllKnownRefcObjectsSync);
    if (gAllKnownRefcObjectsMap.find(object) != gAllKnownRefcObjectsMap.end())
    {
        m_object = object;
        m_ref_count = gAllKnownRefcObjectsMap[object];
        ++(*m_ref_count);
    }
    else
    {
        m_object = object;
        m_ref_count = new sync::au32(1);
        gAllKnownRefcObjectsMap.insert(
                std::make_pair((void*)m_object, m_ref_count));
    }

    return *this;
}

template <class T>
T& refc<T>::operator* ()
{
    if (m_object == NULL)
        throw eNullPointer("Dereferencing NULL!");
    return *m_object;
}

template <class T>
const T& refc<T>::operator* () const
{
    if (m_object == NULL)
        throw eNullPointer("Dereferencing NULL!");
    return *m_object;
}

template <class T>
T* refc<T>::operator-> ()
{
    if (m_object == NULL)
        throw eNullPointer("Dereferencing NULL!");
    return m_object;
}

template <class T>
const T* refc<T>::operator-> () const
{
    if (m_object == NULL)
        throw eNullPointer("Dereferencing NULL!");
    return m_object;
}

template <class T>
bool refc<T>::operator== (const refc<T>& other) const
{
    return m_object == other.m_object;
}

template <class T>
bool refc<T>::operator!= (const refc<T>& other) const
{
    return m_object != other.m_object;
}

template <class T>
bool refc<T>::operator< (const refc<T>& other) const
{
    return m_object < other.m_object;
}

template <class T>
u32 refc<T>::count() const
{
    if (m_ref_count == NULL)
        throw eNullPointer("Dereferencing NULL!");
    return m_ref_count->val();
}

template <class T>
refc<T>::operator T* ()
{
    return m_object;
}

template <class T>
refc<T>::operator const T* () const
{
    return m_object;
}

template <class T>
refc<T>::~refc()
{
    if ((m_ref_count) && --(*m_ref_count) == 0)
    {
        {
            sync::tAutoSync as(gAllKnownRefcObjectsSync);
            gAllKnownRefcObjectsMap.erase(m_object);
        }
        delete m_object;
        delete m_ref_count;
    }
    m_object = NULL;
    m_ref_count = NULL;
}


}    // namespace rho


#endif   // __rho_refc_h__
