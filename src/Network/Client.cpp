#include "Client.hpp"
#include "Misc/Debug.hpp"
#include "Network/Session/Session.hpp"
#include "Network/Socket/TCP/SSLSocket.hpp"

#include <boost/asio/ip/tcp.hpp>

using namespace std::literals::chrono_literals;

#define CONNECT_TIMEOUT 10s

using namespace boost::asio;
using namespace boost::system;

Client::Client(boost::asio::io_context& io, const TDurationUnit& d)
    : Client(io, TTimeDuration(d)) {
}

Client::Client(io_context &io, const TTimeDuration& reconnect)
    : CoroutineTask(io),
      m_managedMode(reconnect.has_value()),
      m_connect(CONNECT_TIMEOUT),
      m_reconnect(reconnect) {
    debug_print_this("");

    registerDefaultType<Session, Socket*>();
    registerDefaultType<TCPSocket, io_context&, ip::tcp::socket&&>();
}

Client::~Client() {
    debug_print_this("");
}

void Client::enableSSL() {
    registerType<TCPSocket, SSLSocket, io_context&, ip::tcp::socket&&>();
}

void Client::setConnectTimeout(const TTimeDuration &connect) {
    m_connect = connect;
}

bool Client::connected() const {
    STRAND_ASSERT(this);
    return m_session.lock() != nullptr;
}

Client::TAwaitResult Client::run(const std::string& ip, unsigned short port) {
    STRAND_ASSERT(this);
    ip::tcp::endpoint endpoint(ip::make_address_v4(ip), port);

    while(running()) {
        ip::tcp::socket socket(io());
        bool connected = false;

        try {
            debug_print_this("connecting...");

            STRAND_ASSERT(this);
            //  co_await timeout(socket.async_connect(endpoint, deferred), m_connect);
            co_await timeout(socket.async_connect(endpoint, use_awaitable), m_connect);
            connected = true;
        } catch(const system_error& e) {
            debug_print_this(fmt("connect error = %1%") % e.what());

            // в случае отмены операции должны выкинуть исключение чтоб отмена клиента работала всегда
            if (e.code() == errc::operation_canceled) {
                throw;
            } else {
                // для других исключений смотрим режим клиента

                // в managedMode все ошибки допустимые, просто продолжаем попытки коннектов
                // в противном случае выкидываем полученное исключение
                if (!m_managedMode) {
                    throw;
                }
            }
        } catch(...) {
            if (!m_managedMode) {
                throw;
            }
        }

        STRAND_ASSERT(this);

        if (connected) {
            debug_print_this("connect success");
            TCPSocket *sock = create<TCPSocket, io_context&, ip::tcp::socket&&>(io(), std::move(socket));
            TSession session(create<Session, Socket*>(sock));

            if (m_managedMode) {
                m_session = session;
                newSession(m_session);

                ScopeGuard sg([this]() {
                    auto s = m_session;
                    // сбрасываем вручную вик поинтер чтоб
                    // для слотов sessionCompleted было верно: Client::connected == false
                    m_session.reset();
                    sessionCompleted(s);
                });

                // в менеджед случае надо все исключения сессии съедать и переподключаться до остановки
                try {
                    co_await session->co_start(use_awaitable);
                } catch(const system_error& e) {
                    // отмену не игнорируем, она только при стопе происходит
                    if (e.code() == errc::operation_canceled) {
                        throw;
                    }
                } catch(...) { }
            } else {
                co_return session;
            }
        }

        // не managedMode режим до сюда не доходит
        assert(m_managedMode);

        STRAND_ASSERT(this);
        debug_print_this(fmt("waiting for next attempt %1%ms") % m_reconnect->count());
        co_await wait(*m_reconnect);
        STRAND_ASSERT(this);
    }

    // чтоб пустую результат не возвращать выкидываем исключение
    throwGenericCoroutineError();
    co_return TSession(); // для подавления ворнинга
}
