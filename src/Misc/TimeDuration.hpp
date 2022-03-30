#ifndef TIME_DURATION_HPP
#define TIME_DURATION_HPP

#include <boost/optional.hpp>

#include <chrono>

using TDurationUnit = std::chrono::milliseconds;
using TTimeDuration = boost::optional<TDurationUnit>;

#endif /* TIME_DURATION_HPP */
