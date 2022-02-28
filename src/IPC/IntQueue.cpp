#include "IntQueue.h"

#define QUEUE_SIZE  1024 * 10

using namespace boost::interprocess;

IPCIntQueue::IPCIntQueue(const std::string& name, bool child)
    : m_parent(!child), m_name(name) {
    if (child) {
        m_queue.reset(new message_queue(open_only, m_name.c_str()));
    } else {
        message_queue::remove(m_name.c_str());
        m_queue.reset(new message_queue(create_only, m_name.c_str(), QUEUE_SIZE, sizeof(int)));
    }
}

IPCIntQueue::~IPCIntQueue() {
    if (m_parent)
        message_queue::remove(m_name.c_str());
}

void IPCIntQueue::send(int msg) {
    m_queue->send(&msg, sizeof(msg), 0);
}

int IPCIntQueue::receive() {
    message_queue::size_type recvd_size;
    unsigned int priority;
    int msg;
    m_queue->receive(&msg, sizeof(msg), recvd_size, priority);

    if(recvd_size != sizeof(msg))
        return -1;

    return msg;
}
