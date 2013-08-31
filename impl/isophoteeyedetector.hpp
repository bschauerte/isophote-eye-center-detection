/** Isophote-based eye center detection (encapsulated in a class).
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

#include <okapi.hpp>

/** NOTES:
 *  - Interface is supposed to be similar to the BinaryPatternEyeDetector
 */

template <typename T>
struct EyeCenterLocations
{
    cv::Point_<T> left;  // left eye center coordinate
    cv::Point_<T> right; // right eye center coordinate

    /** Default constructur. Default is an invalid point. */
    EyeCenterLocations(void)
    : left(-1,-1), right(-1,-1)
    {
    }

    EyeCenterLocations(const cv::Point_<T>& _left, const cv::Point_<T>& _right)
    : left(_left), right(_right)
    {
    }

    bool isValid(void) const 
    {
        return (left.x >= 0) && (right.x >= 0) && (left.y >= 0) && (right.y >= 0);
    }
};

template <typename T>
class IsophoteEyeCenterDetector
{
        public:
            typedef int coord_t;

            /** Default constructor. */
            IsophoteEyeCenterDetector(void);
            /** Destructor. */
            ~IsophoteEyeCenterDetector(void);

            ///
            // Main calculation methods
            ///
            /** Detect the eye center locations in the image. */
            EyeCenterLocations<coord_t> detectEyeCenters(const cv::Mat& img,
                    const cv::Rect_<coord_t>& face_box,
                    const cv::Point_<coord_t>& left_eye, const cv::Point_<coord_t>& right_eye);

            /** Main method to calculate the eye center locations. */ 
            template <typename S> EyeCenterLocations<coord_t> detectEyeCenters(const S* img, int width, int height, 
                                                                               const cv::Rect_<coord_t>& face_box,
                                                                               const cv::Point_<coord_t>& left_eye, const cv::Point_<coord_t>& right_eye);

            /** Run the image processing, filtering, etc. and finally calculate the raw accumulator that is further processed by detectEyeCenters to locate the eye centers.
             *  If you know what you are doing, you can just call process and get the raw accumulator (and the other image and filter informations) and write custom eye
             *  center detection routines based on this data/processing.
             */
            template <typename S> void process(const S* img, int width, int height,
                                               T row_sigma, T col_sigma,
                                               const cv::Rect_<coord_t>& left_roi,
                                               const cv::Rect_<coord_t>& right_roi);

            /** Return the area in which the eyes are searched, i.e. the regions of interest */
            void getSearchRegions(const cv::Mat& img,
                    const cv::Rect_<coord_t>& face_box,
                    const cv::Point_<coord_t>& left_eye, const cv::Point_<coord_t>& right_eye,
                    cv::Rect_<coord_t>& left_eye_roi, cv::Rect_<coord_t>& right_eye_roi);

            ///
            // Informations
            ///
            /** Get height of the (previously) processed image (defines the height of the output images). */
            inline const int getHeight(void) const { return current_height; }
            /** Get width of the (previously) processed image (defines the width of the output images). */
            inline const int getWidth(void) const { return current_width; }
            /** Get sigma that was used to process the image rows. */
            inline const T getRowSigma(void) const { return current_row_sigma; }
            /** Get sigma that was used to process the image cols. */
            inline const T getColSigma(void) const { return current_col_sigma; }
            /** Get the row filter length that was used to process the image. */
            inline const int getRowFilterLength(void) const { return current_row_filter_length; }
            /** Get the col filter length that was used to process the image. */
            inline const int getColFilterLength(void) const { return current_col_filter_length; }
            /** Get the left and right ROI that were used to process the image. */
            inline void getCurrentSearchRegions(cv::Rect_<coord_t>& left_roi, cv::Rect_<coord_t>& right_roi) const { left_roi = current_left_roi; right_roi = current_right_roi; }

            ///
            // Image getter
            ///
            /** Get the curvature. */
            inline const T* getK(void) const { return k; }
            /** Get the curvature as cv::Mat. */
            inline const cv::Mat getMatK(void) const { return cv::Mat(getHeight(),getWidth(),cv::DataType<T>::type,(void*)k); }
            /** Get the curvedness. */
            inline const T* getC(void) const { return c; }
            /** Get the curvedness as cv::Mat. */
            inline const cv::Mat getMatC(void) const { return cv::Mat(getHeight(),getWidth(),cv::DataType<T>::type,(void*)c); }
            /** Get the displacement in x-direction. */
            inline const T* getDx(void) const { return dx; }
            /** Get the displacement in x-direction as cv::Mat. */
            inline const cv::Mat getMatDx(void) const { return cv::Mat(getHeight(),getWidth(),cv::DataType<T>::type,(void*)dx); }
            /** Get the displacement in y-direction. */
            inline const T* getDy(void) const { return dy; }
            /** Get the displacement in y-direction as cv::Mat. */
            inline const cv::Mat getMatDy(void) const { return cv::Mat(getHeight(),getWidth(),cv::DataType<T>::type,(void*)dy); }
            /** Get the 1st partial derivative in x-direction. */
            inline const T* getLx(void) const { return Lx; }
            /** Get the 1st partial derivative in x-direction as cv::Mat. */
            inline const cv::Mat getMatLx(void) const { return cv::Mat(getHeight(),getWidth(),cv::DataType<T>::type,(void*)Lx); }
            /** Get the 1st partial derivative in y-direction. */
            inline const T* getLy(void) const { return Ly; }
            /** Get the 1st partial derivative in y-direction as cv::Mat. */
            inline const cv::Mat getMatLy(void) const { return cv::Mat(getHeight(),getWidth(),cv::DataType<T>::type,(void*)Ly); }
            /** Get the 2nd partial derivative in x-direction. */
            inline const T* getLxx(void) const { return Lxx; }
            /** Get the 2nd partial derivative in x-direction as cv::Mat. */
            inline const cv::Mat getMatLxx(void) const { return cv::Mat(getHeight(),getWidth(),cv::DataType<T>::type,(void*)Lxx); }
            /** Get the 1st partial derivative in x- and y- direction.*/
            inline const T* getLxy(void) const { return Lxy; }
            /** Get the 1st partial derivative in x- and y- direction as cv::Mat. */
            inline const cv::Mat getMatLxy(void) const { return cv::Mat(getHeight(),getWidth(),cv::DataType<T>::type,(void*)Lxy); }
            /** Get the 2nd partial derivative in y-direction. */
            inline const T* getLyy(void) const { return Lyy; }
            /** Get the 2nd partial derivative in y-direction as cv::Mat*/
            inline const cv::Mat getMatLyy(void) const { return cv::Mat(getHeight(),getWidth(),cv::DataType<T>::type,(void*)Lyy); }
            /** Get the accumulator. */
            inline const T* getAcc(void) const { return acc; }
            /** Get the accumulator as cv::Mat. */
            inline const cv::Mat getMatAcc(void) const { return cv::Mat(getHeight(),getWidth(),cv::DataType<T>::type,(void*)acc); }

            ///
            // Filter getter
            ///
            /** Get the row Gauss filter. */
            inline const T* getRowG(void) const { return row_g; }
            /** Get the row Gauss filter as cv::Mat. */
            inline const cv::Mat getMatRowG(void) const { return cv::Mat(getRowFilterLength(),1,cv::DataType<T>::type,(void*)row_g); }
            /** Get the 1st derivative of the row Gauss filter. */
            inline const T* getRowGP(void) const { return row_gp; }
            /** Get the 1st derivative of the row Gauss filter as cv::Mat. */
            inline const cv::Mat getMatRowGP(void) const { return cv::Mat(getRowFilterLength(),1,cv::DataType<T>::type,(void*)row_gp); }
            /** Get the 2nd derivative of the row Gauss filter. */
            inline const T* getRowGPP(void) const { return row_gpp; }
            /** Get the 2nd derivative of the row Gauss filter as cv::Mat. */
            inline const cv::Mat getMatRowGPP(void) const { return cv::Mat(getRowFilterLength(),1,cv::DataType<T>::type,(void*)row_gpp); }
            /** Get the col Gauss filter. */
            inline const T* getColG(void) const { return col_g; }
            /** Get the col Gauss filter as cv::Mat. */
            inline const cv::Mat getMatColG(void) const { return cv::Mat(1,getColFilterLength(),cv::DataType<T>::type,(void*)col_g); }
            /** Get the 1st derivative of the col Gauss filter. */
            inline const T* getColGP(void) const { return col_gp; }
            /** Get the 1st derivative of the col Gauss filter as cv::Mat. */
            inline const cv::Mat getMatColGP(void) const { return cv::Mat(1,getColFilterLength(),cv::DataType<T>::type,(void*)col_gp); }
            /** Get the 2nd derivative of the col Gauss filter. */
            inline const T* getColGPP(void) const { return col_gpp; }
            /** Get the 2nd derivative of the col Gauss filter as cv::Mat. */
            inline const cv::Mat getMatColGPP(void) const { return cv::Mat(1,getColFilterLength(),cv::DataType<T>::type,(void*)col_gpp); }

            ///
            // 2-D coordinate helper interface (e.g., allow to set/check for invalid/valid coordinates)
            ///
            /** Get default invalid coordinate. Useful to indicate that information about the point - e.g. a rough estimation of the eye location - is not available (i.e. when calling a method). */
            inline static cv::Rect_<coord_t> getInvalidCoordRect(void) { return cv::Rect_<coord_t>(-1,-1,-1,-1); }
            /** Get default invalid rectangle. Useful to indicate that information about the rectangle - e.g. region of interest - is not available (i.e. when calling a method). */
            inline static cv::Point_<coord_t> getInvalidCoordPoint(void) { return cv::Point_<coord_t>(-1,-1); }
            /** Is it a valid coordinate point? */
            inline static bool isValidCoord(const cv::Point_<coord_t>& p) { return (p.x >= 0 && p.y >= 0); }
            /** Is it a valid coordinate rectangle? */
            inline static bool isValidCoord(const cv::Rect_<coord_t>& r) { return (r.x >= 0 && r.y >= 0 && r.height >= 0 && r.width >= 0); }
            /** Is p a valid coordinate point given valid rectangle r, i.e. is the point inside/contained-in the rectangle? */
            inline static bool isValidCoord(const cv::Point_<coord_t>& p, const cv::Rect_<coord_t>& r) { return (p.x >= r.x && p.y >= r.y && p.x < (r.x + r.width) && p.y < (r.y + r.height)); };
            /** Is r1 a valid rectangle given rectangle r2, i.e. is r1 inside/contained-in r2? */
            inline static bool isValidCoord(const cv::Rect_<coord_t>& r1, const cv::Rect_<coord_t>& r2) { return (r1.x >= r2.x && r1.y >= r2.y && (r1.x + r1.width) <= (r2.x + r2.width) && (r1.x + r1.height) <= (r2.x + r2.height)); }

            /** Set the eye ROI, i.e. the search rectangle (ROI) around an eye detection. Disables automatical calculation (except if the eye ROI is invalid, then it enables the automatical calculation). */
            inline void setEyeROI(const cv::Rect_<coord_t> eye_roi) { manual_eye_roi = eye_roi; }
            /** Enable automatical calculation of eye ROI (default). Necessary to enable the automatical calculation has been disabled by calling setEyeROI previously. 
             *  Note that setEyeROI can be used for this purpose too, if the eye_roi is chosen accordingly.
             */
            inline void setAutoEyeROI(void) { manual_eye_roi = getInvalidCoordRect(); }
            /** Set sigma. Disables automatical sigma calculation (except if the row and col sigma are invalid, i.e. <0, then the automatical calculation is enabled). 
             *  If just the row sigma is valid or given, then the col sigma will be set to the value of the given row sigma;
             */
            inline void setSigma(const T& row_sigma, const T& col_sigma = -1) { manual_row_sigma = row_sigma; manual_col_sigma = (col_sigma < 0 ? row_sigma : col_sigma); }
            /** Enable automatical calculation of the row and col sigma (default). Necessary to enable the automatical calculation after setSigma has been called previously.
             *  Note that setSigma can be used for this purpose too, if row_sigma and col_sigma are chosen accordingly.
             */
            inline void setAutoSigma(void) { manual_row_sigma = -1; manual_col_sigma = -1; }

        protected:
            /** (Re-)Allocate image memory/buffers if necessary.
             *  -> it is recommended to set set_zero=true in order to set the allocated memory to 0 at least ones (this avoids problems with undefined values in unprocessed image border areas!)
             */
            void ReallocateImageMemory(int new_width, int new_height, bool set_zero = true);
            /** Release/Free the image memory/buffers. */
            void ReleaseImageMemory(void);
            /** (Re-)Allocate filter memory/buffers if necessary */
            void ReallocateFilterMemory(int new_row_length, int new_col_length);
            /** Release/Free the filter memory/buffers. */
            void ReleaseFilterMemory(void);

        private:
            // information about the image and applied filters
            int current_row_filter_length,          // length of the row filter that was used for calculation
                current_col_filter_length;          // length of the col filter that was used for calculation
            int current_width, current_height;      // width/height used for calculation
            T current_row_sigma, current_col_sigma; // sigma used for calculation (separate sigma for row and column filter); use GetGaussLength to calculate the length of the filters
            cv::Rect_<coord_t> current_left_roi,    // ROI in which the left eye center is expected and searched
                               current_right_roi;   // ROI in which the right eye center is expected and searched

            // manually set parameters
            cv::Rect_<coord_t> manual_eye_roi;      // manually set width/height and anchor of ROI around eye detections
            T manual_row_sigma, manual_col_sigma;   // manually set row/col sigma

            // image buffers/memory
            int buf_width, buf_height;              // width/height of currently allocated image buffers
            T *k, *c, *dx, *dy;                     // curvedness, curvature, x- and y-displacement
            T *Lx, *Ly, *Lxx, *Lxy, *Lyy;           // 1st and 2nd order derivatives
            T *tmpColMajor;                         // col-major image as temporary storage for efficient filtering
            T *tmpT1, *tmpLx2, *tmpLy2;             // temporary variables for efficient isophote calculation
            T *acc;                                 // the accumulator
            // filter buffers/memory
            int buf_length;                         // length of currently allocated filter buffers
            T *row_g, *row_gp, *row_gpp;            // row filters (Gaussian, 1st derivative, 2nd derivative)
            T *col_g, *col_gp, *col_gpp;            // col filters (Gaussian, 1st derivative, 2nd derivative)
            T *row_ax, *col_ax;                     // support for filter calculation
};
