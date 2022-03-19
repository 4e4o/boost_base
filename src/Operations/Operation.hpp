#ifndef OPERATION_HPP
#define OPERATION_HPP

#include "Misc/StrandHolder.hpp"

#include <boost/signals2/signal.hpp>

class Operation : public StrandHolder {
public:
    Operation(boost::asio::io_context&);
    ~Operation();

    boost::signals2::signal<void()> starting;
    boost::signals2::signal<void(bool)> completed;

    void start();

protected:
    virtual void startImpl() = 0;
};

#endif // OPERATION_HPP
