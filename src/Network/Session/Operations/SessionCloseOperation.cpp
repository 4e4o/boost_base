#include "SessionCloseOperation.hpp"
#include "Network/Session/Session.hpp"
#include "Network/Socket/Socket.hpp"

using boost::system::error_code;

void SessionCloseOperation::startImpl() {
    SessionOperation::startImpl();
    Socket* sock = socket();
    sock->async_close([this] (const error_code&) {
        finish(true);
    });
}
