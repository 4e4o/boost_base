#ifndef IPC_INT_QUEUE_H
#define IPC_INT_QUEUE_H

#include <boost/interprocess/ipc/message_queue.hpp>

class IPCIntQueue {
public:
    IPCIntQueue(const std::string&, bool child = true);
    ~IPCIntQueue();

    void send(int);
    int receive();

private:
    std::unique_ptr<boost::interprocess::message_queue> m_queue;
    bool m_parent;
    const std::string m_name;
};

#endif /* IPC_INT_QUEUE_H */
