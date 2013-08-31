/** Implementation of efficient image filtering for small filter sizes (i.e. we do not perform the Fourier-Transform for filtering)
 *
 *  \author B. Schauerte
 *  \email  <schauerte@kit.edu>
 *  \date   2011
 */
#include "separable_filter.hpp"
#include "gauss_filter.hpp"
#include <iostream>
#include <assert.h>
#include <stdint.h>
#include <math.h>

#include <algorithm> // required for std::swap

#ifdef __MEX
#include "mex.h"
#include "matrix.h"
#endif

#ifdef _OPENMP_ROW_FILTER
#include <omp.h>
#endif

template <typename T, typename T_size>
inline void
FlipArray(T* array, T_size length)
{
    for (T_size i(0); i < length/2; i++)
        std::swap(array[i],array[length-1-i]);
}
/* Template instantiation. */
template void FlipArray(float*, int);
template void FlipArray(double*, int);
template void FlipArray(float*, unsigned int);
template void FlipArray(double*, unsigned int);

template <typename T, typename T_size>
bool
CheckArrayValues(const T* in, T_size length, const T& min, const T& max)
{
    for (T_size i(0); i < length; i++)
    {
        if (in[i] < min || in[i] > max)
        {
#ifdef __MEX
            mexPrintf("CheckArrayValues: in[%d]=%f (min=%f,max=%f)\n", i, in[i],min,max);
#endif
            return false;
        }
    }
        
    return true;
}

template <typename T, typename S, typename R, typename T_size>
void
RowFilter(const T* in, T_size width, T_size height, const S* filter, T_size length, R* out, bool transposeOut)
{
    /* automatically calculate the ROI */
    assert(_IS_ODD(length));
    const T_size x_min(length / 2);
    const T_size x_max(width - length / 2);
#ifdef _OPENMP_ROW_FILTER
#pragma omp parallel for
#endif
    for (T_size y = 0; y < height; y++)
    {
        if (transposeOut == true)
        {
            // input is row-major, output is column-major
            for (T_size x_anchor(x_min*height + y), x = y*width; x_anchor < x_max*height + y; x_anchor += height, x++)
            {
#ifdef __MEX
                mxAssert(x < width*height,"x >= width*height");
                mxAssert(x_anchor < width*height,"x_anchor >= width*height");                
#endif
                out[x_anchor] = 0;
                for (T_size xf(0); xf < length; xf++)
                {
                    out[x_anchor] += (R)(((S)in[x + xf]) * filter[xf]);
#ifdef __MEX
                    mxAssert((x + xf) < (y+1)*width, "(x + xf) >= (y+1)*width");
                    mxAssert((x + xf) >= y*width, "(x + xf) < y*width");
                    //char buf[256];
                    //sprintf(buf,"Array value check failed! xf=%d x=%d y=%d in[x + xf]=%f filter[xf]=%f out[x_anchor]=%f\n", xf, x, y,(float)in[x + xf],(float)filter[xf],(float)out[x_anchor]);
                    //mxAssert(CheckArrayValues(out,width*height,R(0),R(1000)),buf);
#endif          
                }
            }
        }
        else
        {
            // input is row-major, output is row-major
            const T_size _x_min = y*width + x_min;
            const T_size _x_max = y*width + x_max;
            for (T_size x_anchor(_x_min), x(y*width); x_anchor < _x_max; x_anchor++, x++)
            {
#ifdef __MEX
                mxAssert(x < width*height,"x >= width*height");
                mxAssert(x_anchor < width*height,"x_anchor >= width*height");                
#endif
                
                out[x_anchor] = 0;
                for (T_size xf = 0; xf < length; xf++)
                {
                    out[x_anchor] += (R)(((S)in[x + xf]) * filter[xf]);
#ifdef __MEX
                    mxAssert((x + xf) < (y+1)*width, "(x + xf) >= (y+1)*width");
                    mxAssert((x + xf) >= y*width, "(x + xf) < y*width");
                    //char buf[256];
                    //sprintf(buf,"Array value check failed! xf=%d x=%d y=%d in[x + xf]=%f filter[xf]=%f out[x_anchor]=%f\n", xf, x, y,(float)in[x + xf],(float)filter[xf],(float)out[x_anchor]);
                    //mxAssert(CheckArrayValues(out,width*height,R(0),R(1000)),buf);
#endif
                }
            }
        }
    }
}
// instantiate for uint8_t images
template void RowFilter(const uint8_t*, int, int, const float*, int, float*, bool);
template void RowFilter(const uint8_t*, int, int, const float*, int, double*, bool);
template void RowFilter(const uint8_t*, int, int, const double*, int, double*, bool);
template void RowFilter(const uint8_t*, int, int, const double*, int, float*, bool);
// instantiate for float images
template void RowFilter(const float*, int, int, const float*, int, float*, bool);
template void RowFilter(const float*, int, int, const float*, int, double*, bool);
template void RowFilter(const float*, int, int, const double*, int, double*, bool);
template void RowFilter(const float*, int, int, const double*, int, float*, bool);
// instantiate for double images
template void RowFilter(const double*, int, int, const float*, int, float*, bool);
template void RowFilter(const double*, int, int, const float*, int, double*, bool);
template void RowFilter(const double*, int, int, const double*, int, double*, bool);
template void RowFilter(const double*, int, int, const double*, int, float*, bool);

template <typename T, typename S, typename R, typename T_size>
void
RowFilter(const T* in, T_size width, T_size height, const S* filter, T_size length, R* out, T_size roi_x_min, T_size roi_y_min, T_size roi_width, T_size roi_height, bool isolated, bool transposeOut)
{
    /* automatically calculate the ROI */
    assert(_IS_ODD(length));
    static bool fixme = true;
    if (fixme)
        std::cerr << "RowFilter ROI implementation is incomplete! Please fix me! Currently only y_min <= y < y_min + height is supported!" << std::endl;
    fixme = false;

    //const T_size x_min_isolated = roi_x_min;
    //const T_size x_min_notisolated = roi_x_min - length / 2;
    //const T_size x_max_isolated = (roi_x_min + roi_width - 1);
    //const T_size x_max_notisolated = (roi_x_min + roi_width - 1) + length / 2;
    const T_size x_min = length / 2;
    const T_size x_max = width - length/2;
    //const T_size x_min = (isolated ? std::max(length / 2,x_min_isolated) : std::max(length / 2,x_min_notisolated)); // we do not support border replication and have to take care the no elements "outside the image" are processed
    //const T_size x_max = (isolated ? std::min(width - length / 2,x_max_isolated) : std::min(width - length/2,x_max_notisolated)); // we do not support border replication and have to take care the no elements "outside the image" are processed
    const T_size y_min = roi_y_min;
    const T_size y_max = (roi_y_min + roi_height - 1);

//    std::cout << "ROI RowFilter:" << std::endl;
//    std::cout << "  x_min=" << x_min << " x_max=" << x_max << std::endl; 
//    std::cout << "  y_min=" << y_min << " y_max=" << y_max << std::endl;

#ifdef _OPENMP_ROW_FILTER
#pragma omp parallel for
#endif
    for (T_size y = y_min; y <= y_max; y++)
    {
        if (transposeOut == true)
        {
            // input is row-major, output is column-major
            for (T_size x_anchor(x_min*height + y), x = y*width; x_anchor < x_max*height + y; x_anchor += height, x++)
            {
                out[x_anchor] = 0;
                for (T_size xf(0); xf < length; xf++)
                    out[x_anchor] += (R)(((S)in[x + xf]) * filter[xf]);   
            }
        }
        else
        {
            // input is row-major, output is row-major
            const T_size _x_min = y*width + x_min;
            const T_size _x_max = y*width + x_max;
            for (T_size x_anchor(_x_min), x(y*width); x_anchor < _x_max; x_anchor++, x++)
            {                
                out[x_anchor] = 0;
                for (T_size xf = 0; xf < length; xf++)
                    out[x_anchor] += (R)(((S)in[x + xf]) * filter[xf]);
            }
        }
    }
}
// instantiate for uint8_t images
template void RowFilter(const uint8_t*, int, int, const float*, int, float*, int, int, int, int, bool, bool);
template void RowFilter(const uint8_t*, int, int, const float*, int, double*, int, int, int, int, bool, bool);
template void RowFilter(const uint8_t*, int, int, const double*, int, double*, int, int, int, int, bool, bool);
template void RowFilter(const uint8_t*, int, int, const double*, int, float*, int, int, int, int, bool, bool);
// instantiate for float images
template void RowFilter(const float*, int, int, const float*, int, float*, int, int, int, int, bool, bool);
template void RowFilter(const float*, int, int, const float*, int, double*, int, int, int, int, bool, bool);
template void RowFilter(const float*, int, int, const double*, int, double*, int, int, int, int, bool, bool);
template void RowFilter(const float*, int, int, const double*, int, float*, int, int, int, int, bool, bool);
// instantiate for double images
template void RowFilter(const double*, int, int, const float*, int, float*, int, int, int, int, bool, bool);
template void RowFilter(const double*, int, int, const float*, int, double*, int, int, int, int, bool, bool);
template void RowFilter(const double*, int, int, const double*, int, double*, int, int, int, int, bool, bool);
template void RowFilter(const double*, int, int, const double*, int, float*, int, int, int, int, bool, bool);

#ifdef __MEX
/* MATLAB interface 
 * -> be aware that MATLAB uses column-major data storage
 */
void
mexFunction(int nlhs, mxArray* plhs[],
            int nrhs, const mxArray* prhs[])
{
    // out = filter(in) => just perform transpose
    // out = filter(in,row_filter) => row filter
    // out = filter(in,row_filter,col_filter) => row_filter and col_filter, i.e. linear separable filter
    
    /* Check number of input parameters. */
    if (nrhs < 1 || nrhs > 3)
        mexErrMsgTxt("Wrong number of input arguments. Input should be: image [row_filter] [col_filter].\n");
    
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
    //for (int i = 0; i < nrhs; i++)
    //    mexPrintf(" %d: %s (%d)", i, mxGetClassName(prhs[i]),mxGetClassID(prhs[i]));
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
        if (mxGetM(mxRowFilter) > 1 && mxGetN(mxRowFilter) > 1)
            mexPrintf("WARNING: only 1-D row/col filter are valid! Check your filters!\n");
    }
    if (nrhs >= 3)
    {
        mxColFilter = prhs[2];
        vColFilter = (void*)mxGetPr(mxColFilter);
        col_filter_length = mxGetM(mxColFilter) * mxGetN(mxColFilter);
        mxColFilterClass = mxGetClassID(mxColFilter);
        if (mxGetM(mxColFilter) > 1 && mxGetN(mxColFilter) > 1)
            mexPrintf("WARNING: only 1-D row/col filter are valid! Check your filters!\n");
        if (mxGetClassID(mxRowFilter) != mxGetClassID(mxColFilter))
        {
            char buf[256];
            sprintf(buf,"ERROR: Row and column filter have different classes, i.e. %s (%d) != %s (%d).\n", mxGetClassName(mxRowFilter), mxGetClassID(mxRowFilter), mxGetClassName(mxColFilter), mxGetClassID(mxColFilter));
            mexErrMsgTxt(buf);
        }
    }
    
    /* Output */
    mxArray* mxOut = NULL;
    if (nrhs == 1)
        mxOut = mxCreateNumericMatrix(width,height,mxImageClass,mxREAL); // transposed image simply has the same data type
    else
        mxOut = mxCreateNumericMatrix(height,width,mxRowFilterClass,mxREAL); // filtered image has the type of the filter (which is supposed to be double or single)
    plhs[0] = mxOut;
    void* vOut = (void*)mxGetPr(mxOut);
    
    // just one input => transpose the input matrix
    if (nrhs == 1)
    {
        switch (mxImageClass)
        {
            // transpose has to be called with switched width/height, because matlab uses column-major storage
            case mxUINT8_CLASS:
                Transpose((uint8_t*)vImage,height,width,(uint8_t*)vOut);
                break;
            case mxSINGLE_CLASS:
                Transpose((float*)vImage,height,width,(float*)vOut);
                break;
            case mxDOUBLE_CLASS:
                Transpose((double*)vImage,height,width,(double*)vOut);
                break;
            default:
                mexErrMsgTxt("Unsupported data type for transpose\n");
                break;
        }
    }

    // at least two inputs => filter the input matrix
    if (nrhs >= 2)
    {
        if (mxImageClass != mxUINT8_CLASS)
            mexErrMsgTxt("Currently only UINT8 images are supported\n");

        // create temporary buffer
        uint8_t* transposedImage = new uint8_t[width*height];

        // transpose the image to get a row-major image for filtering
        Transpose((uint8_t*)vImage,height,width,transposedImage);

        switch (mxRowFilterClass)
        {
            case mxSINGLE_CLASS:
                {
                    //mxAssert(CheckArrayValues((float*)vOut,width*height,(float)0,(float)0),"vOut is corrupt");
                    
                    if (nrhs == 2)
                    {
                        RowFilter(transposedImage,width,height,(float*)vRowFilter,row_filter_length,(float*)vOut,true);
                    }
                    else
                    {
                        float* tmp = new float[width*height];
                        for (int i = 0; i < width*height; i++)
                            tmp[i] = (float)0;
                        RowFilter(transposedImage,width,height,(float*)vRowFilter,row_filter_length,tmp,true);
                        //mxAssert(CheckArrayValues((float*)vOut,width*height,(float)0,(float)0),"vOut is corrupt");
                        //mxAssert(CheckArrayValues((float*)tmp,width*height,(float)0,(float)1000),"tmp is corrupt");
                        RowFilter(tmp,height,width,(float*)vColFilter,col_filter_length,(float*)vOut,false); // we don't need to transpose the output here, because matlab wants column-major
                        delete [] tmp;
                    }
                }
                break;
            case mxDOUBLE_CLASS:
                {
                    //mxAssert(CheckArrayValues((double*)vOut,width*height,(double)0,(double)0),"vOut is corrupt");
                    
                    if (nrhs == 2)
                    {
                         RowFilter(transposedImage,width,height,(double*)vRowFilter,row_filter_length,(double*)vOut,true);
                    }
                    else
                    {
                        double* tmp = new double[width*height];
                        for (int i = 0; i < width*height; i++)
                            tmp[i] = (double)0;
                        RowFilter(transposedImage,width,height,(double*)vRowFilter,row_filter_length,tmp,true);
                        //mxAssert(CheckArrayValues((double*)vOut,width*height,(double)0,(double)0),"vOut is corrupt");
                        //mxAssert(CheckArrayValues((double*)tmp,width*height,(double)0,(double)1000),"tmp is corrupt");
                        RowFilter(tmp,height,width,(double*)vColFilter,col_filter_length,(double*)vOut,false); // we don't need to transpose the output here, because matlab wants column-major
                        delete [] tmp;
                    }
                }
                break;
            default:
                mexErrMsgTxt("Currently only single and double are supported filter data types\n");
                break;
       }

       delete [] transposedImage;
    }
    
    return;
}
#endif

#ifdef __STANDALONE
/* Test some routines and test cases in a standalone executable. */
int
main(int argc, char* argv[])
{
    int width = 0, height = 0, row_length = 0, col_length;
    float *in = NULL;
    float *out = NULL;
    float *outt = NULL; // out transposed
    float *outtt = NULL; // out transposed and transposed
    float *row_filter = NULL;
    float *col_filter = NULL;

    width = 320;
    height = 240;
    row_length = 9;
    col_length = 9;

    in = new float[width*height];
    out = new float[width*height];
    outt = new float[width*height];
    outtt = new float[width*height];
    row_filter = new float[row_length];
    col_filter = new float[col_length];

    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            in[y*width + x] = rand() % 100;//1;
    for (int f = 0; f < row_length; f++)
        row_filter[f] = 1.0f / (float)row_length;
    for (int f = 0; f < col_length; f++)
        col_filter[f] = 1.0f / (float)col_length;

    RowFilter(in,width,height,row_filter,row_length,out,false);
    RowFilter(in,width,height,row_filter,row_length,outt,true);
    Transpose(outt,height,width,outtt);

    /* check that the content of out and outtt is equal */
    std::cout << "Checking RowFilter transpose behavior ..."  << std::endl;
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            if (out[y*width + x] != outtt[y*width + x])
                std::cout << "[" << x << "," << y << "]: " << out[y*width + x] << " != " << outtt[y*width + x] << std::endl;

//    /* Perform column filtering by row filtering the transposed output. By transposing again the result will be row-major again. */
//    RowFilter(outt,height,width,col_filter,col_length,outtt,true);

    /* check ROI code */
    int roi_x_min = 50;
    int roi_y_min = 50;
    int roi_width = 100;
    int roi_height = 100;
    float *roi_out = new float[width*height];
    std::cout << "Checking RowFilter ROI behavior ..." << std::endl;
    RowFilter(in,width,height,row_filter,row_length,roi_out,roi_x_min,roi_y_min,roi_width,roi_height,false,false);
    for (int y = roi_y_min; y < roi_y_min + roi_height; y++)
        for (int x = roi_x_min; x < roi_x_min + roi_width; x++)
            if (out[y*width + x] != roi_out[y*width + x])
                std::cout << "[" << x << "," << y << "]: " << out[y*width + x] << " != " << roi_out[y*width + x] << std::endl;
    delete [] roi_out;

    delete [] in;
    delete [] out;
    delete [] outt;
    delete [] outtt;
    delete [] row_filter;
    delete [] col_filter;

    return 0;
}
#endif

