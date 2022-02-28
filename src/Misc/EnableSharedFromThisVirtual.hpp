#ifndef ENABLE_SHARED_FROM_THIS_VIRTUAL_HPP
#define ENABLE_SHARED_FROM_THIS_VIRTUAL_HPP

#include <memory>

template<typename T>
struct enable_shared_from_this_virtual;

class enable_shared_from_this_virtual_base : public std::enable_shared_from_this<enable_shared_from_this_virtual_base> {
    typedef std::enable_shared_from_this<enable_shared_from_this_virtual_base> base_type;

    template<typename T>
    friend struct enable_shared_from_this_virtual;

    std::shared_ptr<enable_shared_from_this_virtual_base> shared_from_this() {
        return base_type::shared_from_this();
    }

    std::shared_ptr<enable_shared_from_this_virtual_base const> shared_from_this() const {
        return base_type::shared_from_this();
    }
};

template<typename T>
struct enable_shared_from_this_virtual: virtual enable_shared_from_this_virtual_base {
    typedef enable_shared_from_this_virtual_base base_type;

public:
    template<typename TCasted = T>
    std::shared_ptr<TCasted> shared_from_this() {
        return std::dynamic_pointer_cast<TCasted>(std::shared_ptr<T>(base_type::shared_from_this(), static_cast<T*>(this)));
    }

    template<typename TCasted = T>
    std::shared_ptr<TCasted const> shared_from_this() const {
        return std::dynamic_pointer_cast<TCasted const>(std::shared_ptr<T const>(base_type::shared_from_this(), static_cast<T const*>(this)));
    }
};

#endif // ENABLE_SHARED_FROM_THIS_VIRTUAL_HPP
