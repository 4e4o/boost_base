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
    boost::asio::io_context m_io;
    std::unique_ptr<boost::asio::io_context::work> m_work;
    std::vector<std::thread> m_threads;
    int m_threadNum;
};

#endif // THREAD_POOL_H
