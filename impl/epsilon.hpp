#pragma once

#include <float.h>

template <typename T> T epsilon(void);

template <>
inline float
epsilon<float>(void)
{
    return FLT_EPSILON;
}

template <>
inline double
epsilon<double>(void)
{
    return DBL_EPSILON;
}

template <>
inline long double
epsilon<long double>(void)
{
    return LDBL_EPSILON;
}

