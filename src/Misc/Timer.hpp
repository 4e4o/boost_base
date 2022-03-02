#ifndef TIMER_HPP
#define TIMER_HPP

#include <boost/asio/deadline_timer.hpp>
#include <boost/signals2.hpp>

#include "Misc/StrandHolder.hpp"

class Timer : public StrandHolder {
public:
    Timer(boost::asio::io_context&, int);
    virtual ~Timer();

    boost::signals2::signal<void(Timer*)> onTimeout;

    void startTimer();
    void stopTimer();

    int sec() const;

private:
    int m_sec;
    boost::asio::deadline_timer m_timeout;
};

#endif /* TIMER_HPP */
