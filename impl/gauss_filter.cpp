/* Implementation Gaussian filters and their derivatives
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
#include "gauss_filter.hpp"
#include <stdint.h>
#include <math.h>
#include <assert.h>

#ifdef __MEX
#include <mex.h>
#include "separable_filter.hpp"
#endif

#ifndef SQR
#define SQR(x) ((x)*(x))
#endif

#ifndef SQRT
#define SQRT(x) sqrt((double)(x))
#endif

#ifndef NULL
#define NULL (0)
#endif

#define _MEX_FLIP_FILTERS // by flipping the filters we get the same results as with conventional convolution

template <typename T, typename S>
S
GetGaussLength(const T& sigma)
{
    return GetGaussSupport<T,S>(sigma)*2 + 1;
}
template int GetGaussLength(const float& sigma);
template int GetGaussLength(const double& sigma);

template <typename T, typename S>
S
GetGaussSupport(const T& sigma, T* Ax)
{
    const S Wx = (S)ceil((double)sigma * 3.0); // half width, i.e. filter defines on [-Wx:Wx]

    if (Ax != NULL)
    {
        int c = 0;
        for (int x = -Wx; x <= Wx; x++)
            Ax[c++] = T(x);
    }

    return Wx;
}
template int GetGaussSupport(const float&,float*);
template int GetGaussSupport(const double&,double*);

template <typename T>
T*
CreateGauss(const T& sigma, bool normalize, const T* _Ax, T* _out)
{
    int Wx = GetGaussSupport<T,int>(sigma);
    int length = GetGaussLength<T,int>(sigma);
    T* __Ax = NULL;
    if (_Ax == NULL)
    {
        __Ax = new T[length];
        int c = 0;
        for (int x = -Wx; x <= Wx; x++)
            __Ax[c++] = T(x);
    }
    const T* Ax = (_Ax != NULL ? _Ax : __Ax);
    
    T* out = (_out != NULL ? _out : new T[length]);
    for (int i = 0; i < length; i++)
        out[i] = exp(-1.0 * SQR(Ax[i]) / (2*SQR(sigma)));
    if (normalize)
        for (int i = 0; i < length; i++)
            out[i] *= 1 / (SQRT(2*M_PI*sigma));
    
    if (__Ax != NULL)
        delete [] __Ax;
    
    return out;
}
template float* CreateGauss(const float& sigma,bool,const float*,float*);
template double* CreateGauss(const double& sigma,bool,const double*,double*);

template <typename T>
T*
CreateGaussFirstDeriv(const T& sigma, bool normalize, const T* _Ax, T* _out)
{
    int Wx = GetGaussSupport<T,int>(sigma);
    int length = GetGaussLength<T,int>(sigma);
    T* __Ax = NULL;
    if (_Ax == NULL)
    {
        __Ax = new T[length];
        int c = 0;
        for (int x = -Wx; x <= Wx; x++)
            __Ax[c++] = T(x);
    }
    const T* Ax = (_Ax != 0 ? _Ax : __Ax);

    T* out = (_out != NULL ? _out : new T[length]);
    for (int i = 0; i < length; i++)
        out[i] = (-Ax[i] / SQR(sigma)) * exp(-1.0 * SQR(Ax[i]) / (2*SQR(sigma)));
    if (normalize)
        for (int i = 0; i < length; i++)
            out[i] *= 1 / (SQRT(2*M_PI*sigma));
   
    if(__Ax != NULL) 
        delete [] __Ax;
    
    return out;
}
template float* CreateGaussFirstDeriv(const float& sigma,bool,const float*,float*);
template double* CreateGaussFirstDeriv(const double& sigma,bool,const double*,double*);

template <typename T>
T*
CreateGaussSecondDeriv(const T& sigma, bool normalize, const T* _Ax, T* _out)
{
    int Wx = GetGaussSupport<T,int>(sigma);
    int length = GetGaussLength<T,int>(sigma);
    T* __Ax = NULL;
    if (_Ax == NULL)
    {
        __Ax = new T[length];
        int c = 0;
        for (int x = -Wx; x <= Wx; x++)
            __Ax[c++] = T(x);
    }
    const T* Ax = (_Ax != NULL ? _Ax : __Ax);
    
    T* out = (_out != NULL ? _out : new T[length]);
    for (int i = 0; i < length; i++)
        out[i] = (SQR(Ax[i]) / (SQR(SQR(sigma)) * exp(SQR(Ax[i]) / (2*SQR(sigma)))) - 1.0 / (SQR(sigma) * exp(SQR(Ax[i]) / (2*SQR(sigma)))));
    if (normalize)
        for (int i = 0; i < length; i++)
            out[i] *= 1 / (SQRT(2*M_PI*sigma));
    
    if (__Ax !=  NULL)
        delete [] __Ax;
    
    return out;
}
template float* CreateGaussSecondDeriv(const float& sigma,bool,const float*,float*);
template double* CreateGaussSecondDeriv(const double& sigma,bool,const double*,double*);

#ifdef __MEX
/* MATLAB interface 
 * -> be aware that MATLAB uses column-major data storage
 */
void
mexFunction(int nlhs, mxArray* plhs[],
            int nrhs, const mxArray* prhs[])
{
    /* Check number of input parameters. */
    if (nrhs < 2 || nrhs > 3)
        mexErrMsgTxt("Wrong number of input arguments. Input should be: \n\timage [sigma]\n\timage [row_sigma] [col_sigma].\n");
    if (nlhs != 5 && nlhs != 10)
        mexErrMsgTxt("Wrong number of output arguments. Output should be: Lx Ly Lxx Lxy Lyy [hx hy hxx hxy hyy]\n");
    if (nlhs == 10)
        mexErrMsgTxt("Output of filters currently not supported. Sorry, dude.\n");
    
    /* Check type of input parameters. */
    mwSize height = 0;
    mwSize width = 0;
    const mxArray* mxImage = NULL;
    mxClassID mxImageClass = mxUNKNOWN_CLASS;
    const void* vImage = NULL;
    mwSize row_filter_length  = 0;
    mxClassID mxRowFilterClass = mxUNKNOWN_CLASS;
    const mxArray* mxRowFilter = NULL;
    const void* vRowFilter = NULL;
    mwSize col_filter_length = 0;
    mxClassID mxColFilterClass = mxUNKNOWN_CLASS;
    const mxArray* mxColFilter = NULL;
    const void* vColFilter = NULL;
    
    if (nrhs >= 1)
    {
        mxImage = prhs[0];
        vImage = (void*)mxGetPr(mxImage);
        height = mxGetM(mxImage);
        width = mxGetN(mxImage);
        mxImageClass = mxGetClassID(mxImage);
    }
    if (nrhs >= 2)
    {
        mxRowFilter = prhs[1];
        vRowFilter = (void*)mxGetPr(mxRowFilter);
        row_filter_length = mxGetM(mxRowFilter) * mxGetN(mxRowFilter);
        mxRowFilterClass = mxGetClassID(mxRowFilter);
        if (row_filter_length != 1)
            mexErrMsgTxt("sigma parameter should be a scalar!\n");
    }
    if (nrhs >= 3)
    {
        mxColFilter = prhs[2];
        vColFilter = (void*)mxGetPr(mxColFilter);
        col_filter_length = mxGetM(mxColFilter) * mxGetN(mxColFilter);
        mxColFilterClass = mxGetClassID(mxColFilter);
        if (col_filter_length != 1)
            mexErrMsgTxt("sigma parameter should be a scalar!\n");
        if (mxGetClassID(mxRowFilter) != mxGetClassID(mxColFilter))
        {
            char buf[256];
            sprintf(buf,"Row and column sigma parameter have different classes, i.e. %s (%d) != %s (%d).\n", mxGetClassName(mxRowFilter), mxGetClassID(mxRowFilter), mxGetClassName(mxColFilter), mxGetClassID(mxColFilter));
            mexErrMsgTxt(buf);
        }
    }
    
    /* Create output arrays */
    mxArray* mxLx = mxCreateNumericMatrix(height,width,mxRowFilterClass,mxREAL); // filtered image has the type of the filter (which is supposed to be double or single)
    plhs[0] = mxLx;
    void* vLx = (void*)mxGetPr(mxLx);
    mxArray* mxLy = mxCreateNumericMatrix(height,width,mxRowFilterClass,mxREAL);
    plhs[1] = mxLy;
    void* vLy = (void*)mxGetPr(mxLy);
    mxArray* mxLxx = mxCreateNumericMatrix(height,width,mxRowFilterClass,mxREAL);
    plhs[2] = mxLxx;
    void* vLxx = (void*)mxGetPr(mxLxx);
    mxArray* mxLxy = mxCreateNumericMatrix(height,width,mxRowFilterClass,mxREAL);
    plhs[3] = mxLxy;
    void* vLxy = (void*)mxGetPr(mxLxy);
    mxArray* mxLyy = mxCreateNumericMatrix(height,width,mxRowFilterClass,mxREAL);
    plhs[4] = mxLyy;
    void* vLyy = (void*)mxGetPr(mxLyy);
    
    // transpose the image to get a row-major image for filtering
    if (mxImageClass != mxUINT8_CLASS)
        mexErrMsgTxt("Currently only UINT8 images are supported\n");
    uint8_t* transposedImage = new uint8_t[width*height];
    Transpose((uint8_t*)vImage,height,width,transposedImage);
    
    double row_sigma = 0;
    double col_sigma = 0;
    if (nrhs == 2)
    {
        switch (mxRowFilterClass)
        {
            case mxSINGLE_CLASS:
                {
                    float* fRowFilter = (float*)vRowFilter;
                    row_sigma = (double)fRowFilter[0];
                    col_sigma = row_sigma;
                }
                break;
            case mxDOUBLE_CLASS:
                {
                    double* fRowFilter = (double*)vRowFilter;
                    row_sigma = fRowFilter[0];
                    col_sigma = row_sigma;
                }
                break;
            default:
                mexErrMsgTxt("Currently only single and double are supported filter data types\n");
                break;
        }
    }
    else if (nrhs == 3)
    {
        switch (mxRowFilterClass)
        {
            case mxSINGLE_CLASS:
                {
                    float* fRowFilter = (float*)vRowFilter;
                    float* fColFilter = (float*)vColFilter;
                    row_sigma = (double)fRowFilter[0];
                    col_sigma = (double)fColFilter[0];
                }
                break;
            case mxDOUBLE_CLASS:
                {
                    double* fRowFilter = (double*)vRowFilter;
                    double* fColFilter = (double*)vColFilter;
                    row_sigma = fRowFilter[0];
                    col_sigma = fColFilter[0];
                }
                break;
            default:
                mexErrMsgTxt("Currently only single and double are supported filter data types. Data type is \n");
                break;
        }
    }
    else
        assert(false);
        
    switch (mxRowFilterClass)
    {
        case mxSINGLE_CLASS:
            {
                // create temporary matrices for 2-step filtering
                float* tmp = new float[width*height];
                for (int i = 0; i < width*height; i++)
                    tmp[i] = (float)0; // zero tmp (necessary, because otherwise the borders won't be initialized)
                
                // create filters
                row_filter_length = GetGaussLength<double,int>(row_sigma);
                float* row_g = CreateGauss((float)row_sigma);
                float* row_gp = CreateGaussFirstDeriv((float)row_sigma);
                float* row_gpp = CreateGaussSecondDeriv((float)row_sigma);
                col_filter_length = GetGaussLength<double,int>(col_sigma);
                float* col_g = CreateGauss((float)col_sigma);
                float* col_gp = CreateGaussFirstDeriv((float)col_sigma);
                float* col_gpp = CreateGaussSecondDeriv((float)col_sigma);
                
//#define _MEXPRINTF_ARRAY(array,length) mexPrintf(#array ": "); for (int i = 0; i < length; i++) mexPrintf("%f ", array[i]); mexPrintf("\n");
                //_MEXPRINTF_ARRAY(row_g,row_filter_length)
                //_MEXPRINTF_ARRAY(row_gp,row_filter_length)
                //_MEXPRINTF_ARRAY(row_gpp,row_filter_length)
                //_MEXPRINTF_ARRAY(col_g,col_filter_length)
                //_MEXPRINTF_ARRAY(col_gp,col_filter_length)
                //_MEXPRINTF_ARRAY(col_gpp,col_filter_length)
                
#ifdef _MEX_FLIP_FILTERS
                FlipArray(row_g,row_filter_length);
                FlipArray(row_gp,row_filter_length);
                FlipArray(row_gpp,row_filter_length);
                FlipArray(col_g,col_filter_length);
                FlipArray(col_gp,col_filter_length);
                FlipArray(col_gpp,col_filter_length);
#endif
                                
                // Calculate Ly and Lyy
                RowFilter(transposedImage,width,height,row_g,row_filter_length,tmp,true);
                RowFilter(tmp,height,width,col_gp,col_filter_length,(float*)vLy,false); // we don't need to transpose the output here, because matlab wants column-major
                RowFilter(tmp,height,width,col_gpp,col_filter_length,(float*)vLyy,false); // we don't need to transpose the output here, because matlab wants column-major
                // Calculate Lx and Lxy
                RowFilter(transposedImage,width,height,row_gp,row_filter_length,tmp,true);
                RowFilter(tmp,height,width,col_g,col_filter_length,(float*)vLx,false); // we don't need to transpose the output here, because matlab wants column-major
                RowFilter(tmp,height,width,col_gp,col_filter_length,(float*)vLxy,false); // we don't need to transpose the output here, because matlab wants column-major
                // Calculate Lxx
                RowFilter(transposedImage,width,height,row_gpp,row_filter_length,tmp,true);
                RowFilter(tmp,height,width,col_g,col_filter_length,(float*)vLxx,false); // we don't need to transpose the output here, because matlab wants column-major
                
                // free data
                delete [] tmp;
                delete [] row_g;
                delete [] row_gp;
                delete [] row_gpp;
                delete [] col_g;
                delete [] col_gp;
                delete [] col_gpp;
            }
            break;
        case mxDOUBLE_CLASS:
            {
                // create temporary matrices for 2-step filtering
                double* tmp = new double[width*height];
                for (int i = 0; i < width*height; i++)
                    tmp[i] = (double)0; // zero tmp (necessary, because otherwise the borders won't be initialized)
                
                // create filters
                row_filter_length = GetGaussLength<double,int>(row_sigma);
                double* row_g = CreateGauss((double)row_sigma);
                double* row_gp = CreateGaussFirstDeriv((double)row_sigma);
                double* row_gpp = CreateGaussSecondDeriv((double)row_sigma);
                col_filter_length = GetGaussLength<double,int>(col_sigma);
                double* col_g = CreateGauss((double)col_sigma);
                double* col_gp = CreateGaussFirstDeriv((double)col_sigma);
                double* col_gpp = CreateGaussSecondDeriv((double)col_sigma);

#ifdef _MEX_FLIP_FILTERS
                FlipArray(row_g,row_filter_length);
                FlipArray(row_gp,row_filter_length);
                FlipArray(row_gpp,row_filter_length);
                FlipArray(col_g,col_filter_length);
                FlipArray(col_gp,col_filter_length);
                FlipArray(col_gpp,col_filter_length);
#endif
                                
                // Calculate Ly and Lyy
                RowFilter(transposedImage,width,height,row_g,row_filter_length,tmp,true);
                RowFilter(tmp,height,width,col_gp,col_filter_length,(double*)vLy,false); // we don't need to transpose the output here, because matlab wants column-major
                RowFilter(tmp,height,width,col_gpp,col_filter_length,(double*)vLyy,false); // we don't need to transpose the output here, because matlab wants column-major
                // Calculate Lx and Lxy
                RowFilter(transposedImage,width,height,row_gp,row_filter_length,tmp,true);
                RowFilter(tmp,height,width,col_g,col_filter_length,(double*)vLx,false); // we don't need to transpose the output here, because matlab wants column-major
                RowFilter(tmp,height,width,col_gp,col_filter_length,(double*)vLxy,false); // we don't need to transpose the output here, because matlab wants column-major
                // Calculate Lxx
                RowFilter(transposedImage,width,height,row_gpp,row_filter_length,tmp,true);
                RowFilter(tmp,height,width,col_g,col_filter_length,(double*)vLxx,false); // we don't need to transpose the output here, because matlab wants column-major
                
                // free data
                delete [] tmp;
                delete [] row_g;
                delete [] row_gp;
                delete [] row_gpp;
                delete [] col_g;
                delete [] col_gp;
                delete [] col_gpp;
            }
            break;
        default:
            mexErrMsgTxt("Currently only single and double are supported filter data types\n");
            break;
    }
    
    delete [] transposedImage;
    
    return;
}
#endif
