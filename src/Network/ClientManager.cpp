#include "ClientManager.hpp"

#include "Network/Client.hpp"
#include "Network/Session.hpp"
#include "Misc/Debug.hpp"

using boost::signals2::connection;

ClientManager::ClientManager(boost::asio::io_context& io, const Timer::TSec& reconnectSec)
    : Timer(io, reconnectSec),
      m_stopped(false) {
    debug_print(boost::format("ClientManager::ClientManager %1%") % this);
    onTimeout.connect([this](Timer*) {
        onReconnectTick();
    });
}

ClientManager::~ClientManager() {
    debug_print(boost::format("ClientManager::~ClientManager %1%") % this);
}

bool ClientManager::connected() const {
    STRAND_ASSERT(this);
    auto sp = m_curSession.lock();

    if (!sp)
        return false;

    return sp->started();
}

void ClientManager::onReconnectTick() {
    if (m_stopped)
        return;

    postStart();
}

void ClientManager::start(const std::string& ip, unsigned short port, const Timer::TSec& sec) {
    auto self = shared_from_this();
    post([self, ip, port, sec] {
        self->m_ip = ip;
        self->m_port = port;
        self->m_connectTimeout = sec;
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
    if (m_stopped)
        return;

    std::shared_ptr<Session> session(create<Session>(io()));
    std::shared_ptr<Client> client(new Client(io()));
    client->setSession(session);

    session->setStrand(this, false);
    client->setStrand(this, false);

    m_curSession = session;
    onNewSession(session.get());

    m_close.connect(decltype(m_close)::slot_type(
                        &Session::close, session.get()).track_foreign(session));

    auto self = shared_from_this();

    session->onDestroy.connect_extended([self](const connection &c, bool) {
        c.disconnect();

        self->post([self] {
            if (self->m_stopped)
                return;

            self->startTimer();

            if (self->sec().has_value()) {
                debug_print(boost::format("ClientManager reconnect started %1% %2% sec") % self.get() % self->sec().value());
            } else {
                debug_print(boost::format("ClientManager reconnect not set %1%") % self.get());
            }
        });
    });

    client->connect(m_ip, m_port, m_connectTimeout);
}

void ClientManager::stop() {
    auto self = shared_from_this();
    post([self] {
        if (self->m_stopped)
            return;

        self->m_stopped = true;
        self->m_close();
        self->stopTimer();
    });
}
