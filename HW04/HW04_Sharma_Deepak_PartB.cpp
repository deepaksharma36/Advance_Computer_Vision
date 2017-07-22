/*
* @file     HW04_Sharma_Deepak_PartB.cpp
* @author  Sharma, Deepak (ds5930@g.rit.edu), ValeryTyumen, sturkmen72,
* RafaRuiz,  RafaRuiz, David Stavens
* @date    7/16/2017
* @version 1.0
*
* @brief CSCI 731, Home Work 3 Part B,
* 1. Lucas Kanade Optical Flow Demo
*
* @section DESCRIPTION
*
* This program find keyPoint dispalcement in all consicutive frames of a
* video. Draw arrow for showing displacement and create an output video
* containing the dispalcement arrows.
* Use command line arguments to specify relative/absolute path of the input
* video.
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


Point2f point;
static const double pi = 3.14159265358979323846;
inline static double square(int a) { return a * a; }
void drawArrows(Point &startPoint, Point &endPoint,  Mat &image, bool &nowDraw);

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
* https://stackoverflow.com/questions/13623394/how-to-write-video-file-in-opencv-2-4-3
*/
int main( int argc, char** argv )
{

    VideoCapture input_cap;
    TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS,20,0.03);
    Size subPixWinSize(10,10), winSize(31,31);
    double fps=0.0;

    const int MAX_COUNT = 500; // Maximum Number of KeyPoints to be selected
    bool nowDraw=false;
    int initCounter=0;
    if( argc == 2 )
        input_cap.open(argv[1]);
    else{
        cout<<"Improper number of arguments, Provide path to two images in arguments"<<endl;
        return 1;
    }

    const char* filePath = argv[1];
        if( !is_file_exist(filePath)){
            cout <<"Bad file path:"<< filePath <<endl;
            return 1;}

    if( !input_cap.isOpened() )
    {
        cout << "Could not initialize capturing...\n";
        return 1;
    }
    fps = input_cap.get(CV_CAP_PROP_FPS);
    cout <<"input Video fps: "<< fps <<endl;

    namedWindow( "Optical Flow", WINDOW_NORMAL);
    string input_video_name = argv[1];
    string output_video_name;
    //            input_video_name.substr(input_video_name.find_first_of('.'));
    output_video_name = output_video_name + "_out" + ".MP4";
    cout << "Output Video Name: "<< output_video_name <<endl;
    //Creating video Writter
    int output_video_fps = 30;
    VideoWriter output_cap(output_video_name,
            CV_FOURCC('X','2','6','4'), //FOURCC for supporting MP4
            output_video_fps,
            Size(input_cap.get(CV_CAP_PROP_FRAME_WIDTH),
                input_cap.get(CV_CAP_PROP_FRAME_HEIGHT)));

    Mat gray, prevGray, image;
    vector<Point2f> startPoints;
    vector<Point2f> endPoints;

    Mat frame, rawFrame;
    int frameCounter=0;
    while(frameCounter++<30)
    {
        input_cap >> rawFrame;
        if( rawFrame.empty() )
            break;
        rawFrame.copyTo(image);
        //rawFrame.copyTo(processedFrame);
        bilateralFilter(rawFrame, frame, 10, 250, 250);
        cvtColor(image, gray, COLOR_BGR2GRAY);
        // Selecting key-points(Harris/Shi-Tomasi(Default)) with both high
        // eignvalue and and storing in startPoints vector.
        float quality_level = 0.01; //Parameter characterizing the minimal accepted quality of image corners
        int min_distance = 10; // Discard corner this close
        int block_size = 3; //Neighbourhood used/patch size
        bool useHarrisDetector = false;

        if(frameCounter%30==0 || frameCounter==1){
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
        }
        if( !startPoints.empty() )
        {
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
                    drawArrows(startPoint, endPoint,  image, nowDraw); }
            //startPoints.resize(k);
            endPoints.resize(foundFeaturePointCounter);
            std::swap(startPoints, endPoints);
        }
        imshow("Optical Flow", image);
        output_cap.write(image);
        char c = (char)waitKey(1);
        if( c == 27 )
            break;
        //swapping points and frames
        cv::swap(prevGray, gray);
        nowDraw=true;
    }
    return 0;
}

/*
* @brief  Draw arrows
* @param  startPoint, Point type,  start coordinates of arrow
* @param  endPoint, Point type,  end coordinates of arrow
* @param  image, Image on which arrow needs to be draw.
* code source: http://robots.stanford.edu/cs223b05/notes/CS%20223-B%20T1%20stavens_opencv_optical_flow.pdf
*/
void drawArrows(Point &startPoint, Point &endPoint,  Mat &image, bool &nowDraw)
    {


    Point tipStartPoint; // tip of the tail of arrow
    Point tipEndPoint; // tip of the head of the arrow
    //angle for arrow head
    double angle;
    angle = atan2( (double) startPoint.y - endPoint.y,
            (double) startPoint.x - endPoint.x );
    double hypotenuse;
    //arow head length
    hypotenuse = sqrt( square(startPoint.y - endPoint.y) + square(startPoint.x - endPoint.x) );

    /* Here we lengthen the arrow by a factor of three. */
    //tipEndPoint.x = (int) (startPoint.x - 3 * hypotenuse * cos(angle));
    //tipEndPoint.y = (int) (startPoint.y - 3 * hypotenuse * sin(angle));

    // Colors
    Scalar colorScalar_megenta = cv::Scalar( 255, 0, 255 );
    Scalar colorScalar_green = cv::Scalar( 0, 255, 0 );
    Scalar colorScalar_red = cv::Scalar( 0, 0, 255 );
    if(nowDraw){
    line( image, startPoint, endPoint, colorScalar_megenta, 4 );
    //cvLine( frame1, startPoint, endPoint, line_color, line_thickness, CV_AA, 0 );
    /* Now draw the tips of the arrow.  I do some scaling so that the
        * tips look proportional to the main line of the arrow.*/

    // Creating arow tip for line
    /*
    tipStartPoint.x = (int) (endPoint.x + 9 * cos(angle + pi / 4));
    tipStartPoint.y = (int) (endPoint.y + 9 * sin(angle + pi / 4));
    if(startPoint.y > endPoint.y)
        line( image, tipStartPoint, startPoint, colorScalar_green);
    else
        line( image, tipStartPoint, startPoint, colorScalar_red);

    tipStartPoint.x = (int) (endPoint.x + 9 * cos(angle + pi / 4));
    tipStartPoint.y = (int) (endPoint.y + 9 * sin(angle + pi / 4));
    if(startPoint.y > endPoint.y)
        line( image, tipStartPoint, endPoint, colorScalar_green  );
    else
        line( image, tipStartPoint, endPoint, colorScalar_red  );
*/
    }
    }
