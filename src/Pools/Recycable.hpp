#ifndef RECYCABLE_HPP
#define RECYCABLE_HPP

class RecycablePool;

class Recycable {
public:
    Recycable();
    virtual ~Recycable();

    void setRecycablePool(RecycablePool*);

    virtual void recycle();

private:
    RecycablePool *m_pool;
};

#endif // RECYCABLE_HPP
