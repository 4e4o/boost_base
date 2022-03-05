#include "Timer.hpp"
#include "Debug.hpp"

Timer::Timer(boost::asio::io_context& io, const TSec& sec)
    : StrandHolder(io),
      m_sec(sec),
      m_timeout(io) {
    //debug_print(boost::format("Timer::Timer %1%") % this);
}

Timer::~Timer() {
    // debug_print(boost::format("Timer::~Timer %1%") % this);
}

void Timer::startTimer(const TSec& sec) {
    if (sec.has_value())
        m_sec = sec;

    if (!m_sec.has_value())
        return;

    //    debug_print(boost::format("Timer::startTimer %1% %2%") % this % m_sec);

    m_timeout.expires_from_now(boost::posix_time::seconds(m_sec.value()));
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

const Timer::TSec& Timer::sec() const {
    return m_sec;
}
