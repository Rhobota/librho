#ifndef __rho_sync_tPCQ_h__
#define __rho_sync_tPCQ_h__


#include <rho/bNonCopyable.h>
#include <rho/eRho.h>
#include <rho/sync/ebSync.h>
#include <rho/types.h>

#include <pthread.h>

#include <queue>
#include <list>


namespace rho
{
namespace sync
{


/**
 * Producer-Consumer Queue (PCQ)
 *
 * This PCQ is completely thread-safe. Use it with as many producers and
 * as many consumers as you need.
 */
template <class T>
class tPCQ;


/**
 * tPCQ can be customized for different behaviors when its capacity
 * is reached. The following are the behaviors it supports.
 */
enum nPCQOverflowBehavior
{
    kDiscard,  // <-- Items which will not fit into the queue will be
               //     silently discarded.
    kBlock,    // <-- The inserting thread will block until space is
               //     available.
    kGrow,     // <-- The queue will grow to accommodate all items
               //     placed into it.
    kThrow     // <-- If an item insertion overflows the queue it
               //     will throw eBufferOverflow.
};


template <class T>
class tPCQ : public bNonCopyable
{
    public:

        /**
         * Creates a producer-consumer queue with the specified capacity
         * and overflow behavior.
         */
        tPCQ(u32 capacity, nPCQOverflowBehavior behavior);

        /**
         * Pushes the item onto the queue. The overflow behavior
         * specified at the queue's construction will be used
         * if the item overflows the queue's capacity.
         */
        void push(T item);

        /**
         * Pops the front item off of the queue and returns it. If the
         * queue has no items when this method is called, the calling
         * thread will block until an item is available.
         */
        T pop();

        /**
         * Returns the number of items in the queue.
         */
        u32 size() const;

        /**
         * Returns the capacity of the queue. This is the number of items
         * which will fit into the queue before the overflow behavior is
         * performed.
         */
        u32 capacity() const;

        /**
         * Destructs...
         */
        ~tPCQ();

    private:

        void m_push(T& item);

        void m_lock();
        void m_unlock();

        class tAutoLock
        {
            public:
                tAutoLock(tPCQ* pcq) : m_pcq(pcq) { m_pcq->m_lock(); }
                ~tAutoLock() { m_pcq->m_unlock(); }
            private:
                tPCQ* m_pcq;
        };

    private:

        u32 m_size;
        u32 m_capacity;

        nPCQOverflowBehavior m_behavior;

        std::queue< T, std::list<T> > m_queue;

        pthread_mutex_t m_mutex;
        pthread_cond_t  m_queueHasSomething;
        pthread_cond_t  m_queueHasRoom;
};


template <class T>
tPCQ<T>::tPCQ(u32 capacity, nPCQOverflowBehavior behavior)
    : m_size(0),
      m_capacity(capacity),
      m_behavior(behavior),
      m_queue(),
      m_mutex(),
      m_queueHasSomething(),
      m_queueHasRoom()
{
    if (m_capacity == 0)
    {
        throw eInvalidArgument("The queue's capacity must be >0");
    }
    if (pthread_mutex_init(&m_mutex, NULL) != 0)
    {
        throw eMutexCreationError();
    }
    if (pthread_cond_init(&m_queueHasSomething, NULL) != 0)
    {
        pthread_mutex_destroy(&m_mutex);
        throw eConditionCreationError();
    }
    if (pthread_cond_init(&m_queueHasRoom, NULL) != 0)
    {
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_queueHasSomething);
        throw eConditionCreationError();
    }
}

template <class T>
tPCQ<T>::~tPCQ()
{
    pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_queueHasSomething);
    pthread_cond_destroy(&m_queueHasRoom);
}

template <class T>
void tPCQ<T>::push(T item)
{
    tAutoLock al(this);

    if (m_size < m_capacity)
    {
        m_push(item);
        return;
    }

    switch (m_behavior)
    {
        case kDiscard:
            break;
        case kBlock:
            while (m_size == m_capacity)
            {
                if (pthread_cond_wait(&m_queueHasRoom, &m_mutex) != 0)
                    throw eRuntimeError("Why can't I wait on the condition?");
            }
            m_push(item);
            break;
        case kGrow:
            m_push(item);
            m_capacity *= 2;
            break;
        case kThrow:
            throw eBufferOverflow("The PCQ is out of room!");
            break;
        default:
            throw eLogicError("What?");
            break;
    }
}

template <class T>
T tPCQ<T>::pop()
{
    tAutoLock al(this);
    while (m_size == 0)
    {
        if (pthread_cond_wait(&m_queueHasSomething, &m_mutex) != 0)
            throw eRuntimeError("Why can't I wait on the condition?");
    }
    T f = m_queue.front();
    m_queue.pop();
    --m_size;
    if (m_size == m_capacity-1)
    {
        if (pthread_cond_broadcast(&m_queueHasRoom) != 0)
            throw eRuntimeError("Why can't I broadcast?");
    }
    return f;
}

template <class T>
u32 tPCQ<T>::size() const
{
    tAutoLock al(const_cast<tPCQ<T>*>(this));
    return m_size;
}

template <class T>
u32 tPCQ<T>::capacity() const
{
    tAutoLock al(const_cast<tPCQ<T>*>(this));
    return m_capacity;
}

template <class T>
void tPCQ<T>::m_push(T& item)
{
    m_queue.push(item);
    ++m_size;
    if (m_size == 1)
    {
        if (pthread_cond_broadcast(&m_queueHasSomething) != 0)
            throw eRuntimeError("Why can't I broadcast?");
    }
}

template <class T>
void tPCQ<T>::m_lock()
{
    if (pthread_mutex_lock(&m_mutex) != 0)
        throw eRuntimeError("Cannot lock pcq mutex!");
}

template <class T>
void tPCQ<T>::m_unlock()
{
    if (pthread_mutex_unlock(&m_mutex) != 0)
        throw eRuntimeError("Cannot unlock pcq mutex!");
}


}    // namespace sync
}    // namespace rho


#endif   // __rho_sync_tPCQ_h__
