#ifndef TIMER_HPP
#define TIMER_HPP

#include <boost/asio/deadline_timer.hpp>
#include <boost/signals2.hpp>

#include "Lifecycle.hpp"
#include "TimeDuration.hpp"
#include "TimerForward.hpp"
#include "Misc/StrandHolder.hpp"

class ThreadSafeCancelation;

/**
 * @brief The Timer class
 *
 */

class Timer : public StrandHolder {
public:
    Timer(boost::asio::io_context&, const TTimeDuration& = boost::none);
    virtual ~Timer();

    bool startTimer(const TTimeDuration& = boost::none);
    bool stopTimer();

    const TTimeDuration &duration() const;
    void setDuration(const TTimeDuration &duration);

    boost::signals2::signal<void(Timer*)> started;
    boost::signals2::signal<void(Timer*)> timeout;
    boost::signals2::signal<void(Timer*, const boost::system::error_code&)> complete;

    /**
     * Будет держать объект obj, пока таймер работает.
     */

    template<typename T>
    void lockWhenActive(const std::weak_ptr<T>& obj) {
        Lifecycle::lockWhen(started, complete, obj);
    }

private:
    TTimeDuration m_duration;
    boost::asio::deadline_timer m_timeout;
    std::shared_ptr<ThreadSafeCancelation> m_cancelation;
};

#endif /* TIMER_HPP */
