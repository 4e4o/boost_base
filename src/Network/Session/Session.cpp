#include "Session.hpp"
#include "Network/Socket/Socket.hpp"
#include "Operations/SessionReader.hpp"
#include "Operations/SessionWriter.hpp"
#include "Misc/Debug.hpp"

using namespace std::literals::chrono_literals;

#define START_TIMEOUT   10s
#define CLOSE_TIMEOUT   5s

using namespace boost::asio;

INIT_DEBUG_OBJECTS_COUNT(sessions)

Session::Session(Socket *s)
    : CoroutineTask(s->io()),
      m_reader(new SessionReader(this)),
      m_writer(new SessionWriter(this)) {
    DEBUG_OBJECTS_COUNT_INC(sessions);

    setSocket(s);
    setTimeout(Timeout::START, START_TIMEOUT);
    setTimeout(Timeout::CLOSE, CLOSE_TIMEOUT);
}

Session::~Session() {
    DEBUG_OBJECTS_COUNT_DEC(sessions);
}

Socket* Session::releaseSocket() {
    Socket *sock = m_socket.release();
    setSocket(sock->create(io()));
    return sock;
}

void Session::setSocket(Socket* s) {
    m_socket.reset(s);
}

SessionReader& Session::reader() const {
    return *m_reader;
}

SessionWriter& Session::writer() const {
    return *m_writer;
}

TAwaitVoid Session::prepare() {
    co_return;
}

TAwaitVoid Session::work() {
    co_return;
}

TAwaitVoid Session::run() {
    debug_print_this("start");

    // FIXME scoped auto close ?

    try {
        co_await prepare();

        if (!socket()->started()) {
            co_await timeout(socket()->co_start(), getTimeout(Timeout::START));
        }

        co_await work();
    } catch(const std::exception& ex1) {
        debug_print_this(fmt("exception %1%") % ex1.what());
        throw ex1;
    }

    co_return;
}

TAwaitVoid Session::onStop() {
    // FIXME !
    co_return ;//co_await socket()->co_close();
}
