#include "AwaitableEvent.hpp"

#include <boost/asio/associated_cancellation_slot.hpp>

using namespace boost::system;
using namespace boost::asio;

AwaitableEvent::AwaitableEvent(const TStrandExecutor& e)
    : m_set(false), m_executor(e) {
}

AwaitableEvent::~AwaitableEvent() {
    std::lock_guard l(m_mutex);
    // если тут словили этот ассерт значит есть неоконченные операции
    // а это ошибка юзания AwaitableEvent, которая может быть из-за:
    // 1. Не юзается shared_ptr при создании AwaitableEvent
    // 2. кто-то насильно удаляет AwaitableEvent пока у него есть незавершенные операции
    assert(m_operations.empty());
}

void AwaitableEvent::set() {
    std::lock_guard l(m_mutex);
    m_set = true;

    m_operations.clear_and_dispose([](TOperation *op) {
        op->finalize(error_code());
    });
}

AwaitableEvent::TReturn AwaitableEvent::co_wait() {
    auto initiate = [this](auto&& handler) mutable {
        std::lock_guard l(m_mutex);
        associated_cancellation_slot_t<THandler> slot = get_associated_cancellation_slot(handler);
        std::shared_ptr<TOperation> op(new TOperation(m_executor, std::move(handler)));

        if (m_set) {
            op->finalize(error_code());
        } else {
            m_operations.push_back(*op);
            auto self = shared_from_this();
            slot.assign([self, this, op](auto) {
                std::lock_guard l(m_mutex);
                op->finalize(errc::make_error_code(errc::operation_canceled));
            });
        }
    };

    co_return co_await boost::asio::async_initiate<TToken, THandlerSignature>(initiate, use_awaitable);
}

AwaitableEvent::TOperation::TOperation(const TStrandExecutor& e, THandler&& h)
    : m_handler(std::move(h)),
      m_finalizing(false),
      m_executor(e) {
}

AwaitableEvent::TOperation::~TOperation() {
}

void AwaitableEvent::TOperation::finalize(const boost::system::error_code& ec) {
    if (m_finalizing) {
        return;
    }

    unlink();
    m_finalizing = true;

    auto self = shared_from_this();
    post(m_executor, [self, this, ec]() mutable {
        std::move(m_handler)(ec);
    });
}
