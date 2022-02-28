#include "ClientManager.hpp"

#include "Network/Client.hpp"
#include "Network/Session.hpp"
#include "Misc/Debug.hpp"

using boost::signals2::connection;

ClientManager::ClientManager(boost::asio::io_context& io, int reconnectSec)
    : Timer(io, reconnectSec), m_stopped(false) {
    debug_print(boost::format("ClientManager::ClientManager %1%") % this);
    onTimeout.connect([this](Timer*) {
        onReconnectTick();
    });
}

ClientManager::~ClientManager() {
    debug_print(boost::format("ClientManager::~ClientManager %1%") % this);
}

void ClientManager::onReconnectTick() {
    if (m_stopped)
        return;

    postStart();
}

void ClientManager::start(const std::string& ip, unsigned short port) {
    auto self = shared_from_this();
    post([self, ip, port] {
        self->m_ip = ip;
        self->m_port = port;
        self->postStart();
    });
}

void ClientManager::postStart() {
    auto self = shared_from_this();
    post([self] {
        self->startImpl();
    });
}

void ClientManager::startImpl() {
    std::shared_ptr<Session> session(create<Session>(io()));
    std::shared_ptr<Client> client(new Client(io()));
    client->setSession(session);

    session->setStrand(this, false);
    client->setStrand(this, false);

    onNewSession(session.get());

    m_closeClient.connect(decltype(m_closeClient)::slot_type(
                              &Session::close, session.get()).track_foreign(session));

    auto self = shared_from_this();

    session->onDestroy.connect_extended([self](const connection &c) {
        c.disconnect();

        self->post([self] {
            if (self->m_stopped)
                return;

            self->startTimer();
        });
    });

    client->onConnect.connect_extended([self, session](const connection &c, bool connected) {
        c.disconnect();
        STRAND_ASSERT(self.get());

        if (self->m_stopped)
            return;

        if (connected) {
            session->start();
        }
    });

    client->connect(m_ip, m_port);
}

void ClientManager::stop() {
    auto self = shared_from_this();
    post([self] {
        if (self->m_stopped)
            return;

        self->m_stopped = true;
        self->m_closeClient();
        self->stopTimer();
    });
}
