#ifndef __rho_refc_h__
#define __rho_refc_h__


#include "ebObject.h"

#include <cstdlib>
#include <map>


namespace rho
{


template <class T>
class refc
{
    public:

        refc(T* object);
        refc(const refc& other);

        const refc& operator= (T* object);
        const refc& operator= (const refc& other);

        T& operator* ();
        T* operator-> ();

        const T& operator* () const;
        const T* operator-> () const;

        bool operator== (const refc& other) const;
        bool operator!= (const refc& other) const;
        bool operator<  (const refc& other) const;

        ~refc();

    private:

        T*   m_object;
        int* m_ref_count;
};


static std::map<void*, int*> gAllKnownRefcObjectsMap;


template <class T>
refc<T>::refc(T* object)
    : m_object(NULL),
      m_ref_count(NULL)
{
    if (object == NULL)
        throw std::logic_error("Reference counting NULL makes no sense.");

    if (gAllKnownRefcObjectsMap.find(object) != gAllKnownRefcObjectsMap.end())
    {
        m_object = object;
        m_ref_count = gAllKnownRefcObjectsMap[object];
        ++(*m_ref_count);
    }
    else
    {
        m_object = object;
        m_ref_count = new int(1);
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
    ++(*m_ref_count);
}

template <class T>
const refc<T>& refc<T>::operator= (const refc<T>& other)
{
    if (m_object == other.m_object)
        return *this;

    --(*m_ref_count);
    if (*m_ref_count == 0)
    {
        gAllKnownRefcObjectsMap.erase(m_object);
        delete m_object;
        delete m_ref_count;
    }

    m_object = other.m_object;
    m_ref_count = other.m_ref_count;
    ++(*m_ref_count);

    return *this;
}

template <class T>
const refc<T>& refc<T>::operator= (T* object)
{
    if (m_object == object)
        return *this;

    if (object == NULL)
        throw std::logic_error("Reference counting NULL makes no sense.");

    --(*m_ref_count);
    if (*m_ref_count == 0)
    {
        gAllKnownRefcObjectsMap.erase(m_object);
        delete m_object;
        delete m_ref_count;
    }

    if (gAllKnownRefcObjectsMap.find(object) != gAllKnownRefcObjectsMap.end())
    {
        m_object = object;
        m_ref_count = gAllKnownRefcObjectsMap[object];
        ++(*m_ref_count);
    }
    else
    {
        m_object = object;
        m_ref_count = new int(1);
        gAllKnownRefcObjectsMap.insert(
                std::make_pair((void*)m_object, m_ref_count));
    }

    return *this;
}

template <class T>
T& refc<T>::operator* ()
{
    return *m_object;
}

template <class T>
const T& refc<T>::operator* () const
{
    return *m_object;
}

template <class T>
T* refc<T>::operator-> ()
{
    return m_object;
}

template <class T>
const T* refc<T>::operator-> () const
{
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
refc<T>::~refc()
{
    --(*m_ref_count);
    if (*m_ref_count == 0)
    {
        gAllKnownRefcObjectsMap.erase(m_object);
        delete m_object;
        delete m_ref_count;
    }
    m_object = NULL;
    m_ref_count = NULL;
}


}    // namespace rho


#endif   // __rho_refc_h__
