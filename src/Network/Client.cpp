#include "Client.hpp"
#include "Network/Session/Session.hpp"
#include "Network/Socket/TCP/SSLSocket.hpp"
#include "Misc/Debug.hpp"
#include "Misc/Timer.hpp"

#include <boost/asio/ip/tcp.hpp>

#define CONNECT_TIMEOUT     TSeconds(10)

using namespace boost::asio;
using boost::system::error_code;

Client::Client(io_context &io, const TTimeDuration& reconnect)
    : StrandHolder(io),
      m_state(State::INITIAL),
      m_connectTimer(new Timer(io, CONNECT_TIMEOUT)),
      m_restartTimer(new Timer(io, reconnect))  {
    debug_print(boost::format("Client::Client %1%") % this);

    registerDefaultType<Session, Socket*>();
    registerDefaultType<TCPSocket, io_context&, ip::tcp::socket&&>();

    m_connectTimer->setStrand(this);
    m_restartTimer->setStrand(this);

    m_connectTimer->timeout.connect([this](auto) { m_close(); });
    m_restartTimer->timeout.connect([this](auto) { run(); });
}

Client::~Client() {
    debug_print(boost::format("Client::~Client %1%") % this);
}

void Client::enableSSL() {
    registerType<TCPSocket, SSLSocket, io_context&, ip::tcp::socket&&>();
}

void Client::start(const std::string& ip, unsigned short port, const TTimeDuration& d) {
    post<true>([this, ip, port, d] {
        if (m_state != State::INITIAL)
            return;

        m_state = State::STARTED;
        m_ip = ip;
        m_port = port;

        if (d.has_value()) {
            m_connectTimer->setDuration(d);
        }

        m_connectTimer->lockWhenActive(weak_from_this());
        m_restartTimer->lockWhenActive(weak_from_this());

        run();
    });
}

void Client::stop() {
    post<true>([this] {
        if (m_state == State::STOPPED)
            return;

        m_state = State::STOPPED;
        m_restartTimer->stopTimer();
        m_connectTimer->stopTimer();
        m_close();
    });
}

bool Client::connected() const {
    STRAND_ASSERT(this);
    return m_session.lock() != nullptr;
}

void Client::run() {
    STRAND_ASSERT(this);

    if (m_state != State::STARTED)
        return;

    ip::tcp::endpoint endpoint(ip::make_address_v4(m_ip), m_port);
    std::shared_ptr<ip::tcp::socket> socket(new ip::tcp::socket(io()));

    m_close.disconnect_all_slots();

    m_close.connect([socket] {
        error_code ec1, ec2;
        socket->shutdown(ip::tcp::socket::shutdown_both, ec1);
        socket->close(ec2);
    });

    auto self = shared_from_this();
    socket->async_connect(endpoint,
                          bindExecutor([self, this, socket](const error_code& ec) {
        debug_print(boost::format("Client::async_connect result %1% %2%") % this % !ec);
        m_connectTimer->stopTimer();

        if (ec) {
            restart();
        } else {
            TCPSocket *sock = create<TCPSocket, io_context&, ip::tcp::socket&&>(io(), std::move(*socket));
            TSession session(create<Session, Socket*>(sock));
            onSession(session);
        }
    }));

    m_connectTimer->startTimer();
    debug_print(boost::format("Client::start started %1%, %2%") % this % m_connectTimer->duration().value());
}

void Client::onSession(TSession session) {
    // Если рестарт включен, то Client не умрёт и будет ждать пока сессия не умрёт,
    // потом начнёт реконнекты.
    // Если рестарт не включен, то Client умрёт после коннекта
    if (m_restartTimer->duration().has_value()) {
        Lifecycle::connectTrack(m_close, session, &Session::close);
        m_session = session;
        auto self = shared_from_this();
        session->closed.connect([self, this] {
            post<true>([this] {
                restart();
            });
        });
    } else {
        // FIXME, remove it after session crash fix
        // stop();
    }

    newSession(session);
    session->start();
}

void Client::restart() {
    STRAND_ASSERT(this);

    if (m_state != State::STARTED)
        return;

    if (m_restartTimer->startTimer()) {
        debug_print(boost::format("Client::restart started %1%, %2%") % this % m_restartTimer->duration().value());
    } else {
        debug_print(boost::format("Client::restart not set, stopping %1%") % this);
        stop();
    }
}
