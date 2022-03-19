#ifndef SESSION_FORWARD_HPP
#define SESSION_FORWARD_HPP

#include <memory>

class Session;

typedef std::shared_ptr<Session> TSession;
typedef std::weak_ptr<Session> TWSession;

#endif /* SESSION_FORWARD_HPP */
