#include "Client.hpp"
#include "Network/Session.hpp"
#include "Misc/Debug.hpp"

#include <boost/asio/ip/tcp.hpp>

Client::Client(boost::asio::io_context &io)
    : Timer(io, DEFAULT_CONNECT_TIMEOUT_SEC) {
    debug_print(boost::format("Client::Client %1%") % this);
    onTimeout.connect([this] (Timer*) {
        m_session->close();
    });
}

Client::~Client() {
    debug_print(boost::format("Client::~Client %1%") % this);
}

void Client::setSession(std::shared_ptr<Session> c) {
    m_session = c;
}

std::shared_ptr<Session> Client::session() const {
    return m_session;
}

void Client::connect(const std::string& ip, unsigned short port, const Timer::TSec& connectTimeout) {
    if (m_session == nullptr)
        return;

    using namespace boost::asio::ip;
    using boost::system::error_code;

    tcp::endpoint endpoint(address_v4::from_string(ip), port);
    auto self = shared_from_this();

    m_session->socket().async_connect(endpoint,
                                      strand().wrap([self](const error_code& ec) {
        self->stopTimer();
        debug_print(boost::format("Client::connect result %1% %2%") % self.get() % !ec);

        if (!ec) {
            self->m_session->start();
        }
    }));

    startTimer(connectTimeout);
    debug_print(boost::format("Client::connect started %1% %2% sec") % this % sec().value());
}
