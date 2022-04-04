#include "ProxyDataSession.hpp"
#include "Network/Session/Operations/SessionReader.hpp"
#include "Network/Session/Operations/SessionWriter.hpp"
#include "Misc/ScopeGuard.hpp"
#include "Misc/Debug.hpp"

using namespace boost::system;

ProxyDataSession::ProxyDataSession(Socket* s)
    : Session(s),
      m_state(State::READ),
      m_otherDone(false) {
    debug_print_this("");

    // Не закрываем сокет после завершения работы текущей сессии,
    // так как другая сессия всё ещё может писать в текущий сокет.
    // Закрываем сокеты позже.
    setSocketAutoClose(false);
}

ProxyDataSession::~ProxyDataSession() {
    debug_print_this("");
}

void ProxyDataSession::setOther(TProxyDataSession o) {
    m_other = o;
}

TAwaitVoid ProxyDataSession::onStop() {
    // в случае стопа сразу стопаем другую сессию
    TProxyDataSession other = m_other.lock();

    if (other) {
        other->stop();
    }

    co_return;
}

void ProxyDataSession::otherDone(State lastState) {
    post<true>([this, lastState]() {
        if (lastState == State::READ) {
            debug_print_this("done on READ, stopping...");
            // другой умер на чтении
            // значит другой сокет уже неписабельный, просто стопаемся
            stop();
        } else {
            debug_print_this("done on WRITE, nothing to do...");
            // другой умер на записи
            // другой сокет всё еще может быть писабельный
            // а этот уже не читаемый, так что текущий не зависнет на чтении
            // ничё не делаем, пусть запись завершается если была таковая
        }

        m_otherDone = true;
    });
}

TAwaitVoid ProxyDataSession::work() {
    debug_print_this("start");

    TProxyDataSession other = m_other.lock();

    if (other == nullptr) {
        throwGenericCoroutineError();
    }

    ScopeGuard sg([this, other]() {
        // мы последние завершились
        if (m_otherDone) {
            debug_print_this("we are last, closing sockets");
            // закрываем сокеты
            // так как автоматически они не будут закрыты
            this->closeSocket();
            other->closeSocket();
        } else {
            // говорим другой сессии что мы завершились
            debug_print_this("notify other we are done");
            other->otherDone(m_state);
        }
    });

    while(running() && !m_otherDone) {
        STRAND_ASSERT(this);
        m_state = State::READ;
        auto size = co_await reader().some();
        m_state = State::WRITE;
        STRAND_ASSERT(this);
        co_await other->writer().all(reader().ptr(), size);
    }

    co_return;
}
