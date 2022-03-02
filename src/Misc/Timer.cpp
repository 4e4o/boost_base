#include "Timer.hpp"
#include "Debug.hpp"

Timer::Timer(boost::asio::io_context& io, int sec)
    : StrandHolder(io),
      m_sec(sec),
      m_timeout(io){
    //debug_print(boost::format("Timer::Timer %1%") % this);
}

Timer::~Timer() {
    // debug_print(boost::format("Timer::~Timer %1%") % this);
}

void Timer::startTimer() {
    //    debug_print(boost::format("Timer::startTimer %1% %2%") % this % m_sec);

    m_timeout.expires_from_now(boost::posix_time::seconds(m_sec));
    m_timeout.async_wait(strand().wrap([this](const boost::system::error_code& error) {
        if (error)
            return;

        onTimeout(this);
    }));
}

void Timer::stopTimer() {
    boost::system::error_code ec;
    m_timeout.cancel(ec);
}

int Timer::sec() const {
    return m_sec;
}
