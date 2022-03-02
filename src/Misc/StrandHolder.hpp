#ifndef STRAND_HOLDER_HPP
#define STRAND_HOLDER_HPP

#include <boost/asio/strand.hpp>
#include <boost/asio/deadline_timer.hpp>

#define STRAND_ASSERT(s) assert((s)->strand().running_in_this_thread())

class StrandHolder {
public:
    StrandHolder(boost::asio::io_context&);
    virtual ~StrandHolder();

    boost::asio::io_context& io() const;
    boost::asio::io_context::strand& strand() const;

    void setStrand(StrandHolder*, bool owner);
    void setStrand(boost::asio::io_context::strand*, bool owner);

    template <class Callable>
    void post(Callable&& c) {
        boost::asio::post(m_io, m_strand->wrap(std::forward<Callable>(c)));
    }

private:
    boost::asio::io_context& m_io;
    std::unique_ptr<boost::asio::io_context::strand> m_strand;
    bool m_owner;
};

#endif /* STRAND_HOLDER_HPP */
