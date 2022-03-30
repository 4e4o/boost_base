#ifndef TIMEOUTS_HPP
#define TIMEOUTS_HPP

#include "MagicEnum.hpp"
#include "TimeDuration.hpp"

#include <array>

template<typename Enum>
class Timeouts {
public:
    typedef Enum Timeout;

    const TTimeDuration& getTimeout(Enum e) {
        return m_timeouts[index(e)];
    }

    void setTimeout(Enum e, const TTimeDuration& d = boost::none) {
        m_timeouts[index(e)] = d;
    }

    void setTimeout(Enum e, const TDurationUnit& d) {
        m_timeouts[index(e)] = d;
    }

private:
    static constexpr int Size = magic_enum::enum_count<Enum>();

    std::size_t index(Enum e) {
        auto indexOpt = magic_enum::enum_index(e);
        assert(indexOpt.has_value());
        return *indexOpt;
    }

    std::array<TTimeDuration, Size> m_timeouts;
};

#endif /* TIMEOUTS_HPP */
