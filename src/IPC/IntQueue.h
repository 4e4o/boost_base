#ifndef IPC_INT_QUEUE_H
#define IPC_INT_QUEUE_H

#include <boost/interprocess/ipc/message_queue.hpp>

class IPCIntQueue {
public:
    IPCIntQueue(const std::string&, bool child = true, size_t size = DEFAULT_QUEUE_SIZE);
    ~IPCIntQueue();

    bool try_send(int);
    bool try_receive(int&);

private:
    static constexpr size_t DEFAULT_QUEUE_SIZE = 1024 * 10;

    std::unique_ptr<boost::interprocess::message_queue> m_queue;
    bool m_parent;
    const std::string m_name;
};

#endif /* IPC_INT_QUEUE_H */

