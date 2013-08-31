/** Correlation-based (thus only useful for small filters) filtering of signals.
 *  Inspired by and as a more efficient (especially less overhead) replacement for imfilter ...
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
#include <iostream>
#include <cassert>
#include <cstring>
#include <cmath>

#include <algorithm> // required for std::swap

#ifdef __MEX
#define __CONST__ const
#include "mex.h"
#include "matrix.h"
#endif

// implemented padding types
typedef enum PadType
{
  ConstPad = 0,
  ReplicatePad,
  SymmetricPad, 
  CircularPad,
  NumPadTypes,
  UnknownPad
} PadType;

std::string 
ToString(const PadType t)
{
  switch (t)
  {
    case ConstPad:
      return "ConstPad";
    case ReplicatePad:
      return "ReplicatePad";
    case CircularPad:
      return "CircularPad";
    case SymmetricPad:
      return "SymmetricPad";
    default:
      assert(false);
      return "UnknownPad";
  }
}

PadType 
GetPadType(const char* c)
{
  if (strcasecmp("circular",c) == 0)
    return ConstPad;
  else if (strcasecmp("replicate",c) == 0)
    return ReplicatePad;
  else if (strcasecmp("symmetric",c) == 0)
    return SymmetricPad;
  else if (strcasecmp("const",c) == 0)
    return CircularPad;
  else 
    return UnknownPad;
}

// create a padded copy of the input array
//  in:            the input array (a step length between the elements can be specified using in_step_length)
//  out:           the padded output array (will be allocated iff it is ==0)
//  pad_length:    the length of the padding on each side of the array
//  pad_type:      the type of the padding (constant, replicate, symmetric, or circular)
//  pad_parameter: an optional parameter can be provided for the padding (most importantly the value for a constant pad)
//
// returns the pointer to the input data segment of the padded array (i.e. the beginning of the original data w/o the left pad)
template <typename T, typename T_size>
inline T* 
PadArray(T*& out, const T* in, const T_size in_length, const PadType pad_type, const T_size pad_length, const void* pad_parameter = 0, const T_size in_step_length = 0)
{
  // if necessary, then we allocate the output array ourselves
	if (out == NULL) 
    out = new T[in_length + 2*pad_length];
  
  T* pad_left  = out;                          // start of the left pad array segment
  T* pad_data  = out + pad_length;             // start of the central (in) data segment
  T* pad_right = out + pad_length + in_length; // start of the right pad array segment
  
  // copy the input data
  if (in_step_length == 0 || in_step_length == 1)
    memcpy((void*)pad_data,(void*)in,sizeof(T)*in_length);
  else
    for (T_size i(0); i < in_length; i++)
      pad_data[i] = in[i*in_step_length];
  
  // create the pad
  switch (pad_type)
  {
    //
    // Input array values outside the bounds of the array are implicitly assumed to have the value *((T*)pad_parameter).  When no (T*)pad_parameter (i.e. pad_parameter=0) is specified, X = 0 is used.
    //
    case ConstPad:
      {
        T pad_value = 0;
        if (pad_parameter != 0)
          pad_value = *((T*)pad_parameter);
        for (T_size i(0); i < pad_length; i++)
        {
          pad_left[i] = pad_value;
          pad_right[i] = pad_value;
        }
      }
      break;
    //
    // Input array values outside the bounds of the array are computed by implicitly assuming the input array is periodic.
    //
    case CircularPad:
      {
        for (T_size i(0); i < pad_length; i++)
        {
          pad_left[i] = pad_data[in_length - pad_length + i];
          pad_right[i] = pad_data[i];
        }
      }
      break;
    //
    // Input array values outside the bounds of the array are computed by mirror-reflecting the array across the array border.
    //
    case SymmetricPad:
      {
        for (T_size i(0); i < pad_length; i++)
        {
          pad_left[i] = pad_data[pad_length - i - 1];
          pad_right[i] = pad_data[in_length - 1 - i];
        }
      }
      break;
    //
    // Input array values outside the bounds of the array are assumed to equal the nearest array border value.
    //
    case ReplicatePad:
      {
        T pad_value_left = pad_data[0];
        T pad_value_right = pad_data[in_length - 1];
        for (T_size i(0); i < pad_length; i++)
        {
          pad_left[i] = pad_value_left;
          pad_right[i] = pad_value_right;
        }
      }
      break;
    default:
      assert(false);
      break;
  }
  
  return pad_data;
}

// Filter a (padded) array.
// Correlation-based filtering is used (i.e. no convolution)
template <typename T, typename T_size>
inline void 
CorrFilterPaddedArray(T*& out, const T* padded_data, const T_size data_length, const T* filter, const T_size filter_length, const T_size filter_anchor = 0, const T_size out_step_length = 0)
{
  int _filter_length = (int)filter_length;
  int _filter_anchor = (int)filter_anchor;
  
  if (out == 0)
  {
    if (out_step_length > 0)
      out = new T[data_length*out_step_length];
    else
      out = new T[data_length];
  }
  
  if (out_step_length == 0 || out_step_length == 1)
  {
    for (T_size i(0); i < data_length; i++)
    {
      // @todo: optimize implementation
      out[i] = T(0);
      const T* in = padded_data + _filter_anchor - filter_length + i;
      for (int xf(0); xf < _filter_length; xf++)
      {
        out[i] += in[xf] * filter[xf];
      }
    }
  }
  else
  {
    for (T_size i(0); i < data_length; i++)
    {
      // @todo: optimize implementation
      out[i*out_step_length] = T(0);
      const T* in = padded_data + _filter_anchor - filter_length + i;
      for (int xf(0); xf < _filter_length; xf++)
      {
        out[i*out_step_length] += in[xf] * filter[xf];
      }
    }
  }
}

// simple interface to filter an array (not optimal, because the padded_in memory isn't reused
template <typename T, typename T_size>
inline void 
CorrFilterArray(T*& out,                                         // output (allocated if ==0); the number of output elements (and the size respecting out_step_length) always is the same the input length
                const T* in, const T_size in_length,             // input
                const T* filter, const T_size filter_length,     // filter
                const PadType pad_type, const T_size pad_length, // pad
                const void* pad_parameter = 0,                   // optional: pad parameters
                const T_size filter_anchor = 0,                  // optional: where is the filter anchored
                const T_size in_step_length = 0,                 // optional: input data step length/size
                const T_size out_step_length = 0                 // optional: output data step length/size
               )
{
  T* padded_array = 0;
  
  // 1. pad array
  T* padded_data = PadArray(padded_array,in,in_length,pad_type,pad_length,pad_parameter,in_step_length);
  // 2. filter
  CorrFilterPaddedArray(out,padded_data,in_length,filter,filter_length,filter_anchor,out_step_length);
  
  delete [] padded_array;
}

#ifdef __MEX
template <typename T>
void
_mexFunction(int nlhs, mxArray* plhs[],
            int nrhs, const mxArray* prhs[])
{
  // short-cuts for the input data
  __CONST__ mxArray* mdata = prhs[0];
  __CONST__ mxArray* mfilter = prhs[1];
  __CONST__ mxArray* mpadding = prhs[2];
  __CONST__ mxArray* mdimension = prhs[3];
  
  // create the output data structures
  mxArray *mout = mxCreateNumericArray(mxGetNumberOfDimensions(mdata),
          mxGetDimensions(mdata),
          mxGetClassID(mdata),
          (mxIsComplex(mdata) ? mxCOMPLEX: mxREAL));
	plhs[0] = mout; 
  
  T* pout = (T*)mxGetData(mout);
  const T* pdata = (T*)mxGetData(mdata);
  const T* pfilter = (T*)mxGetData(mfilter);
  char* cpad_type = mxArrayToString(mpadding);
  PadType pad_type;
  T* pad_parameter = 0;
  T pad_const_value = 0;
  if (cpad_type == NULL)
  {
    pad_type = ConstPad;
    pad_const_value = (T)mxGetScalar(mpadding);
    pad_parameter = &pad_const_value;
  }
  else
  {
    pad_type = GetPadType(cpad_type);
    if (pad_type == UnknownPad)
    {
      mexErrMsgTxt("Invalid/Unknown pad type/parameter.\n");
      mxFree(cpad_type);
    }
  }
  
  size_t ndims = mxGetNumberOfDimensions(mdata);
  const mwSize* dims = mxGetDimensions(mdata);
  
  // "target" dimension, i.e. the dimension along which we want to filter
  size_t tdim = (mwSize)mxGetScalar(mdimension);
  if (tdim > ndims)
    mexErrMsgTxt("Invalid target dimension (number of dimensions is smaller than the target dimension).\n");
  
  size_t filter_length = mxGetNumberOfElements(mfilter);
  size_t pad_length = filter_length; // dummy! @todo: more intelligent choice
  
  // is it a row or column vector, or even a degenerate vector in which all dimensions except one are singleton?
  bool is_one_dimensional_data = false;
  for (size_t i = 0; i < ndims; i++)
  {
    if (dims[i] == mxGetNumberOfElements(mdata))
    {
      is_one_dimensional_data = true;
      break;
    }
  }
  if (is_one_dimensional_data && tdim == 0) // allow a special case: if the data is practically one-dimensional, then having the target dimension set to 0 lets us automatically filter along the only non-singleton dimension
  {
    CorrFilterArray(pout,pdata,mxGetNumberOfElements(mdata),pfilter,filter_length,pad_type,pad_length,pad_parameter,filter_length / 2+1);
  }
  else
  {
    if (ndims > 3)
     mexErrMsgTxt("Currently only filtering for data with at most 3 dimensions is implemented; extend the implementation if necessary!"); // @todo: allow arbitrary number of dimensions
    
    size_t _tdim = tdim - 1; // from MATLAB indices to C/C++ indices (i.e. start with 1 vs. starting with 0)
        
    // calculate the input/output step length
    size_t in_step_length = 1;
    for (size_t i = 0; i  < _tdim; i++)
      in_step_length *= dims[i];
    size_t out_step_length = in_step_length; // because we cloned the dimensions of the input array
    
    // 2-D data
    if (ndims == 2)
    {
      if (_tdim == 0)
      {
        for (mwSize i = 0; i < dims[1]; i++)
        {
          const mwSize idx = i*dims[_tdim];
          T* _pout = &pout[idx];
          const T* _pdata = &pdata[idx];
          CorrFilterArray(_pout,_pdata,(size_t)dims[_tdim],pfilter,filter_length,pad_type,pad_length,(void*)pad_parameter,filter_length / 2+1,in_step_length,out_step_length);
        }
      }
      else if (_tdim == 1)
      {
        for (mwSize i = 0; i < dims[0]; i++)
        {
          const mwSize idx = i;
          T* _pout = &pout[idx];
          const T* _pdata = &pdata[idx];
          CorrFilterArray(_pout,_pdata,(size_t)dims[_tdim],pfilter,filter_length,pad_type,pad_length,(void*)pad_parameter,filter_length / 2+1,in_step_length,out_step_length);
        }
      }
    }
    
    // 3-D data
#define _A3D_IDX(i,j,k,dims) ((i)+((j)+(k)*dims[1])*dims[0]) // column-major indexing of 3-D array
    if (ndims == 3)
    {
      if (_tdim == 0)
      {
        const mwSize i = 0;
        for (mwSize j = 0; j < dims[1]; j++)
        {
          for (mwSize k = 0; k < dims[2]; k++)
          {
            const mwSize idx = _A3D_IDX(i,j,k,dims);
            T* _pout = &pout[idx];
            const T* _pdata = &pdata[idx];
            CorrFilterArray(_pout,_pdata,(size_t)dims[_tdim],pfilter,filter_length,pad_type,pad_length,(void*)pad_parameter,filter_length / 2+1,in_step_length,out_step_length);
          }
        }
      }
      else if (_tdim == 1)
      {
        for (mwSize i = 0; i < dims[0]; i++)
        {
          const mwSize j = 0;
          for (mwSize k = 0; k < dims[2]; k++)
          {
            const mwSize idx = _A3D_IDX(i,j,k,dims);
            T* _pout = &pout[idx];
            const T* _pdata = &pdata[idx];
            CorrFilterArray(_pout,_pdata,(size_t)dims[_tdim],pfilter,filter_length,pad_type,pad_length,(void*)pad_parameter,filter_length / 2+1,in_step_length,out_step_length);
          }
        }
      }
      else if (_tdim == 2)
      {
        for (mwSize i = 0; i < dims[0]; i++)
        {
          for (mwSize j = 0; j < dims[1]; j++)
          {
            const mwSize k = 0;
            const mwSize idx = _A3D_IDX(i,j,k,dims);
            T* _pout = &pout[idx];
            const T* _pdata = &pdata[idx];
            CorrFilterArray(_pout,_pdata,(size_t)dims[_tdim],pfilter,filter_length,pad_type,pad_length,(void*)pad_parameter,filter_length / 2+1,in_step_length,out_step_length);
          }
        }
      }
    }
    
    
//     -- ARBITRARY DIMENSIONS -- SORRY, FIX ME, BUT I CAN'T SEE THROUGH THE INDEXING TODAY
//     size_t n = 1;
//     for (size_t i = 0; i < ndims; i++)
//       if (i != _tdim)
//         n *= dims[i];
//     size_t numel = n * dims[_tdim];
//     
//     size_t offset_step_length = 1;
//     size_t offset = 0;
//     for (size_t i = 0; i < n; i++)
//     {
//       mexPrintf("%d (numel=%d,in_step_length=%d,offset_step_length=%d,dims[tdim]=%d)\n", offset,numel,in_step_length,offset_step_length,dims[_tdim]);
//       T* _pout = &pout[offset];
//       const T* _pdata = &pdata[offset];
//       CorrFilterArray(_pout,_pdata,(size_t)dims[_tdim],pfilter,filter_length,pad_type,pad_length,(void*)pad_parameter,filter_length / 2+1,in_step_length,out_step_length);
//       offset += offset_step_length;
//       //offset = offset % numel;
//       //offset = offset / numel;
//     }
  }
    
  // free memory
  if (cpad_type)
    mxFree(cpad_type);
}

/* imfilter like filtering implementation, but 1-D and with support filter along a specified dimension */
void
mexFunction(int nlhs, mxArray* plhs[],
            int nrhs, const mxArray* prhs[])
{
  // check number of input parameters
  if (nrhs != 4)
    mexErrMsgTxt("Wrong number of input arguments. Input should be: data filter padding dimension.\n  If the data has only one non-singleton dimension and dimension is set to 0, then the filtering is done along the only non-singleton dimension.");
  // Check number of output parameters
  if (nlhs > 1) 
    mexErrMsgTxt("Wrong number of output arguments.");
  // all arrays are required to have the same input type
  if (mxGetClassID(prhs[0]) != mxGetClassID(prhs[1]))
    mexErrMsgTxt("The data and filter required to have the same data type.");
  // only float and double are currently supported
  if (!mxIsDouble(prhs[0]) && !mxIsSingle(prhs[0])) 
  	mexErrMsgTxt("Only float and double input arguments are supported.");
  
  switch (mxGetClassID(prhs[0]))
  {
    case mxDOUBLE_CLASS:
      _mexFunction<double>(nlhs,plhs,nrhs,prhs);
      break;
    case mxSINGLE_CLASS:
      _mexFunction<float>(nlhs,plhs,nrhs,prhs);
      break;
    default:
      // this should never happen
      break;
  }
}
#endif

#ifdef __STANDALONE
/* Test some routines and test cases in a standalone executable. */
int
main(int argc, char* argv[])
{
  //
  // The following code should produce the same result as MATLAB's imfilter
  //   imfilter([1 2 3 4 5 6],[1 2 3],0)
  //   imfilter([1 2 3 4 5 6],[1 2 3],'replicate')
  //   imfilter([1 2 3 4 5 6],[1 2 3],'symmetric')
  //   imfilter([1 2 3 4 5 6],[1 2 3],'circular')
  //
  
  double in_array[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};
  const int in_array_length = 6;
  const int pad_length = 3;
  
  double* padded_array = 0;
  double* padded_data = 0;
  
  double filter[] = {1.0, 2.0, 3.0};
  const int filter_length = 3;
  
  double* filtered_data = 0;
  
  // constant padding
  padded_data = PadArray(padded_array,in_array,in_array_length,ConstPad,pad_length,0,0);
  for (int i = 0; i < in_array_length + 2*pad_length; i++) { std::cout << padded_array[i] << " "; } std::cout << std::endl;
  
  CorrFilterPaddedArray(filtered_data,padded_data,in_array_length,filter,filter_length,filter_length/2+1);
  for (int i = 0; i < in_array_length; i++) { std::cout << filtered_data[i] << " "; } std::cout << std::endl;
  
  // replicate padding
  padded_data = PadArray(padded_array,in_array,in_array_length,ReplicatePad,pad_length,0,0);
  for (int i = 0; i < in_array_length + 2*pad_length; i++) { std::cout << padded_array[i] << " "; } std::cout << std::endl;
  
  CorrFilterPaddedArray(filtered_data,padded_data,in_array_length,filter,filter_length,filter_length/2+1);
  for (int i = 0; i < in_array_length; i++) { std::cout << filtered_data[i] << " "; } std::cout << std::endl;
  
  // symmetric padding
  padded_data = PadArray(padded_array,in_array,in_array_length,SymmetricPad,pad_length,0,0);
  for (int i = 0; i < in_array_length + 2*pad_length; i++) { std::cout << padded_array[i] << " "; } std::cout << std::endl;
  
  CorrFilterPaddedArray(filtered_data,padded_data,in_array_length,filter,filter_length,filter_length/2+1);
  for (int i = 0; i < in_array_length; i++) { std::cout << filtered_data[i] << " "; } std::cout << std::endl;
  
  // circular padding
  padded_data = PadArray(padded_array,in_array,in_array_length,CircularPad,pad_length,0,0);
  for (int i = 0; i < in_array_length + 2*pad_length; i++) { std::cout << padded_array[i] << " "; } std::cout << std::endl;
  
  CorrFilterPaddedArray(filtered_data,padded_data,in_array_length,filter,filter_length,filter_length/2+1);
  for (int i = 0; i < in_array_length; i++) { std::cout << filtered_data[i] << " "; } std::cout << std::endl;

  return 0;
}
#endif