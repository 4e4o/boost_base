#ifndef CANCELLABLE_SPAWN_HPP
#define CANCELLABLE_SPAWN_HPP

#include "Misc/CallableTraits.hpp"

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/bind_cancellation_slot.hpp>

typedef std::function<void()> TSpawnCancellation;

template <typename Executor, typename Callable, typename CompletionToken>
auto cancellable_spawn(Executor ex, Callable&& callable, CompletionToken&& token) {
    using namespace boost::asio;

    typedef typename CallableTraits<Callable>::result_type TCallableResult;
    typedef std::shared_ptr<cancellation_signal> TCancellationSignal;
    typedef typename detail::awaitable_signature<TCallableResult>::type TSpawnHandlerSignature;

    auto initiation = [](auto&& handler, Executor ex, Callable&& callable) {
        TCancellationSignal sig(new cancellation_signal());

        auto cancel = [sig, ex]() {
            dispatch(ex, [sig] {
                sig->emit(cancellation_type::all);
            });
        };

        auto slot = get_associated_cancellation_slot(handler);

        // для co_spawn которые были вызваны с bind_cancellation_slot сигналы отмены не распространяются
        // автоматически во вложенные co_spawn, нужно руками связывать.
        // Ставим слот отмены только если handler слот соединён.

        if (slot.is_connected()) {
            slot.assign([cancel](auto) {
                cancel();
            });
        }

        auto callableWrapper = [cancel, callable = std::move(callable)]() mutable -> TCallableResult {
            if constexpr (CallableTraits<Callable>::arity == 0) {
                co_return co_await callable();
            } else {
                co_return co_await callable(cancel);
            }
        };

        auto cancellable_handler = bind_cancellation_slot((*sig).slot(), std::move(handler));
        return boost::asio::co_spawn(ex, std::move(callableWrapper), std::move(cancellable_handler));
    };

    return boost::asio::async_initiate<
            CompletionToken, TSpawnHandlerSignature>(
                initiation, token, ex, std::move(callable));
}

#endif /* CANCELLABLE_SPAWN_HPP */
