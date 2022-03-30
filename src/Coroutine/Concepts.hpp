#ifndef COROUTINE_CONCEPTS_HPP
#define COROUTINE_CONCEPTS_HPP

#include <boost/asio/awaitable.hpp>
#include <boost/asio/experimental/deferred.hpp>

template<class T>
concept Awaitable = requires(T&& x) {
    { boost::asio::awaitable(std::move(x)) } -> std::same_as<T>;
};

template<class T>
concept DeferredAsyncOp = requires(T&& x) {
    { boost::asio::experimental::deferred_async_operation(std::move(x)) } -> std::same_as<T>;
};

#endif // COROUTINE_CONCEPTS_HPP

