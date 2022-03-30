#ifndef ASYNC_LOGGER_HPP
#define ASYNC_LOGGER_HPP

#include "Coroutine/CoroutineTask.hpp"
#include "SimpleLogger.hpp"

#include <queue>

class AsyncLogger : public SimpleLogger, public CoroutineTask<void> {
public:
    AsyncLogger(boost::asio::io_context&);
    ~AsyncLogger();

    void log(const std::string&) override final;

private:
    TAwaitResult run() override;
    void flush();

    typedef std::queue<std::string> TLogs;

    std::mutex m_mutex;
    TLogs m_logs;
};

#endif // ASYNC_LOGGER_HPP
