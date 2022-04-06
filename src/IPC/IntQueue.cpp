#include "IntQueue.h"

using namespace boost::interprocess;

IPCIntQueue::IPCIntQueue(const std::string& name, bool child, size_t size)
    : m_parent(!child), m_name(name) {
    if (child) {
        m_queue.reset(new message_queue(open_only, m_name.c_str()));
    } else {
        message_queue::remove(m_name.c_str());
        m_queue.reset(new message_queue(create_only, m_name.c_str(), size, sizeof(int)));
    }
}

IPCIntQueue::~IPCIntQueue() {
    if (m_parent)
        message_queue::remove(m_name.c_str());
}

bool IPCIntQueue::try_send(int msg) {
    return m_queue->try_send(&msg, sizeof(msg), 0);
}

bool IPCIntQueue::try_receive(int &msg) {
    message_queue::size_type recvd_size;
    unsigned int priority;
    bool received = m_queue->try_receive(&msg, sizeof(msg), recvd_size, priority);

    if (received && (recvd_size == sizeof(msg)))
        return true;

    return false;
}
