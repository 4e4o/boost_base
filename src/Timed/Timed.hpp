#ifndef TIMED_HPP
#define TIMED_HPP

// adapted from https://cppalliance.org/richard/2021/10/10/RichardsOctoberUpdate.html

#include "TimedError.hpp"
#include "Misc/TimeDuration.hpp"

#include <boost/asio/steady_timer.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/associated_cancellation_slot.hpp>
#include <boost/asio/experimental/parallel_group.hpp>

#include <chrono>

template <typename CompletionToken>
struct timed_token {
    boost::asio::steady_timer* timer;
    TDurationUnit timeout;
    CompletionToken& token;
};

template <typename... Signatures>
struct timed_initiation {
    template <
            typename CompletionHandler,
            typename Initiation,
            typename... InitArgs>
    void operator()(
            CompletionHandler handler,  // the generated completion handler
            boost::asio::steady_timer* timer,
            const TDurationUnit& timeout,      // the timeout specified in our completion token
            Initiation&& initiation,    // the embedded operation's initiation (e.g. async_read)
            InitArgs&&... init_args)    // the arguments passed to the embedded initiation (e.g. the async_read's buffer argument etc)
    {
        using boost::asio::experimental::make_parallel_group;
        using namespace boost::asio;
        using namespace boost::system;

        // locate the correct executor associated with the underling operation
        // first try the associated executor of the handler. If that doesn't
        // exist, take the associated executor of the underlying async operation's handler
        // If that doesn't exist, use the default executor (system executor currently)
        auto ex = get_associated_executor(handler,
                                          get_associated_executor(initiation));

        associated_cancellation_slot_t<CompletionHandler> slot = get_associated_cancellation_slot(handler);

        auto timerEx = timer->get_executor();

        assert(timerEx == ex);

        slot.assign([timer](auto) {
            timer->cancel();
        });

        auto timerOp = bind_executor(ex, [timer, timeout](auto&& token) {
            timer->expires_after(timeout);
            return timer->async_wait(std::forward<decltype(token)>(token));
        });

        auto originalOp = bind_executor(ex, [&](auto&& token) {
            // Finally, initiate the underlying operation
            // passing its original arguments
            return async_initiate<decltype(token), Signatures...>(
                        std::forward<Initiation>(initiation), token,
                        std::forward<InitArgs>(init_args)...);
        });

        auto groupCompletion = [handler = std::move(handler)] (
                // an array of indexes indicating in which order the group's
                // operations completed, whether successfully or not
                std::array<std::size_t, 2>,

                // The arguments are the result of concatenating
                // the completion handler arguments of all operations in the
                // group, in retained order:
                // first the steady_timer::async_wait
                const error_code& timer_ec,

                // then the underlying operation e.g. async_read(...)
                auto&&... op_args // e.g. error_code, size_t
                ) mutable {

            // если таймер отработал без ошибок - завершение с таймаутом
            if (!timer_ec) {
                auto tup = gen_timeout_error::gen(std::forward<decltype(op_args)>(op_args)...);
                std::apply(std::move(handler), tup);
                return;
            }

            // finally, invoke the handler with the results of the
            // underlying operation
            std::move(handler)(std::forward<decltype(op_args)>(op_args)...);
        };

        // launch a parallel group of asynchronous operations - one for the timer
        // wait and one for the underlying asynchronous operation (i.e. async_read)
        make_parallel_group(std::move(timerOp), std::move(originalOp)).async_wait(
                    // Wait for the first item in the group to complete. Upon completion
                    // of the first, cancel the others.
                    experimental::wait_for_one(),
                    std::move(groupCompletion));
    }
};

// Specialise the async_result primary template for our timed_token
template <typename InnerCompletionToken, typename... Signatures>
struct boost::asio::async_result<
        timed_token<InnerCompletionToken>,  // specialised on our token type
        Signatures...> {
    // The implementation will call initiate on our template class with the
    // following arguments:
    template <typename Initiation, typename... InitArgs>
    static auto initiate(
            Initiation&& init, // This is the object that we invoke in order to
            // actually start the packaged async operation
            timed_token<InnerCompletionToken> t, // This is the completion token that
            // was passed as the last argument to the initiating function
            InitArgs&&... init_args) // any more arguments that were passed to the initiating function
    {
        // we will customise the initiation through our token by invoking
        // async_initiate with our own custom function object called
        // timed_initiation. We will pass it the arguments that were passed to
        // timed(). We will also forward the initiation created by the underlying
        // completion token plus all arguments destined for the underlying
        // initiation.
        return asio::async_initiate<InnerCompletionToken, Signatures...>(
                    timed_initiation<Signatures...>{},
                    t.token,   // the underlying token
                    t.timer,
                    t.timeout, // our timeout argument
                    std::forward<Initiation>(init),  // the underlying operation's initiation
                    std::forward<InitArgs>(init_args)... // that initiation's arguments
                    );
    }
};

template <typename CompletionToken>
timed_token<CompletionToken>
timed(boost::asio::steady_timer* timer, const TDurationUnit& timeout, CompletionToken&& token) {
    return timed_token<CompletionToken>{ timer, timeout, token };
}

template <typename Op, typename CompletionToken>
auto with_timeout(Op&& op, boost::asio::steady_timer* timer, const TDurationUnit& timeout, CompletionToken&& token) {
    return std::forward<Op>(op)(timed(timer, timeout, std::forward<CompletionToken>(token)));
}

#endif /* TIMED_HPP */
