#include "Operation.hpp"
#include "Misc/Debug.hpp"

Operation::Operation(boost::asio::io_context &io)
    : StrandHolder(io) {
    //    debug_print(boost::format("Operation::Operation %1%") % this);
}

Operation::~Operation() {
    //    debug_print(boost::format("Operation::~Operation %1%") % this);
}

void Operation::start() {
    starting();
    startImpl();
}
