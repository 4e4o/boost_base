#ifndef COROUTINE_SPAWN_HPP
#define COROUTINE_SPAWN_HPP

#include "Misc/StrandHolder.hpp"
#include "CancellableSpawn.hpp"

#include <boost/asio/detached.hpp>

class CoroutineSpawn : public StrandHolder {
public:
    using StrandHolder::StrandHolder;

    template <bool SelfLock = false, typename CompletionToken = decltype(boost::asio::detached), class Callable>
    auto spawn(Callable&& c, CompletionToken ct = boost::asio::detached,
               TCancellationSignal cancellation = TCancellationSignal()) const {
        return cancellable_spawn(executor(), autoLockHandler<SelfLock>(std::forward<Callable>(c)), ct, cancellation);
    }

    // spawn который отменится при выходе из скоупа
    template <bool SelfLock = false, class Callable>
    ScopedCancellationSignal scopedSpawn(Callable&& c) {
        TCancellationSignal signal(new TSCancellationSignal(executor()));
        spawn<SelfLock>(std::forward<Callable>(c), boost::asio::detached, signal);
        return {signal};
    }
};

#endif /* COROUTINE_SPAWN_HPP */
