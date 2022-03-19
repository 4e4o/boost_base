#ifndef CLIENT_H
#define CLIENT_H

#include "Misc/TimeDuration.hpp"
#include "Misc/StrandHolder.hpp"
#include "Misc/GenericFactory.hpp"
#include "Misc/TimerForward.hpp"
#include "Network/Session/SessionForward.hpp"

#include <boost/signals2/signal.hpp>

class Client : public StrandHolder, public GenericFactory<StrandHolder> {
public:
    Client(boost::asio::io_context &io, const TTimeDuration& = boost::none);
    virtual ~Client();

    void start(const std::string& ip, unsigned short port, const TTimeDuration& = boost::none);
    void stop();

    bool connected() const;

    void enableSSL();

    boost::signals2::signal<void(TWSession)> newSession;

private:
    void run();
    void restart();
    void onSession(TSession);

    enum class State {
        INITIAL,
        STARTED,
        STOPPED
    };

    State m_state;
    std::string m_ip;
    unsigned short m_port;
    TTimer m_connectTimer;
    TTimer m_restartTimer;
    TWSession m_session;
    boost::signals2::signal<void()> m_close;
};

#endif // CLIENT_H
