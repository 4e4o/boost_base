#ifndef DEBUG_HPP
#define DEBUG_HPP

#include "AApplication.h"

#ifdef NDEBUG
#define debug_print(...) { }
#else
#define debug_print(...) { AAP->log(__VA_ARGS__); }
#endif // NDEBUG

#endif /* DEBUG_HPP */
