#include <okapi.hpp>
#include <okapi-gui.hpp>
#include <okapi-videoio.hpp>

#include "isophoteeyedetector.hpp"

using namespace okapi;
using namespace std;

static const char help_str[] =
    "\nOkapi eye center detector demonstration\n"
    "\nOptions:\n%s\n";

int 
main(int argc, char* argv[])
{
    LibraryInitializer li;

    // parse arguments

    CommandLineOptions opt;
    opt.addOption('v', "videosource",            true,  "Use video source");
    opt.addOption('i', "images",                 false, "Use images");
    opt.addOption('d', "face-detector",          true,  "face detector file");
    opt.addOption('e', "eye-detector",           true,  "eye detector file");
    opt.addOption('m', "mouth-detector",         true,  "mouth detector file");
    opt.addOption('l', "low-resolution",         false, "Use resolution of 320x240 pixels");
    opt.addOption('s', "search-regions",         false, "Show eye and mouth search regions");
    opt.addOption(     "fps",                    false, "Display frames per second");
    opt.addOption('r', "draw-search-rectangles", false, "Draw the search rectangles (i.e. head, eye, etc.)");

    if (argc < 3)
    {
        printf(help_str, opt.helpString().c_str());
        exit(1);
    }

    string fd_fn, ed_fn, md_fn, videosource;
    bool low_resolution, display_fps, use_videosource, use_images, search_regions, display_search_rectangles;
    vector<string> images;
    try
    {
        images                    = opt.parse(argc, argv);
        fd_fn                     = opt.getArgument<string>("face-detector");
        ed_fn                     = opt.getArgument<string>("eye-detector", "");
        md_fn                     = opt.getArgument<string>("mouth-detector", "");
        videosource               = opt.getArgument<string>("videosource", "");
        low_resolution            = opt.parameterSet("low-resolution") > 0;
        display_fps               = opt.parameterSet("fps") > 0;
        use_videosource           = opt.parameterSet("videosource") > 0;
        use_images                = opt.parameterSet("images") > 0;
        search_regions            = opt.parameterSet("search-regions") > 0;
        display_search_rectangles = opt.parameterSet("draw-search-rectangles") > 0;
    }
    catch (CommandLineOptionException e)
    {
        printf(help_str, opt.helpString().c_str());
        exit(1);
    }

    if (!use_videosource && !use_images)
    {
        printf("Either '-v' or '-i' must be given\n");
        printf(help_str, opt.helpString().c_str());
        exit(1);
    }

    // Set up detectors
    BinaryPatternFaceDetector fd(fd_fn);
    fd.setScaleStep(1.2);
    fd.setNeighborDist(1.3);
    auto_ptr<BinaryPatternEyeDetector> ed;
    if (!ed_fn.empty())
        ed.reset(new BinaryPatternEyeDetector(ed_fn));
    auto_ptr<BinaryPatternMouthDetector> md;
    if (!md_fn.empty())
        md.reset(new BinaryPatternMouthDetector(md_fn));

    // Eye center detector
    typedef IsophoteEyeCenterDetector<double> iecd_t; // can be instantiated as float and double (choice defines the algorithmic precision [vs. run-time])
    iecd_t iecd;

    /* default parameters */
    // ...
    double isophote_row_sigma = 1;
    double isophote_col_sigma = 1;
    // options
    bool set_auto_isophote_sigma = false;

    // Set up GUI
    GuiThread thread;
    ImageWindow* imgwin = new ImageWindow("Detector Demo (cam)");
    thread.addClient(imgwin);
    // parameter control 
    okapi::WidgetWindow* win_params = new okapi::WidgetWindow("Parameter Window", 350/*widget width*/, 250/*label width*/);
    thread.addClient(win_params);
    // start the GUI thread
    thread.start();
    // set initial values
    win_params->setSlider("isophote row sigma (sigma)",          isophote_row_sigma, 0, 5);
    win_params->setSlider("isophote col sigma (sigma)",          isophote_col_sigma, 0, 5);     
    win_params->setButton("set auto sigma",                      set_auto_isophote_sigma);     
    win_params->setButton("display search rectangles",           display_search_rectangles);

    double old_ts = -1;
    double fps = -1;

    // Set up VideoSource
    auto_ptr<VideoSource> src;
    if (use_videosource)
    {
        if (videosource.empty() || videosource == "")
        {
            if (low_resolution)
                src.reset(createDefaultCamera(320, 240));
            else
                //src.reset(createDefaultCamera(640, 480));
                src.reset(createDefaultCamera(1024,768));
                //src.reset(createDefaultCamera(1600,1200));
        }
        else
        {
            if (videosource == "")
                src.reset(createVideoSourceFromString(videosource));
            else
                src.reset(createVideoSource("/home/bschauer/devel/local/share/okapi/videoio/videosources.xml","grasshopper"));
        }

        if (!src.get())
        {
            printf("Error opening camera device, make sure cam is connected!\n");
            exit(1);
        }
    }

    while (imgwin->getWindowState() && (use_images || src->getNextFrame()))
    {
        if (use_images && images.empty())
        {
            sleep_ms(20);
            continue;
        }

        string fn;
        cv::Mat img;
        if (use_videosource)
           img = src->getImage();
        else
        {
            fn = images.front();
            images.erase(images.begin());
            img = loadImage(fn);
        }
        cv::Mat aimg = img.clone();
        cv::Mat gimg;
        cv::cvtColor(aimg,gimg,CV_RGB2GRAY);
        ImageDeco deco(aimg);
        deco.setAntiAlias(true);

        /* get current parameter values */
        // ...
        set_auto_isophote_sigma = win_params->getButton("set auto sigma");
        if (!set_auto_isophote_sigma)
        {
            win_params->toggleActivated("isophote row sigma (sigma)", true);
            win_params->toggleActivated("isophote col sigma (sigma)", true);
            isophote_row_sigma = win_params->getSlider("isophote row sigma (sigma)");
            isophote_col_sigma = win_params->getSlider("isophote col sigma (sigma)");
        }
        else
        {
            win_params->toggleActivated("isophote row sigma (sigma)", false);
            win_params->toggleActivated("isophote col sigma (sigma)", false);
        }
        display_search_rectangles = win_params->getButton("display search rectangles");

        // Detect faces
        OKAPI_TIMER_START("detect faces");
        vector<RectDetection> faces = fd.detectFaces(img);
        OKAPI_TIMER_STOP("detect faces");

        double fsize=0; //faces[0].box.width * faces[0].box.height;
        size_t i=0;
        for (size_t j=0; j<faces.size(); j++)
        {
            if ((faces[j].box.width * faces[j].box.height) > fsize)
                i = j;
        }

        //for (size_t i=0; i<faces.size(); ++i)
        if (faces.size() > 0)
        {
            EyeLocations eyes(cv::Point(-1, -1), cv::Point(-1, -1));

            // Detect Mouth
            if (md.get())
            {
                cv::Point2f mouth = fd.getMeanMouthCenter(faces[i].box);
                double mouth_width = fd.getMeanMouthWidth(faces[i].box);

                OKAPI_TIMER_START("detect mouth");
                mouth = md->detectMouth(img, mouth, mouth_width);
                OKAPI_TIMER_STOP("detect mouth");

                if (mouth.x >= 0)
                {
                    deco.setColor(255, 0, 255);
                    deco.setThickness(1);
                    deco.drawCircle(cvRound(mouth.x), cvRound(mouth.y), 3);
                }
            }

            // Detect eyes
            if (ed.get())
            {
                cv::Point2f le, re;
                le = fd.getMeanLeftEye(faces[i].box);
                re = fd.getMeanRightEye(faces[i].box);

                OKAPI_TIMER_START("detect eyes");
                eyes = ed->detectEyes(img, le, re);
                OKAPI_TIMER_STOP("detect eyes");

                /*if (search_regions)
                {
                    cv::Rect left_eye_roi, right_eye_roi;
                    int min_result_width, max_result_width;
                    ed->getSearchRegions(img, le, re, left_eye_roi, right_eye_roi,
                                         min_result_width, max_result_width);
                    deco.setThickness(1);
                    deco.setColor(255, 127, 127);
                    deco.drawRect(left_eye_roi);
                    deco.drawRect(right_eye_roi);
                }*/

                // Draw eye locations
                deco.setThickness(1);
                deco.setColor(0, 127, 0);
                if (eyes.left.x > 0)
                    deco.drawCircle(eyes.left.x, eyes.left.y, 3);
                if (eyes.right.x > 0)
                    deco.drawCircle(eyes.right.x, eyes.right.y, 3);
            }

            // Draw rotated face bounding box
            if (eyes.isValid())
            {
                // Draw detected face (lightly)
                deco.setColor(127, 127, 127);
                deco.setThickness(1);
                deco.drawRect(faces[i].box);

                // Draw rotated face box
                cv::Point2f eye_center(eyes.right + eyes.left);
                eye_center *= 0.5f;
                double eye_dist = cv::norm(eyes.right - eyes.left);
                double angle = atan2(eyes.left.y - eyes.right.y, eyes.right.x - eyes.left.x);
                cv::Point2f offset(sin(angle), cos(angle));
                offset *= 0.25f * (float)eye_dist;
                cv::RotatedRect rrect(eye_center + offset, cv::Size2f(eye_dist * 2.0, eye_dist * 1.75), - angle / CV_PI * 180);
                deco.setColor(0, 255, 0);
                deco.setThickness(2);
                deco.drawRect(rrect);

                // Run the eye detector
                if (set_auto_isophote_sigma)
                    iecd.setAutoSigma();
                else
                    iecd.setSigma(isophote_row_sigma,isophote_col_sigma);
                OKAPI_TIMER_START("iecd.detectEyeCenters");
                EyeCenterLocations<iecd_t::coord_t> eye_centers = iecd.detectEyeCenters(gimg,faces[i].box,eyes.left,eyes.right);
                OKAPI_TIMER_STOP("iecd.detectEyeCenters");
                if (set_auto_isophote_sigma)
                {
                    win_params->setSlider("isophote row sigma (sigma)",          iecd.getRowSigma(), 0, 5);
                    win_params->setSlider("isophote col sigma (sigma)",          iecd.getColSigma(), 0, 5);    
                }
                deco.setThickness(2);
                deco.setColor(255,0,0);
                deco.drawCircle(eye_centers.left.x,eye_centers.left.y,3);
                deco.drawCircle(eye_centers.right.x,eye_centers.right.y,3);
                deco.setThickness(1);
                deco.drawLine(eye_centers.left.x,eye_centers.left.y,eye_centers.right.x,eye_centers.right.y);

                // Draw search regions, i.e. regions of interest
                cv::Rect_<iecd_t::coord_t> left_roi, right_roi;
                iecd.getCurrentSearchRegions(left_roi, right_roi);
                if (display_search_rectangles)
                {
                    if (iecd_t::isValidCoord(left_roi))
                    {
                        deco.setThickness(1);
                        deco.setColor(255, 127, 127);
                        deco.drawRect(left_roi);
                    }
                    if (iecd_t::isValidCoord(right_roi))
                    {
                        deco.setThickness(1);
                        deco.setColor(255, 127, 127);
                        deco.drawRect(right_roi); 
                    }
                }
                //cv::Mat left_eye_image(left_roi.height,left_roi.width,img.type());
                //cv::Mat right_eye_image(left_roi.height,left_roi.width,img.type());
                cv::Mat left_eye_image(img,left_roi);
                cv::Mat right_eye_image(img,right_roi);
                cv::Mat left_eye_image_resized;
                cv::Mat right_eye_image_resized;
                cv::resize(left_eye_image,left_eye_image_resized,cv::Size(), 3, 3);
                cv::resize(right_eye_image,right_eye_image_resized,cv::Size(), 3, 3);
                ImageDeco dleir(left_eye_image_resized);
                ImageDeco dreir(right_eye_image_resized);
                dleir.setThickness(2);
                dleir.setColor(255,0,0);
                dleir.drawCircle(3*(eye_centers.left.x - left_roi.x),3*(eye_centers.left.y - left_roi.y),3);
                dreir.setThickness(2);
                dreir.setColor(255,0,0);
                dreir.drawCircle(3*(eye_centers.right.x - right_roi.x),3*(eye_centers.right.y - right_roi.y),3); 
                imgwin->setImage("left_eye_image", left_eye_image_resized);
                imgwin->setImage("right_eye_image", right_eye_image_resized);
            }
            else
            {
                // Draw detected face (strongly)
                deco.setColor(127, 0, 0);
                deco.setThickness(1);
                deco.drawRect(faces[i].box);

                // Run the eye detector
                if (set_auto_isophote_sigma)
                    iecd.setAutoSigma();
                else
                    iecd.setSigma(isophote_row_sigma,isophote_col_sigma);
                OKAPI_TIMER_START("iecd.detectEyeCenters");
                EyeCenterLocations<iecd_t::coord_t> eye_centers = iecd.detectEyeCenters(gimg,faces[i].box,iecd_t::getInvalidCoordPoint(),iecd_t::getInvalidCoordPoint());
                OKAPI_TIMER_STOP("iecd.detectEyeCenters");
                if (set_auto_isophote_sigma)
                {
                    win_params->setSlider("isophote row sigma (sigma)",          iecd.getRowSigma(), 0, 5);
                    win_params->setSlider("isophote col sigma (sigma)",          iecd.getColSigma(), 0, 5);    
                }
                deco.setThickness(1);
                deco.setColor(127,0,0);
                deco.drawCircle(eye_centers.left.x,eye_centers.left.y,3);
                deco.drawCircle(eye_centers.right.x,eye_centers.right.y,3);
                deco.drawLine(eye_centers.left.x,eye_centers.left.y,eye_centers.right.x,eye_centers.right.y);
            }
        }

        if (display_fps)
        {
            double new_ts = src->getTimestamp();

            if (old_ts < 0)
                old_ts = new_ts - 0.1;

            // Compute fps
            double new_fps = 1 / (new_ts - old_ts);
            if (fps < 0)
                fps = new_fps;
            else
                fps = 0.95 * fps + 0.05 * new_fps;

            char buf[1000];
            sprintf(buf, "%4.1f fps\n", fps);
            old_ts = new_ts;

            // Write frame rate
            deco.setColor(255, 255, 255);
            deco.setThickness(1);
            deco.drawText(buf, 0, 15);
        }

        // Display annotated image
        string name;
        if (use_videosource)
            name = "Video";
        else
            name = basename(fn);
        imgwin->setImage(name, aimg, low_resolution ? 2 : 1);
        imgwin->setImage("accumulator",iecd.getMatAcc(), low_resolution ? 2 : 1);
    }

    return 0;
}
