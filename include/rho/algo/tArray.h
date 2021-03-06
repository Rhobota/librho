#ifndef __rho_algo_tArray_h__
#define __rho_algo_tArray_h__


#include <rho/ppcheck.h>
#include <rho/eRho.h>
#include <rho/types.h>
#include <algorithm>


namespace rho
{
namespace algo
{


template <class T>
class tArray
{
    public:

        tArray()
            : m_size(0),
              m_capacity(8),
              m_buf(new T[8])
        {
        }

        explicit
        tArray(size_t capacity)
            : m_size(0),
              m_capacity(capacity),
              m_buf(new T[capacity])
        {
        }

        tArray(size_t size, T initval)
            : m_size(size),
              m_capacity(size*2),
              m_buf(new T[size*2])
        {
            for (size_t i = 0; i < m_size; i++)
                m_buf[i] = initval;
        }

        tArray(const tArray& o)
        {
            m_size = o.m_size;
            m_capacity = o.m_capacity;
            m_buf = new T[m_capacity];
            for (size_t i = 0; i < m_size; i++)
                m_buf[i] = o.m_buf[i];
        }

        ~tArray()
        {
            m_size = 0;
            m_capacity = 0;
            delete [] m_buf;
            m_buf = NULL;
        }

        tArray& operator= (const tArray& o)
        {
            if (&o == this)
                return *this;

            if (m_capacity < o.m_size)
            {
                m_size = o.m_size;
                m_capacity = o.m_capacity;
                delete [] m_buf;
                m_buf = new T[m_capacity];
            }
            else
            {
                m_size = o.m_size;
            }

            for (size_t i = 0; i < m_size; i++)
                m_buf[i] = o.m_buf[i];

            return *this;
        }

        bool operator== (const tArray& o) const
        {
            if (m_size != o.m_size)
                return false;
            for (size_t i = 0; i < m_size; i++)
                if (m_buf[i] != o.m_buf[i])
                    return false;
            return true;
        }

        void push_back(T val)
        {
            if (m_size >= m_capacity)
                grow();
            m_buf[m_size++] = val;
        }

        void pop_back()
        {
            if (m_size == 0)
                throw eLogicError("Cannot pop when size is zero.");
            --m_size;
        }

        T& operator[] (size_t i)
        {
            return m_buf[i];
        }

        const T& operator[] (size_t i) const
        {
            return m_buf[i];
        }

        T& back()
        {
            if (m_size == 0)
                throw eLogicError("Cannot get back element when size is zero.");
            return m_buf[m_size-1];
        }

        const T& back() const
        {
            if (m_size == 0)
                throw eLogicError("Cannot get back element when size is zero.");
            return m_buf[m_size-1];
        }

        void reverse()
        {
            if (m_size < 2)
                return;
            size_t a = 0;
            size_t b = m_size-1;
            while (a < b)
            {
                std::swap(m_buf[a], m_buf[b]);
                ++a;
                --b;
            }
        }

        size_t size() const
        {
            return m_size;
        }

        void setSize(size_t size)
        {
            while (size >= m_capacity)
                grow();
            m_size = size;
        }

        void clear()
        {
            m_size = 0;
        }

    private:

        void grow()
        {
            if (m_capacity == 0)
                m_capacity = 1;
            m_capacity *= 2;
            T* buf = new T[m_capacity];
            for (size_t i = 0; i < m_size; i++)
                buf[i] = m_buf[i];
            delete [] m_buf;
            m_buf = buf;
        }

    private:

        size_t m_size;
        size_t m_capacity;
        T* m_buf;
};


}  // namespace algo
}  // namespace rho


#endif    // __rho_algo_tArray_h__
