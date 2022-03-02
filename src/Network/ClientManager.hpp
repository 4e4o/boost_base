#ifndef CLIENT_MANAGER_HPP
#define CLIENT_MANAGER_HPP

#include <boost/asio/io_context.hpp>

#include "Misc/Timer.hpp"
#include "Misc/EnableSharedFromThisVirtual.hpp"
#include "Misc/GenericFactory.hpp"

class Session;

class ClientManager
        : public enable_shared_from_this_virtual<ClientManager>,
        public GenericFactory<Session>,
        public Timer {
public:
    ClientManager(boost::asio::io_context&, int = 0);
    ~ClientManager();

    void start(const std::string& ip, unsigned short port);
    void stop();

    boost::signals2::signal<void(Session*)> onNewSession;

private:
    void onReconnectTick();
    void startImpl();
    void postStart();

    bool m_stopped;
    std::string m_ip;
    unsigned short m_port;
    boost::signals2::signal<void()> m_closeClient;
};

#endif /* CLIENT_MANAGER_HPP */
