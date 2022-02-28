#include "QueuedSessionWriter.hpp"
#include "Network/Session.hpp"
#include "Misc/Debug.hpp"

QueuedSessionWriter::QueuedSessionWriter(Session *s)
    : m_session(s), m_writing(false) {
    m_writeDoneConnection = s->onWriteDone.connect([this] {
        onWriteDone();
    });
}

QueuedSessionWriter::~QueuedSessionWriter() {
    m_writeDoneConnection.disconnect();
}

void QueuedSessionWriter::writeAll(const uint8_t *ptr, std::size_t size) {
    m_queue.emplace(ptr, ptr + size);
    doWrite();
}

void QueuedSessionWriter::doWrite() {
    if (m_writing || m_queue.empty())
        return;

    m_writing = true;
    auto& ref = m_queue.front();
    //debug_print(boost::format("QueuedSessionWriter::doWrite %1% %2% %3%") % this % ref.size() % m_queue.size());
    m_session->writeAll(ref.data(), ref.size());
}

void QueuedSessionWriter::onWriteDone() {
    if (m_queue.empty())
        return;

    m_queue.pop();
    m_writing = false;
    //debug_print(boost::format("QueuedSessionWriter::onWriteDone %1% %2%") % this % m_queue.size());
    doWrite();
}
