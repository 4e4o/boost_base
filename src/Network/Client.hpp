#ifndef CLIENT_H
#define CLIENT_H

#include "Misc/Timer.hpp"

#include <boost/signals2.hpp>

#include <memory>

class Session;

class Client : public std::enable_shared_from_this<Client>, public Timer {
public:
    Client(boost::asio::io_context &io);
    virtual ~Client();

    void setSession(std::shared_ptr<Session>);
    std::shared_ptr<Session> session() const;

    void connect(const std::string& ip, unsigned short port, const Timer::TSec& = boost::none);

private:
    static constexpr int DEFAULT_CONNECT_TIMEOUT_SEC = 10;

    std::shared_ptr<Session> m_session;
};

#endif // CLIENT_H
