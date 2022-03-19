#include "Timer.hpp"
#include "Debug.hpp"
#include "ScopeGuard.hpp"

using namespace boost::asio;
using namespace boost::system;

// FIXME TEST IT!

class ThreadSafeCancelation : public std::enable_shared_from_this<ThreadSafeCancelation> {
public:
    ThreadSafeCancelation()
        : m_canceled(false) {
    }

    ~ThreadSafeCancelation() {
    }

    template<typename Callable>
    auto wrap(Callable&& c) {
        auto self = shared_from_this();
        auto wrapper = [this, self, c = std::move(c)](const error_code& ec) {
            std::lock_guard lk(m_mutex);

            if (m_canceled)
                return;

            c(ec);
        };

        return std::move(wrapper);
    }

    void cancel() {
        std::lock_guard lk(m_mutex);
        m_canceled = true;
    }

private:
    bool m_canceled;
    std::mutex m_mutex;
};

Timer::Timer(io_context& io, const TTimeDuration& d)
    : StrandHolder(io),
      m_duration(d),
      m_timeout(io),
      m_cancelation(new ThreadSafeCancelation()) {
    //debug_print(boost::format("Timer::Timer %1%") % this);
}

Timer::~Timer() {
    stopTimer();
    // debug_print(boost::format("Timer::~Timer %1%") % this);
}

bool Timer::startTimer(const TTimeDuration& d) {
    if (d.has_value())
        m_duration = d;

    if (!m_duration.has_value())
        return false;

    //    debug_print(boost::format("Timer::startTimer %1% %2%") % this % m_duration.value());

    stopTimer();
    started(this);
    m_timeout.expires_from_now(m_duration.value());
    m_cancelation.reset(new ThreadSafeCancelation());
    m_timeout.async_wait(bindExecutor(m_cancelation->wrap([this](const error_code& ec) {
        // чтоб complete вызывался после timeout
        // нужно для юзабельности lockWhenActive
        ScopeGuard sg([this, ec] {
            complete(this, ec);
        });

        if (ec)
            return;

        timeout(this);
    })));

    return true;
}

bool Timer::stopTimer() {
    error_code ec;
    m_timeout.cancel(ec);
    m_cancelation->cancel();
    return !ec;
}

const TTimeDuration& Timer::duration() const {
    return m_duration;
}

void Timer::setDuration(const TTimeDuration &duration) {
    m_duration = duration;
}
