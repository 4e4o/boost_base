#ifndef DEBUG_HPP
#define DEBUG_HPP

#include "AApplication.h"

#ifdef NDEBUG
#define debug_print(...) { }
#define assert_strand(x) { }
#else
#define debug_print(...) { AAP->log(__VA_ARGS__); }
#define assert_strand(x) assert(x.running_in_this_thread())
#endif // NDEBUG

#endif /* DEBUG_HPP */
