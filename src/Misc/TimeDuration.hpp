#ifndef TIME_DURATION_HPP
#define TIME_DURATION_HPP

#include <boost/optional.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using TSeconds = boost::posix_time::seconds;
using TTimeDuration = boost::optional<boost::posix_time::time_duration>;

#endif /* TIME_DURATION_HPP */
