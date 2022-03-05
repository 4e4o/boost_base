#ifndef CLIENT_MANAGER_HPP
#define CLIENT_MANAGER_HPP

#include "Misc/EnableSharedFromThisVirtual.hpp"
#include "Misc/GenericFactory.hpp"
#include "Misc/Timer.hpp"

class Session;

class ClientManager :
        public enable_shared_from_this_virtual<ClientManager>,
        public GenericFactory<Session>,
        public Timer {
public:
    ClientManager(boost::asio::io_context&, const Timer::TSec& = boost::none);
    ~ClientManager();

    void start(const std::string& ip, unsigned short port, const Timer::TSec& = boost::none);
    void stop();

    boost::signals2::signal<void(Session*)> onNewSession;

    bool connected() const;

private:
    void onReconnectTick();
    void startImpl();
    void postStart();

    bool m_stopped;
    std::string m_ip;
    unsigned short m_port;
    Timer::TSec m_connectTimeout;
    boost::signals2::signal<void()> m_close;
    std::weak_ptr<Session> m_curSession;
};

#endif /* CLIENT_MANAGER_HPP */
