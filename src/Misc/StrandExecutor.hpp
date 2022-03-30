#ifndef STRAND_EXECUTOR_HPP
#define STRAND_EXECUTOR_HPP

#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>

typedef boost::asio::strand<boost::asio::io_context::executor_type> TStrandExecutor;

#endif /* STRAND_EXECUTOR_HPP */
