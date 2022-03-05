#ifndef SERVER_H
#define SERVER_H

#include <boost/signals2.hpp>

#include "Network/TCPSocket.hpp"
#include "Misc/StrandHolder.hpp"

template<class Session>
class Server : public StrandHolder {
public:
    using Acceptor = boost::asio::ip::tcp::acceptor;

    Server(boost::asio::io_context &io)
        : StrandHolder(io),
          m_acceptor(io),
          m_socket(io),
          m_stopped(false) {
    }

    void start(const std::string& ip, unsigned short port) {
        m_acceptor = Acceptor(io(), boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(ip), port));
        doAccept();
    }

    void stop() {
        post([this]() {
            if (m_stopped)
                return;

            m_stopped = true;
            m_acceptor.cancel();
            closeAllSessions();
        });
    }

    typedef std::function<void(Session*)> TSessionInitEvent;

    void setSessionInit(TSessionInitEvent&& e) {
        m_sessionInit = std::move(e);
    }

private:
    void doAccept() {
        m_acceptor.async_accept(m_socket,
                                strand().wrap([this](const boost::system::error_code& ec) {
            if (m_stopped)
                return;

            if (!ec) {
                auto session = std::make_shared<Session>(io(), std::move(TCPSocket(std::move(m_socket))));

                if (m_sessionInit)
                    m_sessionInit(session.get());

                closeAllSessions.connect(typename decltype(closeAllSessions)::slot_type(
                                             &Session::close, session.get()).track_foreign(session));

                session->start();
            }

            this->doAccept();
        }));
    }

    boost::signals2::signal<void()> closeAllSessions;
    Acceptor m_acceptor;
    boost::asio::ip::tcp::socket m_socket;
    bool m_stopped;
    TSessionInitEvent m_sessionInit;
};

#endif // SERVER_H
