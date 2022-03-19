#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <boost/asio/io_context.hpp>

#include <thread>

class ThreadPool {
public:
    ThreadPool(int threadNum = (std::thread::hardware_concurrency() - 1));
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;

    boost::asio::io_context &io();
    int run();
    void stop(bool join);

private:
    using work_guard_type = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;

    boost::asio::io_context m_io;
    work_guard_type m_workGuard;
    std::vector<std::thread> m_threads;
    int m_threadNum;
};

#endif // THREAD_POOL_H
