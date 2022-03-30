#ifndef IO_CONTEXT_HOLDER_HPP
#define IO_CONTEXT_HOLDER_HPP

#include <boost/asio/io_context.hpp>

class IOContextHolder {
public:
    IOContextHolder(boost::asio::io_context& io)
        : m_io(io) {
    }

    virtual ~IOContextHolder() {
    }

    boost::asio::io_context& io() const {
        return m_io;
    }

private:
    boost::asio::io_context& m_io;
};

#endif /* IO_CONTEXT_HOLDER_HPP */
