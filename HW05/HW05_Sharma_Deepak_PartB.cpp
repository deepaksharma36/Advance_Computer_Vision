/*
* @file     HW05_Sharma_Deepak_PartB.cpp
* @author  Sharma, Deepak (ds5930@g.rit.edu),
* taka-no-me,Valery Tyumen,
* Steven Puttemans, Adil Ibragimo
*
* @date    7/24/2017
* @version 1.0
*
* @brief CSCI 731, Home Work 5 Part B,
* 1. Using Camera Parameters, remove distortion from Input Image
*
* @section DESCRIPTION
*
* This program use camera Parameters for removing distortion form image
* Use command line arguments to specify relative/absolute path of the
* input image
* CMD:
* ./HW05_Sharma_Deepak_PartB <Path to input Images>
* Program will exit, in case of missing or invalid dir path
* argument C++ documentation has been learned from:
* http://csweb.cs.wfu.edu/~fulp/CSC112/codeStyle.html
* IMPORTANT MESSAGE: Many parts of the code is based on the demo code provided
* in opencv:
* https://github.com/opencv/opencv/blob/master/samples/cpp/calibration.cpp
*/
#include "opencv2/core.hpp"
#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "iostream"
#include <cctype>
#include <stdio.h>
#include <string.h>
#include <time.h>

using namespace cv;
using namespace std;

//Main method
int main( int argc, char** argv )
{
    cout<<"Correct cmd is: ./HW05_Sharma_Deepak_PartB <Path to input image>"<<endl;
    cout<<"This program uses file: HW05_Sharma_Deepak_Camera_Parameters, make sure it is avaialbal in the working dirctory, else change line 47 in the code "<<endl;
    //Loading camera parameters file
    FileStorage fs( "HW05_Sharma_Deepak_Camera_Parameters.yml", FileStorage::READ);
    //fs.open("camera.yml", cv::FileStorage::READ);
    Mat cameraMatrixLoaded, distCoeffsLoaded ;
    Size imageSize;
    //Loading parameters from file
    fs["intrinsic_matrix"] >> cameraMatrixLoaded;
    fs["distortion_coefficients"] >> distCoeffsLoaded;
    fs["image_width"] >> imageSize.width;
    fs["image_height"] >> imageSize.height;

    cout<<"Generating Maps"<<endl;
    Mat view, rview, map1, map2;
    view = imread(argv[1], 1);
    // Creating Rectify Maps
    initUndistortRectifyMap(cameraMatrixLoaded, distCoeffsLoaded, Mat(),
                            getOptimalNewCameraMatrix(cameraMatrixLoaded,
                            distCoeffsLoaded, imageSize, 1, imageSize, 0),
                            imageSize, CV_16SC2, map1, map2);
    cout<<"Showing Results"<<endl;
    // Mapping the input image for removing camera distortions
    remap(view, rview, map1, map2, INTER_LINEAR);

    //undistort(view, rview, cameraMatrixLoaded,
    //        distCoeffsLoaded, cameraMatrixLoaded);
    namedWindow("Distored", WINDOW_NORMAL );
    namedWindow("Undistored", WINDOW_NORMAL );
    namedWindow("Diff", WINDOW_NORMAL );
    imshow("Undistored", rview);
    waitKey(1);
    imshow("Distored", view);
    waitKey(2);
    Mat diff_image = rview - view;
    imshow("Diff", diff_image);
    waitKey(0);
    std::string original = "HW05_PartB_Sharma_Deepak_Distorted.JPG";
    std::string Mapped = "HW05_PartB_Sharma_Deepak_Mapped.JPG";
    std::string difference = "HW05_PartB_Sharma_Deepak_Difference.JPG";

    imwrite(original, view);
    imwrite(Mapped, rview);
    imwrite(difference, diff_image);

    return 0;
}
