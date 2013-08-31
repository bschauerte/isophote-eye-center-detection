/* Isophote and Accumulator calculation
 * 
 *  \author B. Schauerte
 *  \email  <schauerte@ieee.org>
 *  \date   2011
 *
 * Copyright (C) Boris Schauerte - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Boris Schauerte <schauerte@ieee.org>, 2011
 */
#pragma once

#ifndef NULL
#define _NON_STD_NULL_DEFINED // define NULL here, but don't forget to clean it up at the end in order to avoid name clashes
#define NULL (0)
#endif

/** Calculate the isophote information
 *  \param Lx partial derivative in x-direction
 *  \param Ly partial derivative in y-direction
 *  \param Lxx 2nd partial derivative in x-direction
 *  \param Lyy 2nd partial derivative in y-direction
 *  \param Lxy partial derivative in x- and y-direction
 *  \param width image width
 *  \param height image height
 *  \param k output: the isophote curvature
 *  \param c output: the isophote curvedness
 *  \param d output: displacement vector
 *  \param tmpT1 optional temporary variable; has to be big enough to hold width x height images (it is beneficial to pre-allocate tmpT1 if this procedure is called more than once)
 *  \param tmpLx2 optional temporary variable; has to be big enough to hold width x height images (it is beneficial to pre-allocate tmpT1 if this procedure is called more than once)
 *  \param tmpLy2 optional temporary variable; has to be big enough to hold width x height images (it is beneficial to pre-allocate tmpT1 if this procedure is called more than once)
 *
 *  \note this is a pixel-wise calculation, accordingly it is not important whether the data is in row-major or column-major order
 */
template <typename T, typename T_size>
void
CalculateIsophoteInformation(const T* Lx, const T* Ly, const T* Lxx, const T* Lxy, const T* Lyy, T_size width, T_size height, T* k, T* c, T* dx, T* dy, 
                             T* tmpT1 = NULL, T* tmpLx2 = NULL, T* tmpLy2 = NULL);

/** Calculate the isophote information
 *  \param Lx partial derivative in x-direction
 *  \param Ly partial derivative in y-direction
 *  \param Lxx 2nd partial derivative in x-direction
 *  \param Lyy 2nd partial derivative in y-direction
 *  \param Lxy partial derivative in x- and y-direction
 *  \param width image width
 *  \param height image height
 *  \param k output: the isophote curvature
 *  \param c output: the isophote curvedness
 *  \param d output: displacement vector
 *  \param roi_x_min min. x-coordinate of the ROI (anchor)
 *  \param roi_y_min min. y-coordinate of the ROI (anchor)
 *  \param roi_width width of the ROI
 *  \param roi_height height of the ROI
 *  \param tmpT1 optional temporary variable; has to be big enough to hold width x height images (it is beneficial to pre-allocate tmpT1 if this procedure is called more than once)
 *  \param tmpLx2 optional temporary variable; has to be big enough to hold width x height images (it is beneficial to pre-allocate tmpT1 if this procedure is called more than once)
 *  \param tmpLy2 optional temporary variable; has to be big enough to hold width x height images (it is beneficial to pre-allocate tmpT1 if this procedure is called more than once)
 *
 *  \note data is expected in row-major order
 */
template <typename T, typename T_size>
void
CalculateIsophoteInformation(const T* Lx, const T* Ly, const T* Lxx, const T* Lxy, const T* Lyy, T_size width, T_size height, T* k, T* c, T* dx, T* dy,
                             T_size roi_x_min, T_size roi_y_min, T_size roi_width, T_size roi_height,
                             T* tmpT1 = NULL, T* tmpLx2 = NULL, T* tmpLy2 = NULL);

/** 
 * Calculates the accumulator. Only updates the accumulator for values of 
 * k < 0 (i.e., for eye-center detection - gradient towards the darker eye 
 * center). 
 */
template <typename T, typename T_size>
void
CalculateAccumulator(const T* k, const T* c, const T* dx, const T* dy, T_size width, T_size height, T* acc, bool zero_acc = false, bool row_major = true); // zero_acc => are the accumulator cells set to zero?

/** 
 * Calculates the accumulator. Only updates the accumulator for values of 
 * k > 0 (i.e., for saliency maps - gradient towards the more salient center 
 * of the peak). 
 */
template <typename T, typename T_size>
void
CalculateAccumulatorPosK(const T* k, const T* c, const T* dx, const T* dy, T_size width, T_size height, T* acc, bool zero_acc = false, bool row_major = true); // zero_acc => are the accumulator cells set to zero?

#ifdef _NON_STD_NULL_DEFINED
#undef _NON_STD_NULL_DEFINED
#undef NULL
#endif
