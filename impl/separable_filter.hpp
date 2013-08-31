/** Implementation of separable image filter.
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

#ifndef _IS_ODD
#define _IS_ODD(x) (x % 2 != 0 ? true : false)
#endif
#ifndef _IS_EVEN
#define _IS_EVEN(x) (x % 2 == 0 ? true : false)
#endif

#ifndef _ROWMAJOR_INDEX
#define _ROWMAJOR_INDEX(x,y,width,height) (width*y + x) 
#endif
#ifndef _COLUMNMAJOR_INDEX
#define _COLUMNMAJOR_INDEX(x,y,width,height) (height*x + y)
#endif

#ifndef NULL
#define _NON_STD_NULL_DEFINED // define NULL here, but don't forget to clean it up at the end in order to avoid name clashes
#define NULL (0)
#endif

/* TODO:
 * - ROI!
 * - GPU implementation?
 */

/** Flip an array. */
template <typename T, typename T_size>
inline void
FlipArray(const T* in, T_size length, T* out)
{
    for (T_size i(0); i < length; i++)
        out[length-1-i]=in[i];
}

/** In-place flip of an array. */
template <typename T, typename T_size>
void
FlipArray(T* array, T_size length);

/** Transpose the matrix. */
template <typename T, typename T_size>
inline void
Transpose(const T* in, T_size in_width, T_size in_height, T* out)
{
    for (T_size y(0); y < in_height; y++)
        for (T_size x(0); x < in_width; x++)
            out[x*in_height + y] = in[in_width*y + x];
}

/** Implementation of 2-D linear separable filter. First we perform row-filtering and the column-filtering. Row-major input is expected. */
template <typename T, typename S, typename R, typename T_size>
inline void
SeparableFilter(const T* in, T_size width, T_size height, const S* row_filter, T_size row_length, const S* col_filter, T_size col_length, R*& tmp, R*& out)
{
    /* Allocate output memory if necessary */
    if (out == NULL)
        out = new R[width*height];   
    if (tmp == NULL)
        tmp = new R[width*height]; // temporary result, i.e. transposed output of the row filter
        
    // perform the filtering
    RowFilter(in,width,height,row_filter,row_length,tmp,true);
    RowFilter(tmp,height,width,col_filter,col_length,out,true);
}

/** Filter all rows (separate) of an image. Useful for linear separable filters, which can be applied as (1-D) row/column filter. Row-major! 
 *  Allowing to transpose the output makes it possible to simply (and with good memory access patterns) implement 2-D linear separable filters by
 *  applying the filter as follows:
 *   RowFilter(in,width,height,row_filter,row_filter_length,temporary_out,true)
 *   RowFilter(temporary_out,height,width,column_filter,column_filter_length,out,true)
 *  Since we transpose the (temporary) output two times the resulting output image is row-major again.
 *
 *  Note: at image borders the filter responses in x-direction (i.e. row direction) are invalid (size of this area depends on the filter size)! For efficiency reasons we do not have any border handling, e.g. replication!
 */
template <typename T, typename S, typename R, typename T_size>
void
RowFilter(const T* in, T_size width, T_size height, const S* filter, T_size length, R* out, bool transposedOut = false); //R*& out, bool transposeOut = false);

/** Filter all rows (separate) of an image. Useful for linear separable filters, which can be applied as (1-D) row/column filter. Row-major! 
 *  Allowing to transpose the output makes it possible to simply (and with good memory access patterns) implement 2-D linear separable filters by
 *  applying the filter as follows:
 *   RowFilter(in,width,height,row_filter,row_filter_length,temporary_out,true)
 *   RowFilter(temporary_out,height,width,column_filter,column_filter_length,out,true)
 *  Since we transpose the (temporary) output two times the resulting output image is row-major again.
 *  Allows the use of a region of interest. The parameter "isolated" has an effect on the border area
 *   isolated = true, then only elements in the ROI are processed, which results to invalid filter responses in the border areas of the output in x-direction, i.e. the first and last floor(length/2) elements are no valid filter responses
 *   isolated = false, then the filtering also processes elements outside the ROI in x-direction and all filter responses in the ROI are valid, i.e. the elements in the x-interval [roi_x_min-length/2,roi_x_max+length/2] are used for calculation
 *
 *  Note: at image borders the filter responses in x-direction (i.e. row direction) are invalid (size of this area depends on the filter size)! For efficiency reasons we do not have any border handling, e.g. replication!
 */
template <typename T, typename S, typename R, typename T_size>
void
RowFilter(const T* in, T_size width, T_size height, const S* filter, T_size length, R* out, T_size roi_x_min, T_size roi_y_min, T_size roi_width, T_size roi_height, bool isolated = false, bool transposedOut = false);

#ifdef _NON_STD_NULL_DEFINED
#undef _NON_STD_NULL_DEFINED
#undef NULL
#endif

