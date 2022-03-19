#ifndef TS_OBJ_POOL1_HPP
#define TS_OBJ_POOL1_HPP

#include "RecycablePool.hpp"

#include <boost/pool/object_pool.hpp>
#include <boost/lockfree/queue.hpp>

//#define DEBUG_TSPOOL1

#ifdef DEBUG_TSPOOL1
#include "Misc/Debug.hpp"
#endif //DEBUG_TSPOOL1

class Recycable;

/**
 * Пул объектов, потоко-безопасный по отношению к созданию-ресайклу.
 * Операция создания сама по себе не потокобезопасна, только одна в текущий момент может быть.
 * А вот recycle потоко-безопасен, юзает boost::lockfree::queue, объект попадает в эту очередь и
 * при следующем создании объекта будет юзаться.
 */

template<typename T>
class TSObjPool1 : public RecycablePool {
public:
    TSObjPool1() : m_recycled(INITIAL_RECYCLED_CAPACITY) { }
    virtual ~TSObjPool1() { }

    template <class ... ArgN>
    T * create(ArgN&&... args) {
#ifdef DEBUG_TSPOOL1
        debug_print(boost::format("TSObjPool1::create %1%, allocs = %2%, recycleUsages = %3%, recycleSize = %4%") %
                    this % m_allocCount % m_recycleUsages % m_recycleSize);
#endif

        T* r = getRecycled();

        if (r == nullptr) {
            r = m_pool.malloc();
#ifdef DEBUG_TSPOOL1
            m_allocCount++;
#endif
        }

        new (r) T(std::forward<ArgN>(args)...);
        return r;
    }

    void recycle(Recycable* p) override final {
#ifdef DEBUG_TSPOOL1
        m_recycleSize++;
#endif
        m_recycled.push(static_cast<T*>(p));
    }

private:
    static constexpr int INITIAL_RECYCLED_CAPACITY = 16;

    T* getRecycled() {
        T* item;

        if (m_recycled.pop(item)) {
#ifdef DEBUG_TSPOOL1
            m_recycleSize--;
            m_recycleUsages++;
#endif
            item->~T();
            return item;
        }

        return nullptr;
    }

#ifdef DEBUG_TSPOOL1
    std::atomic<int> m_allocCount;
    std::atomic<int> m_recycleUsages;
    std::atomic<int> m_recycleSize;
#endif

    boost::object_pool<T> m_pool;
    boost::lockfree::queue<T*> m_recycled;
};

#endif // TS_OBJ_POOL1_HPP
