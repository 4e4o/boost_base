#ifndef INTRUSIVE_LIST_HELPERS_HPP
#define INTRUSIVE_LIST_HELPERS_HPP

#include <boost/intrusive/list.hpp>

typedef boost::intrusive::list_base_hook<boost::intrusive::link_mode<boost::intrusive::auto_unlink>> TAutoUnlinkBaseListHook;

template<class T>
using TIntrusiveListAutoUnlink = boost::intrusive::list<T, boost::intrusive::constant_time_size<false>>;

#endif /* INTRUSIVE_LIST_HELPERS_HPP */
