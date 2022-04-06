#ifndef CANCELLABLE_SPAWN_HPP
#define CANCELLABLE_SPAWN_HPP

#include "Misc/CallableTraits.hpp"
#include "CancellationSignal.hpp"

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/bind_cancellation_slot.hpp>

template <typename Executor, typename Callable, typename CompletionToken>
auto cancellable_spawn(Executor ex, Callable&& callable, CompletionToken&& token,
                       TCancellationSignal cancellation = TCancellationSignal()) {
    using namespace boost::asio;

    typedef typename CallableTraits<Callable>::result_type TCallableResult;
    typedef typename detail::awaitable_signature<TCallableResult>::type TSpawnHandlerSignature;

    if (cancellation == nullptr) {
        cancellation.reset(new TSCancellationSignal(ex));
    }

    // если spawn выполняется в ex executor то и сигнал отмены должен вызываться
    // в ex executor контексте
    assert(cancellation->executor() == ex);

    auto initiation = [cancellation](auto&& handler, Executor ex, Callable&& callable) {
        auto slot = get_associated_cancellation_slot(handler);

        // для co_spawn которые были вызваны с bind_cancellation_slot сигналы отмены не распространяются
        // автоматически во вложенные co_spawn, нужно руками связывать.
        // Ставим слот отмены только если handler слот соединён.

        if (slot.is_connected()) {
            slot.assign([cancellation](auto type) {
                cancellation->emit(type);
            });
        }

        auto callableWrapper = [cancellation, callable = std::move(callable)]() mutable -> TCallableResult {
            if constexpr (CallableTraits<Callable>::arity == 0) {
                co_return co_await callable();
            } else {
                co_return co_await callable(cancellation);
            }
        };

        auto cancellable_handler = bind_cancellation_slot(cancellation->signal()->slot(), std::move(handler));
        return boost::asio::co_spawn(ex, std::move(callableWrapper), std::move(cancellable_handler));
    };

    return boost::asio::async_initiate<
            CompletionToken, TSpawnHandlerSignature>(
                initiation, token, ex, std::move(callable));
}

#endif /* CANCELLABLE_SPAWN_HPP */
