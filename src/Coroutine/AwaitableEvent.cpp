#include "AwaitableEvent.hpp"
#include "Misc/Debug.hpp"

#include <boost/asio/associated_cancellation_slot.hpp>
#include <boost/asio/bind_cancellation_slot.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/steady_timer.hpp>

using namespace boost::system;
using namespace boost::asio;

using namespace std::literals::chrono_literals;

INIT_DEBUG_OBJECTS_COUNT(AwaitableEvent)
INIT_DEBUG_OBJECTS_COUNT(AE_TOperation)

AwaitableEvent::AwaitableEvent(const TStrandExecutor& e)
    : m_set(false), m_executor(e) {
    DEBUG_OBJECTS_COUNT_INC(AwaitableEvent)
}

AwaitableEvent::~AwaitableEvent() {
    DEBUG_OBJECTS_COUNT_DEC(AwaitableEvent)

    std::lock_guard l(m_mutex);
    // если тут словили этот ассерт значит есть неоконченные операции
    // а это ошибка юзания AwaitableEvent, которая может быть из-за:
    // 1. Не юзается shared_ptr при создании AwaitableEvent
    // 2. кто-то насильно удаляет AwaitableEvent пока у него есть незавершенные операции
    assert(m_operations.empty());
}

void AwaitableEvent::set() {
//    debug_print_this("start");

    std::lock_guard l(m_mutex);
    m_set = true;

    m_operations.clear_and_dispose([](TOperation *op) {
        op->finalize(error_code());
    });

//    debug_print_this("end");
}

TAwaitVoid AwaitableEvent::co_wait() {
//    debug_print_this("start");

    auto initiate = [this](auto&& handler) mutable {
        std::lock_guard l(m_mutex);
        associated_cancellation_slot_t<THandler> slot = get_associated_cancellation_slot(handler);
        std::shared_ptr<TOperation> op(new TOperation(m_executor, std::move(handler)));

//        debug_print_this(fmt("op created %1%") % op.get());

        if (m_set) {
            op->finalize(error_code());
        } else {
            m_operations.push_back(*op);
            auto self = shared_from_this();
            slot.assign([self, this, op](auto) {
//                debug_print_this(fmt("cancellation slot %1%") % op.get());
                std::lock_guard l(m_mutex);
                op->finalize(errc::make_error_code(errc::operation_canceled));
            });
//            debug_print_this("cancellation slot assigned");
        }
    };

    co_await boost::asio::async_initiate<TToken, THandlerSignature>(initiate, use_awaitable);
    co_return;
}

AwaitableEvent::TOperation::TOperation(const TStrandExecutor& e, THandler&& h)
    : m_handler(std::move(h)),
      m_finalizing(false),
      m_executor(e) {
    DEBUG_OBJECTS_COUNT_INC(AE_TOperation)
}

AwaitableEvent::TOperation::~TOperation() {
    DEBUG_OBJECTS_COUNT_DEC(AE_TOperation)
}

void AwaitableEvent::TOperation::finalize(const boost::system::error_code& ec) {
    if (m_finalizing) {
        debug_print_this("already finalizing, nothing to do");
        return;
    }

//    debug_print_this("");

    unlink();
    m_finalizing = true;

    auto self = shared_from_this();
    post(m_executor, [self, this, ec]() mutable {
//        debug_print_this("handler call start");
        std::move(m_handler)(ec);
//        debug_print_this("handler call end");
    });
}
