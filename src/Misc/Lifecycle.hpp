#ifndef LIFECYCLE_HPP
#define LIFECYCLE_HPP

#include <boost/signals2/signal.hpp>

class Lifecycle {
public:
    template<typename TStartSignal, typename T, typename TMethod>
    static void connectTrack(TStartSignal &s, const std::shared_ptr<T>& obj, TMethod&& m) {
        s.connect(typename TStartSignal::slot_type(std::forward<TMethod>(m), obj.get()).track_foreign(obj));
    }
};

#endif /* LIFECYCLE_HPP */
