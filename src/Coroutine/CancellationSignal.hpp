#ifndef CANCELLATION_SIGNAL_HPP
#define CANCELLATION_SIGNAL_HPP

#include "Misc/StrandExecutor.hpp"

#include <boost/asio/cancellation_signal.hpp>

// thread safe cancellation signal
class TSCancellationSignal {
public:
    typedef std::shared_ptr<boost::asio::cancellation_signal> TSignal;

    TSCancellationSignal(TStrandExecutor ex);

    void emit(boost::asio::cancellation_type type);
    TStrandExecutor executor() const;
    TSignal signal() const;

private:
    TStrandExecutor m_executor;
    TSignal m_signal;
};

typedef std::shared_ptr<TSCancellationSignal> TCancellationSignal;

class ScopedCancellationSignal {
public:
    using cancel_type = boost::asio::cancellation_type;
    ScopedCancellationSignal(TCancellationSignal signal,
                             cancel_type type = cancel_type::all);

    ~ScopedCancellationSignal();

    TCancellationSignal signal() const;

private:
    TCancellationSignal m_signal;
    const cancel_type m_type;
};

#endif /* CANCELLATION_SIGNAL_HPP */
