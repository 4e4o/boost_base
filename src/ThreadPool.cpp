#include "ThreadPool.hpp"

ThreadPool::ThreadPool(int threadNum)
    : m_workGuard(m_io.get_executor()),
      m_threadNum(threadNum) {
}

boost::asio::io_context &ThreadPool::io() {
    return m_io;
}

int ThreadPool::run() {
    for (int i = 0; i < m_threadNum; ++i)
        m_threads.emplace_back([this] { m_io.run(); });

    m_io.run();
    return 0;
}

void ThreadPool::stop(bool join) {
    m_workGuard.reset();

    if (join) {
        for (auto &t: m_threads)
            t.join();
    }
}
