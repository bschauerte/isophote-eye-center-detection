/* Templatized version of {FLT|DBL|LDBL}_EPSILON
 *
 *  \author B. Schauerte
 *  \email  <schauerte@kit.edu>
 *  \date   2011
 *
 * Copyright (C) Boris Schauerte - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Boris Schauerte <schauerte@ieee.org>, 2011
 */
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

