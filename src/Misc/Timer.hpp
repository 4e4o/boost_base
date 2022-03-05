#ifndef TIMER_HPP
#define TIMER_HPP

#include <boost/asio/deadline_timer.hpp>
#include <boost/signals2.hpp>

#include "Misc/StrandHolder.hpp"

class Timer : public StrandHolder {
public:
    typedef boost::optional<int> TSec;

    Timer(boost::asio::io_context&, const TSec& = boost::none);
    virtual ~Timer();

    boost::signals2::signal<void(Timer*)> onTimeout;

    void startTimer(const TSec& = boost::none);
    void stopTimer();

    const TSec& sec() const;

private:
    TSec m_sec;
    boost::asio::deadline_timer m_timeout;
};

#endif /* TIMER_HPP */
