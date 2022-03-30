#ifndef AWAITABLES_HPP
#define AWAITABLES_HPP

#include <boost/asio/awaitable.hpp>

typedef boost::asio::awaitable<int> TAwaitInt;
typedef boost::asio::awaitable<void> TAwaitVoid;
typedef boost::asio::awaitable<bool> TAwaitBool;
typedef boost::asio::awaitable<std::size_t> TAwaitSize;

#endif // AWAITABLES_HPP

