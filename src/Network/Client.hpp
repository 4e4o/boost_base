#ifndef CLIENT_H
#define CLIENT_H

#include <boost/asio/steady_timer.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>
#include <boost/signals2.hpp>

#include <memory>

#include "Misc/StrandHolder.hpp"

class Session;

class Client : public std::enable_shared_from_this<Client>, public StrandHolder {
public:
    Client(boost::asio::io_context &io);
    virtual ~Client();

    void setSession(std::shared_ptr<Session>);
    std::shared_ptr<Session> session() const;

    void connect(const std::string& ip, unsigned short port);

    boost::signals2::signal<void (bool)> onConnect;

private:
    static constexpr int CONNECT_TIMEOUT_SEC = 10;

    boost::asio::steady_timer m_connectTimer;
    std::shared_ptr<Session> m_session;
};

#endif // CLIENT_H
