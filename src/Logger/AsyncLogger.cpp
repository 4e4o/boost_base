#include "AsyncLogger.hpp"

using namespace std::literals::chrono_literals;

#define FLUSH_INTERVAL 1s

AsyncLogger::AsyncLogger(boost::asio::io_context& io)
    : SimpleLogger(false), CoroutineTask(io) {
}

AsyncLogger::~AsyncLogger() {
    flush();
}

TAwaitVoid AsyncLogger::onStop() {
    flush();
    co_return;
}

void AsyncLogger::logImpl(const std::string& msg) {
    // юзаем мьютекс а не пост чтоб иметь
    // возможность по завершению программы выводить заключительные логи без
    // запущенных io_context
    std::lock_guard lg(m_mutex);
    m_logs.push(msg);
}

void AsyncLogger::flush() {
    TLogs logs;

    {
        std::lock_guard lg(m_mutex);
        logs = std::move(m_logs);
    }

    if (logs.empty())
        return;

    std::string oneMsg;

    while(!logs.empty()) {
        oneMsg += logs.front();
        logs.pop();

        if (!logs.empty())
            oneMsg += "\n";
    }

    SimpleLogger::logImpl(oneMsg);
    fflush(stdout);
}

AsyncLogger::TAwaitResult AsyncLogger::run() {
    while(running()) {
        flush();
        co_await wait(FLUSH_INTERVAL);
    }
}
