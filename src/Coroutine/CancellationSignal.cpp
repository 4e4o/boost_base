#include "CancellationSignal.hpp"

TSCancellationSignal::TSCancellationSignal(TStrandExecutor ex)
    : m_executor(ex),
      m_signal(new boost::asio::cancellation_signal()) {
}

void TSCancellationSignal::emit(boost::asio::cancellation_type type) {
    auto sig = m_signal;
    // emit signal in strand executor context
    boost::asio::dispatch(m_executor, [sig, type]() {
        sig->emit(type);
    });
}

TStrandExecutor TSCancellationSignal::executor() const {
    return m_executor;
}

TSCancellationSignal::TSignal TSCancellationSignal::signal() const {
    return m_signal;
}

ScopedCancellationSignal::ScopedCancellationSignal(TCancellationSignal signal,
                                                   cancel_type type)
    : m_signal(signal),
      m_type(type) {
}

ScopedCancellationSignal::~ScopedCancellationSignal() {
    m_signal->emit(m_type);
}

TCancellationSignal ScopedCancellationSignal::signal() const {
    return m_signal;
}
