#ifndef COROUTINE_SPAWN_HPP
#define COROUTINE_SPAWN_HPP

#include "Misc/StrandHolder.hpp"
#include "CancellableSpawn.hpp"

#include <boost/asio/detached.hpp>
//#include <boost/asio/use_awaitable.hpp>
//#include <boost/asio/experimental/deferred.hpp>

class CoroutineSpawn : public StrandHolder {
public:
    using StrandHolder::StrandHolder;

    template <bool SelfLock = false, typename CompletionToken = decltype(boost::asio::detached), class Callable>
    auto spawn(Callable&& c, CompletionToken ct = boost::asio::detached) const {
        return cancellable_spawn(executor(), autoLockHandler<SelfLock>(std::forward<Callable>(c)), ct);
    }

    /*
    template <bool SelfLock = false, class Callable>
    auto spawn(Callable&& c) const {
        return spawn<SelfLock>(std::forward<Callable>(c), boost::asio::detached);
    }

    template <bool SelfLock = false, class Callable>
    auto spawn(Callable&& c, boost::asio::detached_t) const {
        return cancellable_spawn(executor(), autoLockHandler<SelfLock>(std::forward<Callable>(c)), boost::asio::detached);
    }

    template <bool SelfLock = false, class Callable>
    auto spawn(Callable&& c, boost::asio::use_awaitable_t<>) const {
        return cancellable_spawn(executor(), autoLockHandler<SelfLock>(std::forward<Callable>(c)), boost::asio::use_awaitable);
    }

    template <bool SelfLock = false, class Callable>
    auto spawn(Callable&& c, boost::asio::experimental::deferred_t) const {
        return cancellable_spawn(executor(), autoLockHandler<SelfLock>(std::forward<Callable>(c)), boost::asio::experimental::deferred);
    }*/
};

#endif /* COROUTINE_SPAWN_HPP */
