#include "ProxyDataSession.hpp"
#include "Network/Session/Operations/SessionReader.hpp"
#include "Network/Session/Operations/SessionWriter.hpp"
#include "Misc/ScopeGuard.hpp"
#include "Misc/Debug.hpp"

using namespace std::literals::chrono_literals;

#define ACTIVITY_TIMEOUT  10s

// FIXME increase read buffer ?

ProxyDataSession::ProxyDataSession(Socket* s)
    : Session(s),
      m_activityDetect(false),
      m_activity(false) {
    debug_print_this("");
}

ProxyDataSession::~ProxyDataSession() {
    debug_print_this("");
}

void ProxyDataSession::setOther(TProxyDataSession o) {
    m_other = o;
}

void ProxyDataSession::otherDone() {
    spawn<true>([this]() -> TAwaitVoid {
        // другой остановился.
        // сразу не останавливаемся сами
        // определяем активность цикла пересылки и закрываемся при неактивном -
        // чтоб не оборвать передачу на может быть еще живой другой конец.
        // (другой может остановиться по экзепшну на записи но сам еще может
        // быть писабельный)

        // активируем детектор активности
        m_activityDetect = true;
        m_activity = false;

        while(running()) {
            co_await wait(ACTIVITY_TIMEOUT);

            // no activity
            if (!m_activity) {
                stop();
                break;
            }

            m_activity = false;
        }
    });
}

TAwaitVoid ProxyDataSession::work() {
    debug_print_this("start");

    TProxyDataSession other = m_other.lock();

    if (other == nullptr) {
        throwGenericCoroutineError();
    }

    ScopeGuard stopOther([other] {
        other->otherDone();
    });

    while(running()) {
        STRAND_ASSERT(this);
        auto size = co_await reader().some();
        STRAND_ASSERT(this);
        co_await other->writer().all(reader().ptr(), size);

        if (m_activityDetect) {
            m_activity = true;
        }
    }

    //debug_print_this("end");
    co_return;
}
