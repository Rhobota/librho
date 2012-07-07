#ifndef __rho_sync_tAtomicInt_h__
#define __rho_sync_tAtomicInt_h__


#include "tAutoSync.h"
#include "tMutex.h"

#include "rho/bNonCopyable.h"
#include "rho/types.h"


namespace rho
{
namespace sync
{


template <class T>
class tAtomicInt;


typedef tAtomicInt<u8>  au8;
typedef tAtomicInt<u16> au16;
typedef tAtomicInt<u32> au32;
typedef tAtomicInt<u64> au64;

typedef tAtomicInt<i8>  ai8;
typedef tAtomicInt<i16> ai16;
typedef tAtomicInt<i32> ai32;
typedef tAtomicInt<i64> ai64;


template <class T>
class tAtomicInt : public bNonCopyable
{
    public:

        /**
         * Constructs an atomic integer initialized with zero.
         */
        tAtomicInt();

        /**
         * Constructs an atomic integer initialized with the given value.
         */
        tAtomicInt(T initialVal);

        /**
         * Sets the atomic integer equal to the given value.
         */
        void operator= (T val);

        /**
         * Conversion operator: converts to T
         */
        operator T ();

        /**
         * In-place addition operator
         */
        T operator+= (T n);

        /**
         * In-place subtraction operator
         */
        T operator-= (T n);

        /**
         * Pre-increment operator
         */
        T operator++ ();

        /**
         * Pre-decrement operator
         */
        T operator-- ();

    private:

        T      m_val;
        tMutex m_mutex;
};


template <class T>
tAtomicInt<T>::tAtomicInt()
    : m_val(0),
      m_mutex()
{
}

template <class T>
tAtomicInt<T>::tAtomicInt(T initialVal)
    : m_val(initialVal),
      m_mutex()
{
}

template <class T>
void tAtomicInt<T>::operator= (T val)
{
    tAutoSync as(m_mutex);
    m_val = val;
}

template <class T>
tAtomicInt<T>::operator T ()
{
    T temp = m_val;
    return temp;
}

template <class T>
T tAtomicInt<T>::operator+= (T n)
{
    tAutoSync as(m_mutex);
    m_val += n;
    T temp = m_val;
    return temp;
}

template <class T>
T tAtomicInt<T>::operator-= (T n)
{
    tAutoSync as(m_mutex);
    m_val -= n;
    T temp = m_val;
    return temp;
}

template <class T>
T tAtomicInt<T>::operator++ ()
{
    tAutoSync as(m_mutex);
    ++m_val;
    T temp = m_val;
    return temp;
}

template <class T>
T tAtomicInt<T>::operator-- ()
{
    tAutoSync as(m_mutex);
    --m_val;
    T temp = m_val;
    return temp;
}


}     // namespace sync
}     // namespace rho


#endif    // __rho_sync_tAtomicInt_h__
