#include "Recycable.hpp"
#include "RecycablePool.hpp"

Recycable::Recycable()
    : m_pool(nullptr) {
}

Recycable::~Recycable() {
}

void Recycable::setRecycablePool(RecycablePool *p) {
    m_pool = p;
}

void Recycable::recycle() {
    if (m_pool == nullptr)
        return;

    m_pool->recycle(this);
}
