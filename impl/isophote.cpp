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
#include "isophote.hpp"
#include "epsilon.hpp"
#include <iostream>
#include <math.h>

#ifdef _OPENMP_ISOPHOTE_CALCULATION
#include <omp.h>
#endif

#ifndef SQR
#define SQR(x) ((x)*(x))
#endif

#ifndef CUBIC
#define CUBIC(x) ((x)*(x)*(x))
#endif

#ifdef __MEX
#include <mex.h>
#endif

#ifndef _ROWMAJOR_INDEX
#define _ROWMAJOR_INDEX(x,y,width,height) (width*y + x) 
#endif

#ifndef _COLUMNMAJOR_INDEX
#define _COLUMNMAJOR_INDEX(x,y,width,height) (height*x + y)
#endif

template <typename T> T _sqrt(T x);

template <>
inline float
_sqrt<float>(float x)
{
    return sqrtf(x);
}

template <>
inline double
_sqrt<double>(double x)
{
    return sqrt(x);
}

template <>
inline long double
_sqrt<long double>(long double x)
{
    return sqrtl(x);
}

template <typename T, typename T_size>
void
CalculateAccumulator(const T* k, const T* c, const T* dx, const T* dy, T_size width, T_size height, T* acc, bool zero_acc, bool row_major)
{
    /* Set accumulator cells to zero if necessary */
    if (!zero_acc)
        for (T_size i(0); i < width*height; i++)
            acc[i] = T(0);
    
    /* Calculate the accumulator */
    for (T_size y(0); y < height; y++)
    {
        for (T_size x(0); x < width; x++)
        {
            if (row_major) // data is stored in row-major order
            {
                const T_size idx = _ROWMAJOR_INDEX(x,y,width,height);
                const T cval = c[idx];
                const T kval = k[idx];
                if (kval < 0)
                {
                    // check radius - does it make any sense?
                    //const T min_radius = 2; // votes for radii < 2 usually are basically noise
                    //const T max_radius = 20;
                    //if (SQR(dx[idx]) + SQR(dy[idx]) < SQR(min_radius))
                    //    continue;
                    //if (SQR(dx[idx]) + SQR(dy[idx]) > SQR(max_radius))
                    //    continue;
                    const T_size indx = T_size(dx[idx] + T(0.5)) + x; // +0.5 for cheap round
                    const T_size indy = T_size(dy[idx] + T(0.5)) + y; // +0.5 for cheap round
                    if (indx < 0 || indx > width-1 || indy < 0 || indy > height - 1) // @NOTE: actually checking indy or indx < 0 problematic for unsigned types; however, due to the limited range and tight boundaries the potentially wrap-around is not problematic in combination with indx < width and indy < height ...
                        continue;
                    const T_size accidx = _ROWMAJOR_INDEX(indx,indy,width,height);
                    acc[accidx] += cval;
                }
            }
            else  // data is stored in column-major order
            {
                const T_size idx = _COLUMNMAJOR_INDEX(x,y,width,height);
                const T cval = c[idx];
                const T kval = k[idx];
                if (kval < 0)
                {
                    // check radius - does it make any sense?
                    //const T min_radius = 2; // votes for radii < 2 usually are basically noise
                    //const T max_radius = 20;
                    //if (SQR(dx[idx]) + SQR(dy[idx]) < SQR(min_radius))
                    //    continue;
                    //if (SQR(dx[idx]) + SQR(dy[idx]) > SQR(max_radius))
                    //    continue;
                    const T_size indx = T_size(dx[idx] + T(0.5)) + x; // +0.5 for cheap round
                    const T_size indy = T_size(dy[idx] + T(0.5)) + y; // +0.5 for cheap round
                    // valid index
                    if (indx < 0 || indx > (width-1) || indy < 0 || indy > (height-1)) // @NOTE: actually checking indy or indx < 0 problematic for unsigned types; however, due to the limited range and tight boundaries the potentially wrap-around is not problematic in combination with indx < width and indy < height ...
                        continue;
                    const T_size accidx = _COLUMNMAJOR_INDEX(indx,indy,width,height);
                    acc[accidx] += cval;
                }
            }
        }
    }
}
template void CalculateAccumulator(const float*,const float*,const float*,const float*,int,int,float*,bool,bool);
template void CalculateAccumulator(const double*,const double*,const double*,const double*,int,int,double*,bool,bool);
template void CalculateAccumulator(const float*,const float*,const float*,const float*,size_t,size_t,float*,bool,bool);
template void CalculateAccumulator(const double*,const double*,const double*,const double*,size_t,size_t,double*,bool,bool);
template void CalculateAccumulator(const float*,const float*,const float*,const float*,unsigned int,unsigned int,float*,bool,bool);
template void CalculateAccumulator(const double*,const double*,const double*,const double*,unsigned int,unsigned int,double*,bool,bool);

template <typename T, typename T_size>
void
CalculateAccumulatorPosK(const T* k, const T* c, const T* dx, const T* dy, T_size width, T_size height, T* acc, bool zero_acc, bool row_major)
{
    /* Set accumulator cells to zero if necessary */
    if (!zero_acc)
        for (T_size i(0); i < width*height; i++)
            acc[i] = T(0);
    
    /* Calculate the accumulator */
    for (T_size y(0); y < height; y++)
    {
        for (T_size x(0); x < width; x++)
        {
            if (row_major) // data is stored in row-major order
            {
                const T_size idx = _ROWMAJOR_INDEX(x,y,width,height);
                const T cval = c[idx];
                const T kval = k[idx];
                if (kval > 0)
                {
                    // check radius - does it make any sense?
                    //const T min_radius = 2; // votes for radii < 2 usually are basically noise
                    //const T max_radius = 20;
                    //if (SQR(dx[idx]) + SQR(dy[idx]) < SQR(min_radius))
                    //    continue;
                    //if (SQR(dx[idx]) + SQR(dy[idx]) > SQR(max_radius))
                    //    continue;
                    const T_size indx = T_size(dx[idx] + T(0.5)) + x; // +0.5 for cheap round
                    const T_size indy = T_size(dy[idx] + T(0.5)) + y; // +0.5 for cheap round
                    if (indx < 0 || indx > width-1 || indy < 0 || indy > height - 1) // @NOTE: actually checking indy or indx < 0 is problematic/senseless for unsigned types; however, due to the limited range and tight boundaries the potentially wrap-around is not problematic in combination with indx < width and indy < height ...
                        continue;
                    const T_size accidx = _ROWMAJOR_INDEX(indx,indy,width,height);
                    acc[accidx] += cval;
                }
            }
            else  // data is stored in column-major order
            {
                const T_size idx = _COLUMNMAJOR_INDEX(x,y,width,height);
                const T cval = c[idx];
                const T kval = k[idx];
                if (kval > 0)
                {
                    // check radius - does it make any sense?
                    //const T min_radius = 2; // votes for radii < 2 usually are basically noise
                    //const T max_radius = 20;
                    //if (SQR(dx[idx]) + SQR(dy[idx]) < SQR(min_radius))
                    //    continue;
                    //if (SQR(dx[idx]) + SQR(dy[idx]) > SQR(max_radius))
                    //    continue;
                    const T_size indx = T_size(dx[idx] + T(0.5)) + x; // +0.5 for cheap round
                    const T_size indy = T_size(dy[idx] + T(0.5)) + y; // +0.5 for cheap round
                    // valid index
                    if (indx < 0 || indx > (width-1) || indy < 0 || indy > (height-1)) // @NOTE: actually checking indy or indx < 0 is problematic/senseless for unsigned types; however, due to the limited range and tight boundaries the potentially wrap-around is not problematic in combination with indx < width and indy < height ...
                        continue;
                    const T_size accidx = _COLUMNMAJOR_INDEX(indx,indy,width,height);
                    acc[accidx] += cval;
                }
            }
        }
    }
}
template void CalculateAccumulatorPosK(const float*,const float*,const float*,const float*,int,int,float*,bool,bool);
template void CalculateAccumulatorPosK(const double*,const double*,const double*,const double*,int,int,double*,bool,bool);
template void CalculateAccumulatorPosK(const float*,const float*,const float*,const float*,size_t,size_t,float*,bool,bool);
template void CalculateAccumulatorPosK(const double*,const double*,const double*,const double*,size_t,size_t,double*,bool,bool);
template void CalculateAccumulatorPosK(const float*,const float*,const float*,const float*,unsigned int,unsigned int,float*,bool,bool);
template void CalculateAccumulatorPosK(const double*,const double*,const double*,const double*,unsigned int,unsigned int,double*,bool,bool);

template <typename T, typename T_size>
void
CalculateIsophoteInformation(const T* Lx, const T* Ly, const T* Lxx, const T* Lxy, const T* Lyy, T_size width, T_size height, T* k, T* c, T* dx, T* dy, 
                             T* tmpT1, T* tmpLx2, T* tmpLy2)//, T* tmpLxx2 = NULL, T* tmpLxy2 = NULL, T* tmpLyy2 = NULL) // allow reuse of temporary variables and avoid memory allocation madness
{
    const T_size size(width*height);
    
    // shortcuts
    T* T1 = NULL;
    T* Lx2 = NULL;
    T* Ly2 = NULL;
    
    /* Allocate space for temporary variables if necessary */
    if (tmpT1 == NULL)
        T1 = new T[size];
    else
        T1 = tmpT1;
    if (tmpLx2 == NULL)
        Lx2 = new T[size];
    else
        Lx2 = tmpLx2;
    if (tmpLy2 == NULL)
        Ly2 = new T[size];
    else
        Ly2 = tmpLy2;
    
    /* Pre-calculate some terms */
#ifdef _OPENMP_ISOPHOTE_CALCULATION
#pragma omp parallel for
#endif
    for (T_size i(0); i < size; i++)
    {
        Lx2[i] = SQR(Lx[i]);
        Ly2[i] = SQR(Ly[i]);
        T1[i] = (2 * Lx[i] * Lxy[i] * Ly[i]) - (Lx2[i] * Lyy[i]) - (Ly2[i] * Lxx[i]); // + epsilon<T>(); // +epsilon to avoid division by zero (and keep defined behavior) -- if adding epsilon => what about the sign???
        if (T1[i] == 0) 
            T1[i] = epsilon<T>();
    }
#ifdef _OPENMP_ISOPHOTE_CALCULATION
#pragma omp barrier
#endif

    /* Main calculation (actually we could merge this with the pre-calculation loop) */
#ifdef _OPENMP_ISOPHOTE_CALCULATION
#pragma omp parallel for
#endif   
    for (T_size i(0); i < size; i++)
    {
        T tmp = (Lx2[i] + Ly2[i]);
        
        k[i] = T1[i] / (_sqrt(CUBIC(tmp)) + epsilon<T>()); // +epsilon to avoid division by zero (and keep defined behavior)
        c[i] = _sqrt<T>(SQR(Lxx[i]) + 2*SQR(Lxy[i]) + SQR(Lyy[i]));
        dx[i] = (Lx[i] * tmp) / T1[i];
        dy[i] = (Ly[i] * tmp) / T1[i];
    }
#ifdef _OPENMP_ISOPHOTE_CALCULATION
#pragma omp barrier
#endif
   
    /* Free allocated temporary memory */ 
    if (tmpT1 == NULL)
        delete [] T1;
    if (tmpLx2 == NULL)
        delete [] Lx2;
    if (tmpLy2 == NULL)
        delete [] Ly2;
}
template void CalculateIsophoteInformation(const float*,const float*,const float*,const float*,const float*,int,int,float*,float*,float*,float*,float*,float*,float*);
template void CalculateIsophoteInformation(const double*,const double*,const double*,const double*,const double*,int,int,double*,double*,double*,double*,double*,double*,double*);
template void CalculateIsophoteInformation(const float*,const float*,const float*,const float*,const float*,size_t,size_t,float*,float*,float*,float*,float*,float*,float*);
template void CalculateIsophoteInformation(const double*,const double*,const double*,const double*,const double*,size_t,size_t,double*,double*,double*,double*,double*,double*,double*);
template void CalculateIsophoteInformation(const float*,const float*,const float*,const float*,const float*,unsigned int,unsigned int,float*,float*,float*,float*,float*,float*,float*);
template void CalculateIsophoteInformation(const double*,const double*,const double*,const double*,const double*,unsigned int,unsigned int,double*,double*,double*,double*,double*,double*,double*);

template <typename T, typename T_size>
void
CalculateIsophoteInformation(const T* Lx, const T* Ly, const T* Lxx, const T* Lxy, const T* Lyy, T_size width, T_size height, T* k, T* c, T* dx, T* dy, 
                             T_size roi_x_min, T_size roi_y_min, T_size roi_width, T_size roi_height,
                             T* tmpT1, T* tmpLx2, T* tmpLy2)
{
    const T_size size(width*height);
    
    // shortcuts
    T* T1 = NULL;
    T* Lx2 = NULL;
    T* Ly2 = NULL;
    
    /* Allocate space for temporary variables if necessary */
    if (tmpT1 == NULL)
        T1 = new T[size];
    else
        T1 = tmpT1;
    if (tmpLx2 == NULL)
        Lx2 = new T[size];
    else
        Lx2 = tmpLx2;
    if (tmpLy2 == NULL)
        Ly2 = new T[size];
    else
        Ly2 = tmpLy2;
    
    // pre-calculate array borders
    const T_size x_max = roi_x_min + roi_width - 1;
    const T_size y_max = roi_y_min + roi_height - 1;

    /* Pre-calculate some terms */
    for (T_size y(roi_y_min); y <= y_max; y++)
    {
        T_size i = y*width + roi_x_min; // row-major order index
        for (T_size x(roi_x_min); x <= x_max; x++, i++)
        {
            Lx2[i] = SQR(Lx[i]);
            Ly2[i] = SQR(Ly[i]);
            T1[i] = (2 * Lx[i] * Lxy[i] * Ly[i]) - (Lx2[i] * Lyy[i]) - (Ly2[i] * Lxx[i]); // + epsilon<T>(); // +epsilon to avoid division by zero (and keep defined behavior) -- if adding epsilon => what about the sign???
            if (T1[i] == 0) 
                T1[i] = epsilon<T>();
        }
    }

    /* Main calculation (actually we could merge this with the pre-calculation loop) */
    for (T_size y(roi_y_min); y <= y_max; y++)
    {
        T_size i = y*width + roi_x_min; // row-major order index
        for (T_size x(roi_x_min); x <= x_max; x++, i++)
        {
            T tmp = (Lx2[i] + Ly2[i]);
        
            k[i] = T1[i] / (_sqrt(CUBIC(tmp)) + epsilon<T>()); // +epsilon to avoid division by zero (and keep defined behavior)
            c[i] = _sqrt<T>(SQR(Lxx[i]) + 2*SQR(Lxy[i]) + SQR(Lyy[i]));
            dx[i] = (Lx[i] * tmp) / T1[i];
            dy[i] = (Ly[i] * tmp) / T1[i];
        }
    }
   
    /* Free allocated temporary memory */ 
    if (tmpT1 == NULL)
        delete [] T1;
    if (tmpLx2 == NULL)
        delete [] Lx2;
    if (tmpLy2 == NULL)
        delete [] Ly2;
}
template void CalculateIsophoteInformation(const float*,const float*,const float*,const float*,const float*,int,int,float*,float*,float*,float*,int,int,int,int,float*,float*,float*);
template void CalculateIsophoteInformation(const double*,const double*,const double*,const double*,const double*,int,int,double*,double*,double*,double*,int,int,int,int,double*,double*,double*);
template void CalculateIsophoteInformation(const float*,const float*,const float*,const float*,const float*,size_t,size_t,float*,float*,float*,float*,size_t,size_t,size_t,size_t,float*,float*,float*);
template void CalculateIsophoteInformation(const double*,const double*,const double*,const double*,const double*,size_t,size_t,double*,double*,double*,double*,size_t,size_t,size_t,size_t,double*,double*,double*);
template void CalculateIsophoteInformation(const float*,const float*,const float*,const float*,const float*,unsigned int,unsigned int,float*,float*,float*,float*,unsigned int,unsigned int,unsigned int,unsigned int,float*,float*,float*);
template void CalculateIsophoteInformation(const double*,const double*,const double*,const double*,const double*,unsigned int,unsigned int,double*,double*,double*,double*,unsigned int,unsigned int,unsigned int,unsigned int,double*,double*,double*);


#ifdef __MEX
/* MATLAB interface 
 * -> be aware that MATLAB uses column-major data storage
 */
void
mexFunction(int nlhs, mxArray* plhs[],
            int nrhs, const mxArray* prhs[])
{
    /* Check number of input parameters. */
    if (nrhs != 5)
        mexErrMsgTxt("Wrong number of input arguments. Input should be: Lx Ly Lxx Lxy Lyy!\n");
    if (nlhs != 4 && nlhs != 5)
        mexErrMsgTxt("Wrong number of output argumens. Output should be: k c dx dy [acc]!\n");
    
    for (int i = 0; i < 4; i++)
        if (mxGetClassID(prhs[i]) != mxGetClassID(prhs[i+1]))
            mexErrMsgTxt("Input arguments need to have of the same data type!\n");
    mxClassID inputClass = mxGetClassID(prhs[0]);
        
    if (mxGetClassID(prhs[0]) != mxSINGLE_CLASS && mxGetClassID(prhs[0]) != mxDOUBLE_CLASS)
        mexErrMsgTxt("Input arguments need to single or double type!\n");
    
    /* height/width */
    for (int i = 0; i < 4; i++)
        if (mxGetM(prhs[i]) != mxGetM(prhs[i+1]) || mxGetN(prhs[i]) != mxGetN(prhs[i+1]))
            mexErrMsgTxt("Input arguments need to have of the dimensions\n");
    mwSize height = mxGetM(prhs[0]);
    mwSize width = mxGetN(prhs[0]);
    
    /* Get pointers to input matrices */
    const mxArray* mxLx = prhs[0];
    const mxArray* mxLy = prhs[1];
    const mxArray* mxLxx = prhs[2];
    const mxArray* mxLxy = prhs[3];
    const mxArray* mxLyy = prhs[4];
    void* vLx = (void*)mxGetPr(mxLx);
    void* vLy = (void*)mxGetPr(mxLy);
    void* vLxx = (void*)mxGetPr(mxLxx);
    void* vLxy = (void*)mxGetPr(mxLxy);
    void* vLyy = (void*)mxGetPr(mxLyy);
    
    /* Create output matrices */
    mxArray* mxK = mxCreateNumericMatrix(height,width,inputClass,mxREAL);
    mxArray* mxC = mxCreateNumericMatrix(height,width,inputClass,mxREAL);
    mxArray* mxDx = mxCreateNumericMatrix(height,width,inputClass,mxREAL);
    mxArray* mxDy = mxCreateNumericMatrix(height,width,inputClass,mxREAL);
    void* vK = (void*)mxGetPr(mxK);
    void* vC = (void*)mxGetPr(mxC);
    void* vDx = (void*)mxGetPr(mxDx);
    void* vDy = (void*)mxGetPr(mxDy);
    plhs[0] = mxK;
    plhs[1] = mxC;
    plhs[2] = mxDx;
    plhs[3] = mxDy;
    
    mxArray* mxAcc = NULL;
    void* vAcc = NULL;
    if (nlhs == 5) // calculate accumulator
    {
        mxAcc = mxCreateNumericMatrix(height,width,inputClass,mxREAL);
        vAcc = (void*)mxGetPr(mxAcc);
        plhs[4] = mxAcc;
    }
    
    switch (inputClass)
    {
        case mxSINGLE_CLASS:
            CalculateIsophoteInformation((float*)vLx,(float*)vLy,(float*)vLxx,(float*)vLxy,(float*)vLyy,width,height,(float*)vK,(float*)vC,(float*)vDx,(float*)vDy);
            if (mxAcc != 0) // Calculate accumulator
                CalculateAccumulator((float*)vK,(float*)vC,(float*)vDx,(float*)vDy,width,height,(float*)vAcc,true,false);
            break;
        case mxDOUBLE_CLASS:
            CalculateIsophoteInformation((double*)vLx,(double*)vLy,(double*)vLxx,(double*)vLxy,(double*)vLyy,width,height,(double*)vK,(double*)vC,(double*)vDx,(double*)vDy);
            if (mxAcc != 0) // Calculate accumulator
                CalculateAccumulator((double*)vK,(double*)vC,(double*)vDx,(double*)vDy,width,height,(double*)vAcc,true,false);
            break;
        default:
            mexErrMsgTxt("Unsupported data type!\n"); // this should never happen
            break;
    }
}
#endif
