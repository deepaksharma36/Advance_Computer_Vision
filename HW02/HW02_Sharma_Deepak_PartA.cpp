/*
* @file     HW02_Sharma_Deepak_PartA.cpp
* @author  Sharma, Deepak (ds5930@g.rit.edu), Valery Tyumen, Steven Puttemans, avdmitry
* @date    7/01/2017
* @version 1.0
*
* @brief CSCI 731, Home Work 2 Part A,
* 1. Perform watershed Segmentation
*
* @section DESCRIPTION
*
* This program watershed segmentation,
*
* Use command line arguments to specify relative/absolute path of the
* input image. Program will exit, in case of missing or invalid image
* path argument C++ documentation has been learned from:
* http://csweb.cs.wfu.edu/~fulp/CSC112/codeStyle.html
* IMPORTANT MESSAGE: Many parts of the code is based on the demo code provided
* in opecv watershed example.(http://docs.opencv.org/trunk/d8/da9/watershed_8cpp-example.html)
* Objective of this program is to conduct experiment with images provided by
* Professor to understand the limitations of the algorithm and suggesting
* preprocessing steps/image transformation techique for improving results for
* watershed algorithm.
*/

#include "opencv2/highgui/highgui.hpp"		// For OpenCV Windowing and display commands.
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "iostream" 				// For C++ IO
#include "stdexcept"                // For raising exceptions
#include <fstream>                  // For aserting file path
using namespace std;
using namespace cv;

/*
* @brief  Check validity of input file path.
* @param  file Path, string containg file path.
* @return boolen True or False
* code source: https://stackoverflow.com/questions/12774207/fastest-way-to-check
* -if-a-file-exist-using-standard-c-c11-c
*/
bool is_file_exist(const char* &filePath)
    {
        ifstream ifStreamObj(filePath);
        return ifStreamObj.good(); //Check source of input stream is okay

    }

/*
* @brief  Show an image stored at given secondary memory locatation.
* @param  image Path, string containg image path.
* @param  window_name, string containing name of the window
* @return void
*/
void showImage(Mat &image, const string &window_name)
    {
    //Mat image = imread(imagePath);     // Reading/loading image in Mat object
    namedWindow(window_name, WINDOW_AUTOSIZE);  // For showing image creating an image holder window
    imshow( window_name, image);                // Showing image in the image holder window
    waitKey(0);                                 // Waiting until user does't hit any key
    destroyWindow(window_name);                 // Destorying image holder window
    }

/*
* @brief  Extract Image channels from the input image.
* @param  image, an Mat object containing data of input image
* @return An vector of Mat objects, containing channels(BGR) of input image.
* Code source/Help: http://answers.opencv.org/question/59529/how-do-i-/
* separate-the-channels-of-an-rgb-image-and-save-each-one-using-\
* the-249-version-of-opencv/
*/
void getImageChannels(Mat &image, vector<Mat> &channels)
    {
    //Mat image = imread(imagePath);
    int nChannels = image.channels(); //For storing number of channels
    int nRows = image.rows;           //For storing number of rows
    //int nColsOneChannel = image.cols;
    int nCols = image.cols;           //For storing number of cols
    //vector<Mat> channels;             // For storing channels
    // Intializing channels with 0 value
    for (int channelCounter=0; channelCounter<nChannels; channelCounter++)
        {
        channels.push_back(Mat(nRows, nCols, CV_8UC1, Scalar(0)));
        }
    // Iterating over image pixels and accessing intensity value for each
    // channel
    for (int rowCounter=0; rowCounter<nRows; rowCounter++)
        {
        for(int colCounter=0; colCounter<nCols; colCounter++)
            {
            // Acessing intensity of a pixel at specified location
            Vec3b val = image.at<Vec3b >(rowCounter, colCounter);
            for (int channelCounter=0; channelCounter<nChannels;\
                    channelCounter++)
                {
                // Assigning intensity values to corresponding channel image
                channels[channelCounter].at<uchar>(rowCounter, colCounter)=\
                                                        val[channelCounter];
                }
            }
        }
            //return &channels;
    }


Mat markerMask, image; //Holds masks provided by user
Point prevPt(-1, -1); //Holds coordinate value of the mouse Moves
/*
* @brief  Mouse event handler, response to the mouse actions.
* @param  event, integer type event_id corresponding to the action taken by user
* @param  x, y integer coordinate of curser position
* Code Source/Help:https://github.com/opencv/opencv/blob/master/samples/cpp/watershed.cpp
*/
static void onMouse( int event, int x, int y, int flags, void* )
{
    if( x < 0 || x >= image.cols || y < 0 || y >= image.rows )
        return;
    // if left button of mouse has been clicked up
    if( event == EVENT_LBUTTONUP || !(flags & EVENT_FLAG_LBUTTON) )
        prevPt = Point(-1,-1);
    // if left button of mouse has been clicked down
    else if( event == EVENT_LBUTTONDOWN )
        prevPt = Point(x,y);
    // if mouse has been moved and if this is the target movement
    else if( event == EVENT_MOUSEMOVE && (flags & EVENT_FLAG_LBUTTON) )
    {
        Point pt(x, y); // created Point object with mouse click location,
        //before it started movement
        if( prevPt.x < 0 )
            prevPt = pt;  // if selected point is out of frame then set it to
        //previous location
        //created a line segment between current and previous location of
        //the mouse in the binary image markermask(Global Variable).
        line( markerMask, prevPt, pt, Scalar::all(255), 5, 8, 0 );
        //For showing the mouse movement  to the user, also overlayed line
        //segment on original image.
        line( image, prevPt, pt, Scalar::all(255), 5, 8, 0 );
        prevPt = pt;
        imshow("image", image);
    }
}

/*
* @brief  Fill random pseudo colors in the connected components.
* @param  markers, an input Mat object containing connected componts and background
* information.
* @param  wshed, an output Mat object containing pseudo colored connected componts
* information.
* Code Source/Help:https://github.com/opencv/opencv/blob/master/samples/cpp/watershed.cpp
*/
void paintWatershad(Mat &markers, Mat &wshed, int &compCount,
        vector<Vec3b> &colorTab){
        for( int row_index = 0; row_index < markers.rows; row_index ++ )
            for(int  col_index = 0; col_index < markers.cols; col_index ++ )
            {
                int marker_val = markers.at<int>(row_index, col_index);
                if( marker_val == -1 )
                    wshed.at<Vec3b>(row_index, col_index) = Vec3b(255,255,255);
                else if( marker_val <= 0 || marker_val > compCount )
                    wshed.at<Vec3b>(row_index, col_index) = Vec3b(0,0,0);
                else
                    wshed.at<Vec3b>(row_index, col_index) =\
                                                    colorTab[marker_val - 1];
            }
}

/*
* @brief Create a color box by generating random numbers, colorBox contations
* number of colors equals to number of connected components.
* @param  compCount, number of connected componenets.
* @param  colorTab, an output Vector of Mat objects containing pseudo colores.
* Code Source/Help:https://github.com/opencv/opencv/blob/master/samples/cpp/watershed.cpp
* http://docs.opencv.org/trunk/d2/de8/group__core__array.html#ga75843061d150ad6564b5447e38e57722
*/
void createColorTab(vector<Vec3b> &colorTab,  int &compCount){
            for(int cc_index = 0; cc_index < compCount; cc_index++ )
            {
                //The function cv::theRNG returns the default random number
                //generator.
                int blue = theRNG().uniform(0, 255);
                int green = theRNG().uniform(0, 255);
                int red = theRNG().uniform(0, 255);
                colorTab.push_back(Vec3b((uchar)blue,
                                        (uchar)green,
                                        (uchar)red));
            }
}


/*
* @brief Perform supervisied watershed segmentation on input image and image
* generated after performing bilinar transform on input image.
* number of colors equals to number of connected components.
* @param  image_blur, Image generated after performing Bilateral trasform.
* @param  image_org, Original BGR Image.
* @param  markers, Mat object containing generated contures for user input.
* @param  colorTab, an Vector of Mat objects containing pseudo colores.
* @param  compCount, Number of connected components.
* Code Source/Help:https://github.com/opencv/opencv/blob/master/samples/cpp/watershed.cpp
* https://stackoverflow.com/questions/14582082/merging-channels-in-opencv
* https://stackoverflow.com/questions/13133055/opencv-displaying-2-images-adjacently-in-the-same-window
* http://docs.opencv.org/3.0-beta/modules/imgproc/doc/drawing_functions.html#puttext
*/
void watershedExperimet(Mat &image_blur, Mat &image_org, Mat &markers,
        int &compCount, vector<Vec3b> &colorTab, const char* &imagePath)
{

    Mat markers_copy;
    markers.copyTo(markers_copy);
    watershed(image_blur, markers);
    watershed(image_org, markers_copy);
    Mat wshed(markers.size(), CV_8UC3);
    Mat wshed_with_org(markers_copy.size(), CV_8UC3);
    paintWatershad(markers, wshed, compCount,  colorTab);
    paintWatershad(markers_copy, wshed_with_org, compCount,  colorTab);
    putText(wshed, "Using Bilateral Filter.",
                cvPoint(30,30),
                FONT_HERSHEY_PLAIN, 2,
                cvScalar(0,255,0), .5, CV_AA);
    putText(wshed_with_org, "Without using Bilateral Filter.",
                cvPoint(30,30),
                FONT_HERSHEY_PLAIN, 2,
                cvScalar(0,0,255), .5, CV_AA);
    // paint the watershed image_org
    vector<Mat> fakeChannels;
    for(int channel_index=0; channel_index<3; channel_index++)
        fakeChannels.push_back(markerMask);
    Mat fake3CMarkers;
    merge(fakeChannels, fake3CMarkers);
    image_org = image_org*0.5 + fake3CMarkers*0.5;
    Size imageSize = image_org.size();
    Mat combined(imageSize.height, 3*imageSize.width, CV_8UC3);
    wshed.copyTo(combined(Rect(0,0, imageSize.width,
                    imageSize.height)));
    image_org.copyTo(combined(Rect(imageSize.width,0, imageSize.width,
                    imageSize.height)));
    wshed_with_org.copyTo(combined(Rect(imageSize.width*2, 0, imageSize.width,
                    imageSize.height)));
    imshow( "watershed transform", combined);
    imwrite(static_cast<string>(imagePath)+"_watershed_OUTPUT.jpg", combined);
    waitKey(0);}


/*
* @brief  Main method, invoke functionalities for showing input image,
* @param  argc, an integer variable containing number of arguments.
* @param  argv, an char array containing command line arguments.
* @return program status, an integer number[0, 1].
* code source/Help: https://stackoverflow.com/questions/36057028/throw-exception-if-
* the-file-does-not-exist-in-constructor-and-try-catch-it-when
* http://answers.opencv.org/question/7682/copyto-and-clone-functions/
*/
int main( int argc, char** argv )
{

    int status = 0; //record execution status
    if ( argc < 2 )
    // Asserting number of command line arguments provided by user
    {
        cerr << "Error: Missing arguments for image path"<<endl;
        return 1; 			// Return an error code.
    }
    try
        {
        const char* imagePath = argv[1];
        if (!is_file_exist(imagePath))
            {cerr<<"Image file Does not exist"<<endl;
            throw invalid_argument("Invalid file path");}
        image = imread(imagePath);     // Reading/loading image in Mat object
        Mat image_org;
        image.copyTo(image_org); //creating a copy of the input image.
        Mat image_blur;
        //Applied bilateral filler on input image
        bilateralFilter(image, image_blur, 10, 250, 250);
        //For showing image creating an image holder window
        namedWindow("image", WINDOW_AUTOSIZE);
        imshow("image", image);
        cvtColor(image, markerMask, COLOR_BGR2GRAY);
        //created an empty mask for holding mask provided by user.
        markerMask = Scalar::all(0);
        //activating event handler
        setMouseCallback( "image", onMouse, 0 );
        char c = (char)0;
        while(c!=27){//holding image for getting user inputs
            cout<<"Press ESC for exit"<<endl;
            c = (char)waitKey(0);
            int i, j, compCount = 0;
            vector<vector<Point> > contours;
            //Contains all countorus by holding contour points for each
            //each countor, output of findContours method will be stored
            //in this container.
            //Contour represents a curve using set of points.
            //findContours generate these set of points for a given
            //binary image.
            vector<Vec4i> hierarchy;
            // Contain contour hierachy(releationship between contours)
            //In the presence of mutiple contours, for defining the
            //relationship between contours, findcontor povide relationship
            //between contorus in hierachy container.

            findContours(markerMask, contours, hierarchy,
                    RETR_CCOMP, CHAIN_APPROX_SIMPLE);
            if( contours.empty() ){
                cout<<"No point is present in contoures"<<endl;
                continue;}
            Mat markers(markerMask.size(), CV_32S);
            markers = Scalar::all(0);
            int idx = 0;
            for( ; idx >= 0; idx = hierarchy[idx][0], compCount++ )
                drawContours(markers, contours, idx,
                        Scalar::all(compCount+1), -1, 8, hierarchy, INT_MAX);
            //creating a mask image where countours have been given
            // different integer numbers, this number has been used as
            // intensith value for all the points associated with that contour.
            if( compCount == 0 ){
                cout<<"No connected componets found"<<endl;
                continue;}
            cout<<"Number of connected component  "<<compCount<<endl;
            vector<Vec3b> colorTab;
            createColorTab(colorTab, compCount);
            watershedExperimet(image_blur, image_org, markers ,compCount,
                    colorTab, imagePath);
            return 0;}

        }
    catch(exception &ex){
        cerr<<"Bellow error occured:\n "<<ex.what()<<endl;
        cerr<<"Exiting"<<endl;
        return 1;}
}


