#include "SessionStartOperation.hpp"
#include "Network/Session/Session.hpp"
#include "Network/Socket/Socket.hpp"

using boost::system::error_code;

void SessionStartOperation::startImpl() {
    SessionOperation::startImpl();
    Socket* sock = socket();

    if (sock->started()) {
        post([this] {
            finish(true);
        });
    } else {
        sock->async_start([this](const error_code& ec) {
            finish(!ec);
        });
    }
}
