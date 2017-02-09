#pragma once

#include <ostream>

/* Macro for ASSERT */
#ifndef ASSERT
#ifndef OPT
#define ASSERT(x) assert(x)
#else
#define ASSERT(x)
#endif
#endif

/* Macro for ERROR */
#ifndef ERROR
#ifndef __ERROR_1__
#define __ERROR_1__(text) { std::cerr << "Error: " << text << std::endl; exit(1); }
#endif
#ifndef __ERROR_2__
#define __ERROR_2__(text, callback) { std::cerr << "Error: " << text << std::endl; callback(); exit(1); }
#endif
#define GET_MACRO(_1, _2, NAME, ...) NAME
#define ERROR(...) GET_MACRO(__VA_ARGS__, __ERROR_2__, __ERROR_1__)(__VA_ARGS__)
#endif
