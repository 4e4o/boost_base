#ifndef TIMED_ERROR_HPP
#define TIMED_ERROR_HPP

#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

// adapted example from https://www.boost.org/doc/libs/1_78_0/libs/outcome/doc/html/motivation/plug_error_code2.html

enum class TimedErrc {
    NoError = 0,  // 0 should not represent an error
    TimedOut = 1
};

namespace boost::system {
template <> struct is_error_code_enum<TimedErrc> : std::true_type { };
}

namespace detail {
class TimedErrc_category : public boost::system::error_category {
public:
    // Return a short descriptive name for the category
    virtual const char *name() const noexcept override final { return "TimedErrc_category"; }
    // Return what each enum means in text
    virtual std::string message(int c) const override final {
        switch(static_cast<TimedErrc>(c)) {
        case TimedErrc::NoError:
            return "timed no error";
        case TimedErrc::TimedOut:
            return "timed operation timeout";
        default:
            return "unknown";
        }
    }
};
}  // namespace detail

inline const detail::TimedErrc_category &TimedErrc_category() {
    static detail::TimedErrc_category c;
    return c;
}

inline boost::system::error_code make_error_code(TimedErrc e) {
    return {static_cast<int>(e), TimedErrc_category()};
}

struct gen_timeout_error {
    template<typename ...Args>
    static auto gen(Args&&... args) {
        return std::make_tuple(std::forward<decltype(args)>(args)...);
    }

    template<typename ...Args>
    static auto gen(boost::system::error_code, Args&&... args) {
        using namespace boost::system;
        const error_code err = make_error_code(TimedErrc::TimedOut);
        return std::make_tuple(err, std::forward<decltype(args)>(args)...);
    }

    template<typename ...Args>
    static auto gen(std::exception_ptr, Args&&... args) {
        using namespace boost::system;
        const error_code err = make_error_code(TimedErrc::TimedOut);
        // тут хрен знает почему надо обарачивать error_code в boost::system::system_error
        // если не оборачивать то выкидывается exception_ptr напрямую
        // а если обернуть то как надо выкидывается boost::system::error_code TimedErrc::TimedOut
        // решение подсмотрено в сорцах co_spawn буста, там тоже оборачивается
        std::exception_ptr exc = std::make_exception_ptr(boost::system::system_error(err));
        return std::make_tuple(exc, std::forward<decltype(args)>(args)...);
    }
};

#endif /* TIMED_ERROR_HPP */
