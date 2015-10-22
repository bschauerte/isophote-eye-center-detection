/** Isophote-based eye center detection (encapsulated in a class).
 *
 * \author Boris Schauerte
 * \email  boris.schauerte@eyezag.com
 * \date   2011
 *
 * Copyright (C) 2011  Boris Schauerte
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "isophoteeyedetector.hpp"

// include necessary stuff for isophote calculation
#include "isophote.hpp"
#include "gauss_filter.hpp"
#include "separable_filter.hpp"
#include "typetostring.hpp"

#ifdef __STANDALONE
#include <okapi.hpp>
#include <okapi-gui.hpp>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) { if ((x) != NULL) delete [] x; x = NULL; }
#endif

#define BENCHMARK_ISOPHOTE_EYE_CENTER_DETECTOR
#ifdef BENCHMARK_ISOPHOTE_EYE_CENTER_DETECTOR
#define BENCHMARK_START(name) OKAPI_TIMER_START(name)
#define BENCHMARK_STOP(name) OKAPI_TIMER_STOP(name)
#else
#define BENCHMARK_START(name) 
#define BENCHMARK_STOP(name)
#endif

template <typename T>
IsophoteEyeCenterDetector<T>::IsophoteEyeCenterDetector(void)
: current_row_filter_length(0), current_col_filter_length(0), current_width(0), current_height(0), current_row_sigma(0), current_col_sigma(0),
  manual_eye_roi(-1,-1,-1,-1), manual_row_sigma(-1), manual_col_sigma(-1),
  buf_width(0), buf_height(0), Lx(NULL), Ly(NULL), Lxx(NULL), Lxy(NULL), Lyy(NULL), tmpColMajor(NULL), tmpT1(NULL), tmpLx2(NULL), tmpLy2(NULL), acc(NULL),
  buf_length(0), row_g(NULL), row_gp(NULL), row_gpp(NULL), col_g(NULL), col_gp(NULL), col_gpp(NULL), row_ax(NULL), col_ax(NULL)
{
}

template <typename T>
IsophoteEyeCenterDetector<T>::~IsophoteEyeCenterDetector(void)
{
    ReleaseImageMemory();
}

template <typename T>
void
IsophoteEyeCenterDetector<T>::ReallocateImageMemory(int new_width, int new_height, bool set_zero)
{
    int buf_size = buf_width*buf_height;
    int new_size = new_width*new_height;
    if (new_width != buf_width || new_height != buf_height)
    {
        if (buf_size < new_size)
        {
            // release old memory
            ReleaseImageMemory();
    
            // allocate new memory
            k           = new T[new_size];
            c           = new T[new_size];
            dx          = new T[new_size];
            dy          = new T[new_size];
            Lx          = new T[new_size];
            Ly          = new T[new_size];
            Lxx         = new T[new_size];
            Lxy         = new T[new_size];
            Lyy         = new T[new_size];
            tmpColMajor = new T[new_size];
            tmpT1       = new T[new_size];
            tmpLx2      = new T[new_size];
            tmpLy2      = new T[new_size];
            acc         = new T[new_size];
    
            // set new buffer width/height
            buf_width = new_width;
            buf_height = new_height;
        }
        if (set_zero)
        {
            for (int i = 0; i < new_width*new_height; i++)
            {
                k[i]           = T(0);
                c[i]           = T(0);
                dx[i]          = T(0);
                dy[i]          = T(0);
                Lx[i]          = T(0);
                Ly[i]          = T(0);
                Lxx[i]         = T(0);
                Lxy[i]         = T(0);
                Lyy[i]         = T(0);
                tmpColMajor[i] = T(0);
                tmpT1[i]       = T(0);
                tmpLx2[i]      = T(0);
                tmpLy2[i]      = T(0);
                acc[i]         = T(0);
            }
        }
    }
}

template <typename T>
void
IsophoteEyeCenterDetector<T>::ReleaseImageMemory(void)
{
    if (buf_width != 0 || buf_height != 0)
    {
        SAFE_DELETE_ARRAY(k);
        SAFE_DELETE_ARRAY(c);
        SAFE_DELETE_ARRAY(dx);
        SAFE_DELETE_ARRAY(dy);
        SAFE_DELETE_ARRAY(Lx);
        SAFE_DELETE_ARRAY(Ly);
        SAFE_DELETE_ARRAY(Lxx);
        SAFE_DELETE_ARRAY(Lxy);
        SAFE_DELETE_ARRAY(Lyy);
        SAFE_DELETE_ARRAY(tmpColMajor);
        SAFE_DELETE_ARRAY(tmpT1);
        SAFE_DELETE_ARRAY(tmpLx2);
        SAFE_DELETE_ARRAY(tmpLy2);
        SAFE_DELETE_ARRAY(acc);
    }
}

template <typename T>
void
IsophoteEyeCenterDetector<T>::ReallocateFilterMemory(int new_row_length, int new_col_length)
{
    // we are lazy and don't really distinguish between the lengths
    int new_length = (new_row_length > new_col_length ? new_row_length : new_col_length);   
    
    if (new_row_length > buf_length || new_col_length > buf_length)
    {
        // release old memory
        ReleaseFilterMemory();

        // allocate new memory
        row_g   = new T[new_length];
        row_gp  = new T[new_length];
        row_gpp = new T[new_length];
        col_g   = new T[new_length];
        col_gp  = new T[new_length];
        col_gpp = new T[new_length];
        row_ax  = new T[new_length];
        col_ax  = new T[new_length];

        buf_length = new_length;
    }
}

template <typename T>
void
IsophoteEyeCenterDetector<T>::ReleaseFilterMemory(void)
{
    if (buf_length != 0)
    {
        SAFE_DELETE_ARRAY(row_g);
        SAFE_DELETE_ARRAY(row_gp);
        SAFE_DELETE_ARRAY(row_gpp);
        SAFE_DELETE_ARRAY(col_g);
        SAFE_DELETE_ARRAY(col_gp);
        SAFE_DELETE_ARRAY(col_gpp);
        SAFE_DELETE_ARRAY(row_ax);
        SAFE_DELETE_ARRAY(col_ax);
    }
}

template <typename T> // for the class
template <typename S> // for the method
void
IsophoteEyeCenterDetector<T>::process(const S* img, int width, int height, T row_sigma, T col_sigma, const cv::Rect_<coord_t>& left_roi, const cv::Rect_<coord_t>& right_roi)
{
    static bool displayed_warning = false;
    if (!displayed_warning)
        std::cout << "IsophoteEyeCenterDetector<" << TypeToString<T>() << ">.process: Warning - use of ROI's in processing not implemented yet!" << std::endl;
    displayed_warning = true;
    // @TODO: check whether one ROI contains the other ROI and then just process the bigger ROI!

    // (Re-)Allocate memory if necessary
    ReallocateImageMemory(width,height);

    // Calculate the filters
    bool normalize_filter = false; // @TODO: does setting this to true really disturb the results? Currently I have the -subjective- feeling that it could be a problem!
    bool flip_filter = true;
    int row_filter_length = GetGaussLength<T,int>(row_sigma); // calculate the necessary filter length, i.e. as defined by the support
    int col_filter_length = GetGaussLength<T,int>(col_sigma); // calculate the necessary filter length, i.e. as defined by the support
    ReallocateFilterMemory(row_filter_length,col_filter_length);
    // recalculate row filters if necessary
    if (row_sigma != current_row_sigma)
    {
        GetGaussSupport<T,int>(row_sigma,row_ax); // calculate the support
        CreateGauss(row_sigma,normalize_filter,row_ax,row_g);
        CreateGaussFirstDeriv(row_sigma,normalize_filter,row_ax,row_gp);
        CreateGaussSecondDeriv(row_sigma,normalize_filter,row_ax,row_gpp);
        // we have to flip the arrays in order to calculate the "real" convolution with the RowFilter procedure
        if (flip_filter)
        {
            // @todo: add another parameter to CreateGauss* to directly get flipped filters as output
            FlipArray(row_g,row_filter_length);
            FlipArray(row_gp,row_filter_length);
            FlipArray(row_gpp,row_filter_length);
        }
    }
    // recalculate col filters if necessary 
    if (col_sigma != current_col_sigma)
    {
        GetGaussSupport<T,int>(col_sigma,col_ax); // calculate the support
        CreateGauss(col_sigma,normalize_filter,col_ax,col_g);
        CreateGaussFirstDeriv(col_sigma,normalize_filter,col_ax,col_gp);
        CreateGaussSecondDeriv(col_sigma,normalize_filter,col_ax,col_gpp);
        // we have to flip the arrays in order to calculate the "real" convolution with the RowFilter procedure
        if (flip_filter)
        {
            // @todo: add another parameter to CreateGauss* to directly get flipped filters as output
            FlipArray(col_g,col_filter_length);
            FlipArray(col_gp,col_filter_length);
            FlipArray(col_gpp,col_filter_length);
        }
    }

    // Let's calculate the Gaussian and its derivatives
    BENCHMARK_START("RowFilter");
#define _ROI_ROW_FILTER
#ifdef _ROI_ROW_FILTER
    // 1. Left eye
    // (a) Calculate Ly and Lyy
    RowFilter(img,width,height,row_g,row_filter_length,tmpColMajor,left_roi.x,left_roi.y,left_roi.width,left_roi.height,false,true);
    RowFilter(tmpColMajor,height,width,col_gp,col_filter_length,Ly,left_roi.y,left_roi.x,left_roi.height,left_roi.width,false,true);
    RowFilter(tmpColMajor,height,width,col_gpp,col_filter_length,Lyy,left_roi.y,left_roi.x,left_roi.height,left_roi.width,false,true);
    // (b) Calculate Lx and Lxy
    RowFilter(img,width,height,row_gp,row_filter_length,tmpColMajor,left_roi.x,left_roi.y,left_roi.width,left_roi.height,false,true);
    RowFilter(tmpColMajor,height,width,col_g,col_filter_length,Lx,left_roi.y,left_roi.x,left_roi.height,left_roi.width,false,true); 
    RowFilter(tmpColMajor,height,width,col_gp,col_filter_length,Lxy,left_roi.y,left_roi.x,left_roi.height,left_roi.width,false,true); 
    // (c) Calculate Lxx
    RowFilter(img,width,height,row_gpp,row_filter_length,tmpColMajor,left_roi.x,left_roi.y,left_roi.width,left_roi.height,false,true);
    RowFilter(tmpColMajor,height,width,col_g,col_filter_length,Lxx,left_roi.y,left_roi.x,left_roi.height,left_roi.width,false,true);
    // 2. Right eye
    // (a) Calculate Ly and Lyy
    RowFilter(img,width,height,row_g,row_filter_length,tmpColMajor,right_roi.x,right_roi.y,right_roi.width,right_roi.height,false,true);
    RowFilter(tmpColMajor,height,width,col_gp,col_filter_length,Ly,right_roi.y,right_roi.x,right_roi.height,right_roi.width,false,true);
    RowFilter(tmpColMajor,height,width,col_gpp,col_filter_length,Lyy,right_roi.y,right_roi.x,right_roi.height,right_roi.width,false,true);
    // (b) Calculate Lx and Lxy
    RowFilter(img,width,height,row_gp,row_filter_length,tmpColMajor,right_roi.x,right_roi.y,right_roi.width,right_roi.height,false,true);
    RowFilter(tmpColMajor,height,width,col_g,col_filter_length,Lx,right_roi.y,right_roi.x,right_roi.height,right_roi.width,false,true); 
    RowFilter(tmpColMajor,height,width,col_gp,col_filter_length,Lxy,right_roi.y,right_roi.x,right_roi.height,right_roi.width,false,true); 
    // (c) Calculate Lxx
    RowFilter(img,width,height,row_gpp,row_filter_length,tmpColMajor,right_roi.x,right_roi.y,right_roi.width,right_roi.height,false,true);
    RowFilter(tmpColMajor,height,width,col_g,col_filter_length,Lxx,right_roi.y,right_roi.x,right_roi.height,right_roi.width,false,true);
#else
    // (a) Calculate Ly and Lyy
    RowFilter(img,width,height,row_g,row_filter_length,tmpColMajor,true);
    RowFilter(tmpColMajor,height,width,col_gp,col_filter_length,Ly,true);
    RowFilter(tmpColMajor,height,width,col_gpp,col_filter_length,Lyy,true);
    // (b) Calculate Lx and Lxy
    RowFilter(img,width,height,row_gp,row_filter_length,tmpColMajor,true);
    RowFilter(tmpColMajor,height,width,col_g,col_filter_length,Lx,true); 
    RowFilter(tmpColMajor,height,width,col_gp,col_filter_length,Lxy,true); 
    // (c) Calculate Lxx
    RowFilter(img,width,height,row_gpp,row_filter_length,tmpColMajor,true);
    RowFilter(tmpColMajor,height,width,col_g,col_filter_length,Lxx,true);
#endif
    BENCHMARK_STOP("RowFilter");
                                                                                                    
    // Calculate the isophote information, i.e. curvature, curvedness, and displacement vectors
    BENCHMARK_START("CalculateIsophoteInformation");
//    CalculateIsophoteInformation(Lx,Ly,Lxx,Lxy,Lyy,width,height,k,c,dx,dy,tmpT1,tmpLx2,tmpLy2);
    // set k to zero => elements with k=0 are not processed in CalculateAccumulator
    for (int i = 0; i < width*height; i++)
        k[i] = T(0);
    CalculateIsophoteInformation(Lx,Ly,Lxx,Lxy,Lyy,(int)width,(int)height,k,c,dx,dy,(int)left_roi.x,(int)left_roi.y,(int)left_roi.width,(int)left_roi.height,tmpT1,tmpLx2,tmpLy2);
    CalculateIsophoteInformation(Lx,Ly,Lxx,Lxy,Lyy,(int)width,(int)height,k,c,dx,dy,(int)right_roi.x,(int)right_roi.y,(int)right_roi.width,(int)right_roi.height,tmpT1,tmpLx2,tmpLy2);
    BENCHMARK_STOP("CalculateIsophoteInformation");
    BENCHMARK_START("CalculateAccumulator");
    CalculateAccumulator(k,c,dx,dy,width,height,acc,false,true);
    BENCHMARK_STOP("CalculateAccumulator");

    // save the most relevant information about the image processing
    current_row_sigma = row_sigma;
    current_col_sigma = col_sigma;
    current_width = width;
    current_height = height;
    current_row_filter_length = row_filter_length;
    current_col_filter_length = col_filter_length;
    current_left_roi = left_roi;
    current_right_roi = right_roi;
}

template <typename T> // for the class
template <typename S> // for the method
EyeCenterLocations<typename IsophoteEyeCenterDetector<T>::coord_t> // yay, nasty template shit
IsophoteEyeCenterDetector<T>::detectEyeCenters(const S* img, int width, int height, const cv::Rect_<coord_t>& face_box, const cv::Point_<coord_t>& left_eye, const cv::Point_<coord_t>& right_eye)
{
    EyeCenterLocations<coord_t> result(cv::Point_<coord_t>(-1,-1),cv::Point_<coord_t>(-1,-1));

    // calculate ROI's
    bool has_face_box      = isValidCoord(face_box);  // do we have a valid face box?
    bool has_left_eye_loc  = isValidCoord(left_eye);  // do we have a pre-estimated left eye location?
    bool has_right_eye_loc = isValidCoord(right_eye); // do we have a pre-estimated right eye location?
    cv::Rect left_roi(0,0,width,height);  // default ROI: the complete image
    cv::Rect right_roi(0,0,width,height); // default ROI: the complete image
    cv::Rect eye_roi;
    if (isValidCoord(manual_eye_roi))
        eye_roi = manual_eye_roi; // did someone set an eye ROI?
    else
        eye_roi = cv::Rect(24,24,48,36); // general size of the region of interest around pre-detected eye locations; @TODO: automatical calculation/estimation of best eye ROI
    // simple version => use the face box and ROI around detected eyes
    if (has_face_box)
    {
        left_roi = face_box;
        right_roi = face_box;

        // additional validity check, but only display a warning
        if (has_left_eye_loc && !isValidCoord(left_eye,face_box))
            std::cerr << "IsophoteEyeCenterDetector<" << TypeToString<T>() << ">.detectEyeCenters: Warning - left eye location is not inside the face box!" << std::endl;
        if (has_right_eye_loc && !isValidCoord(right_eye,face_box))
            std::cerr << "IsophoteEyeCenterDetector<" << TypeToString<T>() << ">.detectEyeCenters: Warning - right eye location is not inside the face box!" << std::endl;

        eye_roi.width = std::min(eye_roi.width,(int)face_box.width/2);
        eye_roi.height = std::min(eye_roi.height,(int)face_box.height/3);
        eye_roi.x = eye_roi.width / 2;
        eye_roi.y = eye_roi.height / 2;
    }
    if (has_left_eye_loc)
    {
        left_roi.x = left_eye.x - eye_roi.x;
        left_roi.y = left_eye.y - eye_roi.y;
        left_roi.width = eye_roi.width;
        left_roi.height = eye_roi.height;
    }
    if (has_right_eye_loc)
    {
        right_roi.x = right_eye.x - eye_roi.x;
        right_roi.y = right_eye.y - eye_roi.y;
        right_roi.width = eye_roi.width;
        right_roi.height = eye_roi.height;
    }

    // process the image
    T row_sigma = 1;
    T col_sigma = 1;
    if (manual_row_sigma > 0)
        row_sigma = manual_row_sigma;
    else
        row_sigma = 1; // @TODO: automatically calculate the "best" row sigma
    if (manual_col_sigma > 0)
        col_sigma = manual_col_sigma;
    else
        col_sigma = 1; // @TODO: automatically calculate the "best" col sigma
    process(img,width,height,row_sigma,col_sigma,left_roi,right_roi);
    
    // Process accumulator in order to detect eye center hypotheses
    BENCHMARK_START("AccumulatorProcessing");
    cv::Mat macc = getMatAcc();
    cv::Mat smacc; // smoothed accumulator
    double accumulator_row_sigma=0;
    double accumulator_col_sigma=0;
    cv::GaussianBlur(macc,smacc,cv::Size(9,9),accumulator_row_sigma,accumulator_col_sigma);
    cv::Point left_max_loc;
    cv::Point right_max_loc;
    cv::Mat lrsmacc = smacc(left_roi); // ROI of the smoothed accumulator for the left eye
    cv::minMaxLoc(lrsmacc,NULL,NULL,NULL,&left_max_loc);
    cv::Mat rrsmacc = smacc(right_roi); // ROI of the smoothed accumulator for the right eye
    cv::minMaxLoc(rrsmacc,NULL,NULL,NULL,&right_max_loc);
    left_max_loc.x += left_roi.x;
    left_max_loc.y += left_roi.y;
    right_max_loc.x += right_roi.x;
    right_max_loc.y += right_roi.y;
    result.left = left_max_loc;
    result.right = right_max_loc;
    BENCHMARK_STOP("AccumulatorProcessing");
    
    return result;
}

template <typename T>
EyeCenterLocations<typename IsophoteEyeCenterDetector<T>::coord_t> // yay, nasty template shit
IsophoteEyeCenterDetector<T>::detectEyeCenters(const cv::Mat& img, const cv::Rect_<coord_t>& face_box, const cv::Point_<coord_t>& left_eye, const cv::Point_<coord_t>& right_eye)
{
    // we need compact memory layout, i.e. rowstep = ?, so we can use the data from img directly
    if (img.step1() != (size_t)img.cols)     
    {
        std::cerr << "IsophoteEyeCenterDetector<" << TypeToString<T>() << ">.detectEyeCenters: img.step1 != img.cols! Skipping image!" << std::endl;
        return EyeCenterLocations<coord_t>();
    }

    // img has to be uint8, single channel
    if (img.type() != CV_8UC1)
    {
        std::cerr << "IsophoteEyeCenterDetector<" << TypeToString<T>() << ">.detectEyeCenters: unsupported image type! Skipping image!" << std::endl;
        return EyeCenterLocations<coord_t>();
    }
    else
    {
        return detectEyeCenters((uint8_t*)img.data,img.cols,img.rows,face_box,left_eye,right_eye);
    }
}

template <typename T>
void
IsophoteEyeCenterDetector<T>::getSearchRegions(const cv::Mat& img, const cv::Rect_<coord_t>& face_box, const cv::Point_<coord_t>& left_eye, const cv::Point_<coord_t>& right_eye,cv::Rect_<coord_t>& left_eye_roi, cv::Rect_<coord_t>& right_eye_roi)
{
    std::cerr << "IsophoteEyeCenterDetector<" << TypeToString<T>() << ">.getSearchRegions(...): not implemented yet!" << std::endl << std::flush;
    assert(false);
}

/* Template instantiation for non-standalone compile */
template class IsophoteEyeCenterDetector<float>;
template class IsophoteEyeCenterDetector<double>;


#ifdef __STANDALONE
template <typename T, typename S>
void
printArray(const std::string& prefix, const T* arr, const S& length)
{
    if (prefix != "")
        std::cout << prefix;
    for (S i(0); i < length-1; i++)
        std::cout << arr[i] << " ";
    std::cout << arr[length-1] << std::endl;
}

int
main(int argc, char* argv[])
{
    // Create IsophoteEyeCenterDetector
    typedef IsophoteEyeCenterDetector<float> ed_t;
    ed_t ed;

    // load a test image 
    //IplImage* imgIpl = cvLoadImage("/home/bschauer/work/matlab/eye_detection/images/test_pattern.png", 1);
    IplImage* imgIpl = cvLoadImage("/home/bschauer/data/face/BioID-X/images/BioID_1014.pgm",1);
    cv::Mat imgbgr(imgIpl);
    cv::Mat imgrgb(imgbgr);
    cvtColor(imgbgr, imgrgb, CV_BGR2RGB);
    // convert to gray scale
    cv::Mat imggray(imgrgb);
    cvtColor(imgrgb,imggray,CV_RGB2GRAY);

    // calculate
    cv::Rect_<ed_t::coord_t> face_box(-1,-1,-1,-1); // initialize as invalid
    cv::Point_<ed_t::coord_t> left_eye(-1,-1);      // initialize as invalid
    cv::Point_<ed_t::coord_t> right_eye(-1,-1);     // initialize as invalid
    EyeCenterLocations<ed_t::coord_t> eye_centers;
    for (int i = 0; i < 100; i++) // let's calculate it several times in order to get reasonable run-times
    {
        OKAPI_TIMER_START("ed.detectEyeCenters");
        eye_centers = ed.detectEyeCenters(imggray,face_box,left_eye,right_eye);
        OKAPI_TIMER_STOP("ed.detectEyeCenters");
    }
    std::cout << "Detected Eye Centers:" << std::endl << "\tleft:  (" << eye_centers.left.x << "," << eye_centers.left.y << ")" << std::endl << "\tright: (" << eye_centers.right.x << "," << eye_centers.right.y << ")" << std::endl;

    // drawings
    okapi::ImageDeco deco(imgrgb);
    deco.setAntiAlias(true);
    deco.setThickness(1);
    deco.setColor(255, 0, 255);
    deco.drawCircle(cvRound(eye_centers.left.x), cvRound(eye_centers.left.y), 3);
    deco.drawCircle(cvRound(eye_centers.right.x), cvRound(eye_centers.right.y), 3);

    // print some information
    std::cout << "ed.getHeight(): " << ed.getHeight() << std::endl;
    std::cout << "ed.getWidth(): " << ed.getWidth() << std::endl;
    std::cout << "ed.getRowSigma(): " << ed.getRowSigma() << std::endl;
    std::cout << "ed.getColSigma(): " << ed.getColSigma() << std::endl;
    // print the filters
    int row_filter_length = ed.getRowFilterLength();
    int col_filter_length = ed.getColFilterLength();
    printArray("ed.getRowG(): ",ed.getRowG(),row_filter_length);
    printArray("ed.getRowGP(): ",ed.getRowGP(),row_filter_length);
    printArray("ed.getRowGPP(): ",ed.getRowGPP(),row_filter_length);
    printArray("ed.getColG(): ",ed.getColG(),col_filter_length);
    printArray("ed.getColGP(): ",ed.getColGP(),col_filter_length);
    printArray("ed.getColGPP(): ",ed.getColGPP(),col_filter_length);

    // show the variables/output, e.g. we want to see the accumulator!
    // Set up GUI
    okapi::GuiThread thread;
    okapi::ImageWindow* imgwin = new okapi::ImageWindow("Isophote Eye Center Detector Demo (still image)");
    thread.addClient(imgwin);
    thread.start();

#define _DISPLAY_METHOD_RESULT(call,scale) imgwin->setImage(#call,okapi::Mat2Gray(call),scale)
    imgwin->setImage("imgrgb",imgrgb,1);
    _DISPLAY_METHOD_RESULT(ed.getMatAcc(),1);
    _DISPLAY_METHOD_RESULT(ed.getMatK(),1);
    _DISPLAY_METHOD_RESULT(ed.getMatC(),1);
    _DISPLAY_METHOD_RESULT(ed.getMatLx(),1);
    _DISPLAY_METHOD_RESULT(ed.getMatLy(),1);
    _DISPLAY_METHOD_RESULT(ed.getMatLxx(),1);
    _DISPLAY_METHOD_RESULT(ed.getMatLxy(),1);
    _DISPLAY_METHOD_RESULT(ed.getMatLyy(),1);

    while (imgwin->getWindowState())
        okapi::sleep_ms(20);

    return 0;
}
#endif
