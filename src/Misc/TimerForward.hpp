#ifndef TIMER_FORWARD_HPP
#define TIMER_FORWARD_HPP

#include "TimeDuration.hpp"

#include <memory>

class Timer;

typedef std::unique_ptr<Timer> TTimer;

#endif /* TIMER_FORWARD_HPP */
