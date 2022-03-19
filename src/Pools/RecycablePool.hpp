#ifndef RECYCABLE_POOL_HPP
#define RECYCABLE_POOL_HPP

class Recycable;

class RecycablePool {
public:
    RecycablePool();
    virtual ~RecycablePool();

    virtual void recycle(Recycable*) = 0;
};

#endif // RECYCABLE_POOL_HPP
