/*
 *  stereo_match.cpp
 *  calibration
 *
 *  Created by Victor  Eruhimov on 1/18/10.
 *  Copyright 2010 Argus Corp. All rights reserved.
 *  Edited by Deepak Sharma
 *
 */

/*
* @file     HW05_Sharma_Deepak_PartC.cpp
* @author  Sharma, Deepak (ds5930@g.rit.edu),
*  Victor  Eruhimov on 1/18/10.
* @date    7/24/2017
* @version 1.0
*
* @brief CSCI 731, Home Work 5 Part C,
* 1. Calculate DisparityMap using left and right images
*
* @section DESCRIPTION
*
* 1. Calculate DisparityMap using left and right images
* Use command line arguments to specify relative/absolute path of the
* input images.
* CMD:
* ./HW05_Sharma_Deepak_PartC <path to left Image> <path to right Image>
* Program will exit, in case of missing or invalid file path``
* argument C++ documentation has been learned from:
* http://csweb.cs.wfu.edu/~fulp/CSC112/codeStyle.html
* IMPORTANT MESSAGE: Many parts of the code is based on the demo code provided
* in opencv:
* https://github.com/npinto/opencv/blob/master/samples/cpp/stereo_match.cpp
*/
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core/utility.hpp"

#include <stdio.h>
#include "iostream"

using namespace cv;
using namespace std;


int main(int argc, char** argv)
{
    cout<<"Cmd for run is: ./HW05_Sharma_Deepak_PartC <Path to image Left> <Path to image Right> "<<endl;
    std::string img1_filename = "";
    std::string img2_filename = "";
    std::string disparity_filename = "HW05_PartC_Sharma_Deepak_Disparity_Map.JPG";

    int SADWindowSize, numberOfDisparities;
    float scale;

    Ptr<StereoBM> bm = StereoBM::create(16,9);
    img1_filename = argv[1];// parser.get<std::string>(0);
    img2_filename = argv[2];// parser.get<std::string>(1);
    numberOfDisparities = 256; // parser.get<int>("max-disparity");
    SADWindowSize = 61;// parser.get<int>("blocksize");
    scale = 1;//parser.get<float>("scale");

    Mat img1 = imread(argv[1], 0);
    Mat img2 = imread(argv[2], 0);



    Size img_size = img1.size();

    //Rect roi1, roi2;
    //Mat Q;


    //bm->setROI1(roi1);
    //bm->setROI2(roi2);
    bm->setPreFilterCap(31);
    bm->setBlockSize(SADWindowSize > 0 ? SADWindowSize : 9);
    bm->setMinDisparity(0);
    bm->setNumDisparities(numberOfDisparities);
    bm->setTextureThreshold(10);
    bm->setUniquenessRatio(15);
    bm->setSpeckleWindowSize(100);
    bm->setSpeckleRange(32);
    bm->setDisp12MaxDiff(1);

    int cn = img1.channels();


    Mat disp, disp8;

    bm->compute(img1, img2, disp);

    disp.convertTo(disp8, CV_8U, 255/(numberOfDisparities*16.));
    namedWindow("left", WINDOW_NORMAL);
    imshow("left", img1);
    namedWindow("right", WINDOW_NORMAL);
    imshow("right", img2);
    double min, max;
    cv::minMaxLoc(disp8, &min, &max);
    cout<<"Relative Max Distance: "<<max<<endl;
    cout<<"Relative Min Distance: "<<min<<endl;
    imwrite(disparity_filename, disp8);
    namedWindow("disparity", 0);
    imshow("disparity", disp8);
    waitKey();

    return 0;
}
