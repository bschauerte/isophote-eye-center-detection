#pragma once

#ifndef NULL
#define _NON_STD_NULL_DEFINED
#define NULL (0)
#endif

/** Get the length of the filter. */
template <typename T, typename S>
S
GetGaussLength(const T& sigma);

/** Calculate the support; here it means half the filter width. */
template <typename T, typename S>
S
GetGaussSupport(const T& sigma, T* Ax = NULL);

/** Calculate the Gaussian with parameter sigma (mu=0). 
 *  If Ax == NULL, then the support will be calculated again; if Ax != NULL, then Ax is used as support (however, the length of support has to be GetGaussLength; this allows to reuse Ax and avoid multiple calculations)
 *  If out != NULL, then the output will be stored in out; otherwise a new array will be allocated
 */
template <typename T>
T*
CreateGauss(const T& sigma, bool normalize = false, const T* Ax = NULL, T* out = NULL);

/** Calculate the 1st derivative of the Gaussian with parameter sigma (mu=0).
 *  If Ax == NULL, then the support will be calculated again; if Ax != NULL, then Ax is used as support (however, the length of support has to be GetGaussLength; this allows to reuse Ax and avoid multiple calculations)
 *  If out != NULL, then the output will be stored in out; otherwise a new array will be allocated
 */
template <typename T>
T*
CreateGaussFirstDeriv(const T& sigma, bool normalize = false, const T* Ax = NULL, T* out = NULL);

/** Calculate the 2nd derivative of the Gaussian with parameter sigma (mu=0).
 *  If Ax == NULL, then the support will be calculated again; if Ax != NULL, then Ax is used as support (however, the length of support has to be GetGaussLength; this allows to reuse Ax and avoid multiple calculations)
 *  If out != NULL, then the output will be stored in out; otherwise a new array will be allocated
 */
template <typename T>
T*
CreateGaussSecondDeriv(const T& sigma, bool normalize = false, const T* Ax = NULL, T* out = NULL);

#ifdef _NON_STD_NULL_DEFINED
#undef _NON_STD_NULL_DEFINED
#undef NULL
#endif
