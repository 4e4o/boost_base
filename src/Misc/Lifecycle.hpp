#ifndef LIFECYCLE_HPP
#define LIFECYCLE_HPP

#include <boost/signals2/signal.hpp>

class Lifecycle {
public:

    /**
     * Залочит объект когда сигнал start сработает, будет слушать сигнал end,
     * на сигнале end отпустит залоченный объект
     */

    template<typename TStartSignal, typename TEndSignal, typename T>
    static void lockWhen(TStartSignal& start,
                         TEndSignal& end,
                         const std::weak_ptr<T>& weakObj) {
        start.connect([weakObj, &end](auto &&...) {
            auto locked = weakObj.lock();
            assert(locked != nullptr);
            // NOTE
            // standart guarantes that obj will NOT be optimized out
            // https://stackoverflow.com/questions/12718423/c-can-an-unused-lambda-explicit-capture-be-optimized-out
            end.connect_extended([locked](const boost::signals2::connection& c,
                                 auto &&...) {
                c.disconnect();
            });
        });
    }

    template<typename TStartSignal, typename T, typename TMethod>
    static void connectTrack(TStartSignal &s, const std::shared_ptr<T>& obj, TMethod&& m) {
        s.connect(typename TStartSignal::slot_type(std::forward<TMethod>(m), obj.get()).track_foreign(obj));
    }
};

#endif /* LIFECYCLE_HPP */
