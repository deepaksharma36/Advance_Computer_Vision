/*
* @file     HW04_Sharma_Deepak_PartA.cpp
* @author  Sharma, Deepak (ds5930@g.rit.edu), ValeryTyumen, sturkmen72,
* RafaRuiz, David Stavens
*
* @date    7/16/2017
* @version 1.0
*
* @brief CSCI 731, Home Work 3 Part A,
* 1. Lucas Kanade Optical Flow Demo
*
* @section DESCRIPTION
*
* This program find keyPoint dispalcement in two images.
* Use command line arguments to specify relative/absolute path of the
* two consicutive input video frames.
* Program will exit, in case of missing or invalid video path
* argument C++ documentation has been learned from:
* http://csweb.cs.wfu.edu/~fulp/CSC112/codeStyle.html
* IMPORTANT MESSAGE: Many parts of the code is based on the demo code provided
* in opencv lucas kanade example:
* https://github.com/opencv/opencv/blob/master/samples/cpp/lkdemo.cpp
*/
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <ctype.h>
#include<math.h>
#include <fstream>                  // For aserting file path
using namespace cv;
using namespace std;
static const int LINE_THICKNESS = 10;


Point2f point;
static const double pi = 3.14159265358979323846;

inline static double square(int a) { return a * a; }
void drawArrows(Point &startPoint, Point &endPoint,  Mat &image);

/*
* @brief  Check validity of input file path.
* @param  file Path, string containg file path.
* @return boolen True or False
* code source: https://stackoverflow.com/questions/12774207/fastest-way-to-check
* -if-a-file-exist-using-standard-c-c11-c
*/
bool is_file_exist(const char* &filePath) {
        //Check source of input stream is okay
        ifstream ifStreamObj(filePath);
        return ifStreamObj.good();
}


/*
* @brief  Main method, Process input images, select features points,
* Track feature points using optical flow.
* @param  argc, an integer variable containing number of arguments.
* @param  argv, an char array containing command line arguments.
* @return program status, an integer number[0, 1].
* code source/Help: https://stackoverflow.com/questions/36057028/throw-exception-if-
* the-file-does-not-exist-in-constructor-and-try-catch-it-when
* http://answers.opencv.org/question/7682/copyto-and-clone-functions/
*/
int main( int argc, char** argv )
{
    Mat gray, rawGray, prevGray, rawPrevGray, image, imagePre, imagePost;
    const int MAX_COUNT = 500; // Maximum Number of KeyPoints to be selected
    bool needToInit = true;
    //int initCounter=0;
    if( argc == 3 ){
        const char* filePath1 = argv[1];
        const char* filePath2 = argv[2];
        if( !is_file_exist(filePath1)){
            cout <<"Bad file path:"<< filePath1 <<endl;
            return 1;}
        if( !is_file_exist(filePath2)){
            cout <<"Bad file path:"<< filePath2 <<endl;
            return 1;}
        // Reading input images and converting to gray scale.
        rawPrevGray = imread(argv[1], 0);
        rawGray = imread(argv[2], 0);
        // Applying on bilateralFilters for improving sharpness and
        // removing noise by gaussian smoothing
        bilateralFilter(rawPrevGray, prevGray, 10, 250, 250);
        bilateralFilter(rawGray, gray, 10, 250, 250);
        imagePre = imread(argv[1], 1);
        imagePost = imread(argv[2], 1);
        //Creating an image by adding two consicutive frames.
        addWeighted(imagePre, .5, imagePost, .5, 0.0, image);
    }

    else
    {
        cout<<"Improper number of arguments, Provide path to two images in arguments"<<endl;
        return 1;
    }
    // Creating window for showing output
    namedWindow( "Displacements", WINDOW_NORMAL );
    namedWindow( "Displacements on post", WINDOW_NORMAL );
    //Creating two point type array for storing intial and final location
    //of key-points.
    vector<Point2f> startPoints;
    vector<Point2f> endPoints;

    // Selecting key-points(Harris/Shi-Tomasi(Default)) with both high
    // eignvalue and and storing in startPoints vector.
    float quality_level = 0.01;//Parameter characterizing the minimal accepted quality of image corners
    int min_distance = 10; // Discard corner this close
    int block_size = 3; //Neighbourhood used/patch size
    bool useHarrisDetector = false;
    goodFeaturesToTrack(gray, startPoints,
            MAX_COUNT, quality_level, min_distance, Mat(), block_size,
            useHarrisDetector);
    //Creating termination critaria for Cornersubpix
    //Termination critaria decide the level of accuracy while converting
    //Integer corrdinates to real valued coordinates using subPix
    TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS,20,0.03);
    // Window sizes for Cornersubpixel method and optical flow patch size
    Size subPixWinSize(20,20), winSize(31,31);
    //convert corner coordinates to real coordinates,
    //performe inplace conversion(output will be store in startPoints)
    cornerSubPix(gray, startPoints, subPixWinSize,
            Size(-1,-1), termcrit);
    needToInit=0;
    vector<uchar> status; // For each point found=1  else 0
    vector<float> err; //For storing error measure for found points
    if(prevGray.empty())
        gray.copyTo(prevGray);
    int npramindLayers = 3; // Number of layers for LK method
    //Calculate optical flow of keypoints stored in start points
    //provide associated points in endPoints
    calcOpticalFlowPyrLK(prevGray, gray, startPoints, endPoints,
            status, err, winSize, npramindLayers, termcrit, 0, 0.001);
    size_t featurePointCounter, foundFeaturePointCounter=0;
    for(featurePointCounter = 0; featurePointCounter < startPoints.size();
            featurePointCounter++)
        {
                if( !status[featurePointCounter] )
                    continue;
            endPoints[foundFeaturePointCounter++] = endPoints[featurePointCounter];
            Point startPoint,endPoint;
            startPoint.x = (int) startPoints[featurePointCounter].x;
            startPoint.y = (int) startPoints[featurePointCounter].y;
            endPoint.x = (int) endPoints[foundFeaturePointCounter -1].x;
            endPoint.y = (int) endPoints[foundFeaturePointCounter -1].y;;
            drawArrows(startPoint, endPoint,  image);
            drawArrows(startPoint, endPoint,  imagePost);
        }

           endPoints.resize(foundFeaturePointCounter);
    imshow("Displacements", image);
    imshow("Displacements on post", imagePost);
    imwrite("./HW04_Sharma_Deepak_PartA_OUTPUT_Solo.jpg", image);
    imwrite("./HW04_Sharma_Deepak_PartA_OUTPUT_combo.jpg", imagePost);
    waitKey(0);
    return 0;
}

/*
* @brief  Draw arrows
* @param  startPoint, Point type,  start coordinates of arrow
* @param  endPoint, Point type,  end coordinates of arrow
* @param  image, Image on which arrow needs to be draw.
* code source: http://robots.stanford.edu/cs223b05/notes/CS%20223-B%20T1%20stavens_opencv_optical_flow.pdf
*/

void drawArrows(Point &startPoint, Point &endPoint,  Mat &image) {
    Point tipStartPoint; // tip of the tail of arrow
    Point tipEndPoint; // tip of the head of the arrow
    double angle;
    //angle for arrow head
    angle = atan2( (double) startPoint.y - endPoint.y,
            (double) startPoint.x - endPoint.x );
    //arow head length
    double hypotenuse;
    hypotenuse = sqrt( square(startPoint.y - endPoint.y) + square(startPoint.x - endPoint.x) );
    /* Here we lengthen the arrow by a factor of three. */
    tipEndPoint.x = (int) (startPoint.x - 3 * hypotenuse * cos(angle));
    tipEndPoint.y = (int) (startPoint.y - 3 * hypotenuse * sin(angle));
    // Colors
    cv::Scalar colorScalar_maganta = cv::Scalar( 255, 0, 255 );
    Scalar arrow_head_color = Scalar( 0, 0, 255 );
    line( image, startPoint, endPoint, colorScalar_maganta, LINE_THICKNESS);
    //cvLine( frame1, startPoint, endPoint, line_color, line_thickness, CV_AA, 0 );
    /* Now draw the tips of the arrow.  I do some scaling so that the
        * tips look proportional to the main line of the arrow.
    */

    tipStartPoint.x = (int) (tipEndPoint.x + 9 * cos(angle + pi / 4));
    tipStartPoint.y = (int) (tipEndPoint.y + 9 * sin(angle + pi / 4));
    //line( image, tipStartPoint, tipEndPoint, arrow_head_color);
    tipStartPoint.x = (int) (tipEndPoint.x + 9 * cos(angle - pi / 4));
    tipStartPoint.y = (int) (tipEndPoint.y + 9 * sin(angle - pi / 4));
    //line( image, tipStartPoint, tipEndPoint, arrow_head_color  );
    }
