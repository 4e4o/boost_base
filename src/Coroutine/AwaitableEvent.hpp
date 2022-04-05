#ifndef AWAITABLE_EVENT_HPP
#define AWAITABLE_EVENT_HPP

#include "Awaitables.hpp"
#include "AwaitableEventForward.hpp"
#include "Misc/IntrusiveListHelpers.hpp"
#include "Misc/StrandExecutor.hpp"

#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/async_result.hpp>

#include <mutex>
#include <list>

// юзать только через std::shared_ptr !

class AwaitableEvent : public std::enable_shared_from_this<AwaitableEvent> {
public:
    AwaitableEvent(const TStrandExecutor&);
    ~AwaitableEvent();

    static constexpr auto Token = boost::asio::use_awaitable;

    typedef void(THandlerSignature)(const boost::system::error_code&);
    typedef decltype(Token) TToken;
    typedef boost::asio::async_result<std::decay_t<TToken>, THandlerSignature> TAsyncResult;
    typedef TAsyncResult::return_type TReturn;

    void set();
    TReturn co_wait();

private:
    typedef TAsyncResult::handler_type THandler;
    typedef std::list<THandler> THandlers;

    struct TOperation : public TAutoUnlinkBaseListHook,
            public std::enable_shared_from_this<TOperation> {
        TOperation(const TStrandExecutor&, THandler&& h);
        ~TOperation();
        void finalize(const boost::system::error_code&);
        THandler m_handler;
        bool m_finalizing;
        TStrandExecutor m_executor;
    };

    typedef TIntrusiveListAutoUnlink<TOperation> TOperations;

    bool m_set;
    std::mutex m_mutex;
    TOperations m_operations;
    TStrandExecutor m_executor;
};

#endif /* AWAITABLE_EVENT_HPP */
