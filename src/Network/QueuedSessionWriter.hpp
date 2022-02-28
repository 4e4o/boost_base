#ifndef QUEUED_SESSION_WRITER_H
#define QUEUED_SESSION_WRITER_H

#include <boost/signals2/signal.hpp>

#include <vector>
#include <queue>

#include <stdint.h>

class Session;

class QueuedSessionWriter {
public:
    typedef std::vector<uint8_t> TBuffer;

    QueuedSessionWriter(Session*);
    virtual ~QueuedSessionWriter();

    void writeAll(const uint8_t *ptr, std::size_t size);

private:
    void doWrite();
    void onWriteDone();

    Session* m_session;
    bool m_writing;
    std::queue<TBuffer> m_queue;
    boost::signals2::connection m_writeDoneConnection;
};

#endif // QUEUED_SESSION_WRITER_H
