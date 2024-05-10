/**
 * \file
 * \brief Contains my assert implementation
*/


#ifndef _ASSERT_H_
#define _ASSERT_H_


#include <cstdio>
#include <cstdlib>


/**
 * \brief If condition fails, prints message and exits
*/
#define ASSERT(condition, ...)                                              \
do {                                                                        \
    if (!(condition)) {                                                     \
        printf("%s(%i) in %s:\n", __FILE__, __LINE__, __PRETTY_FUNCTION__); \
        printf(__VA_ARGS__);                                                \
        abort();                                                            \
    }                                                                       \
} while(0)


#endif
