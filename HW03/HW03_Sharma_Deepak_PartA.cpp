/*
* @file     HW03_Sharma_Deepak_PartA.cpp
* @author  Sharma, Deepak (ds5930@g.rit.edu)
* @date    7/08/2017
* @version 1.0
*
* @brief CSCI 731, Home Work 3 Part A,
* 1. Perform watershed Segmentation
*
* @section DESCRIPTION
*
* This program create collage for input video:
* DR_PEDESTRIAN_2017_06_20_MOV.MOV
* Use command line arguments to specify relative/absolute path of the
* input video. Program will exit, in case of missing or invalid video path
* argument C++ documentation has been learned from:
* http://csweb.cs.wfu.edu/~fulp/CSC112/codeStyle.html
* IMPORTANT MESSAGE: Many parts of the code is based on the demo code provided
* in opencv background substraction example:
* http://docs.opencv.org/3.1.0/d1/dc5/tutorial_background_subtraction.html
*/
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <iostream>
#include <fstream>                  // For aserting file path
//#include <sstream>
using namespace cv;
using namespace std;
// Global constants
int const ASPACT_RATIO = 1.2;
int const MAX_AREA = 3000;
int const MIN_AREA = 500;
int const MORPH_SIZE = 20;
int const FRAME_DROP_RATIO = 60;
int const WAIT = 5;

void processVideo(char* videoFilename);
void processForeground(Mat &fgMaskMOG2, Mat &cleanFGMask);

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
* @brief  Main method, invoke functionalities for creating collage,
* @param  argc, an integer variable containing number of arguments.
* @param  argv, an char array containing command line arguments.
* @return program status, an integer number[0, 1].
* Code help/source:
* http://docs.opencv.org/3.1.0/d1/dc5/tutorial_background_subtraction.html
*/
int main(int argc, char* argv[]) {
    if(argc != 2) {
        cerr <<"Incorret input list" << endl;
        cerr <<"exiting..." << endl;
        return EXIT_FAILURE;
    }
    else{
        const char* filePath = argv[1];
        if (!is_file_exist(filePath))
            {cerr<<"Image file Does not exist"<<endl;
        return EXIT_FAILURE; }
        namedWindow("Process", WINDOW_NORMAL);
        processVideo(argv[1]);}
    destroyAllWindows();
    return EXIT_SUCCESS;}

/*
* @brief  Read input video, process frames of input video by
* performing background subtraction, create collage, show output.
* @param  videoFileName, Relative/Absulute path to input video.
* Code help/source:
* http://docs.opencv.org/3.1.0/d1/dc5/tutorial_background_subtraction.html
*/
void processVideo(char* videoFileName) {
    //create the capture object
    char keyboard; //input from keyboard
    Size imageSize;
    Mat frame, fgMaskMOG2, fake3CfgMaskMOG2, output, combined,\
        previousMask, overlappingMask, cleanFgMask;
    Ptr<BackgroundSubtractor> pMOG2; //MOG2 Background subtractor
    //create Background Subtractor objects
    pMOG2 = createBackgroundSubtractorMOG2(); //MOG2 approach
    int iter_count = 1;
    //create video capture object for reading video.
    VideoCapture capture(videoFileName);
    if(!capture.isOpened()){
        //error in opening the video input
        cerr << "Unable to open video file: " << videoFileName << endl;
        exit(EXIT_FAILURE);
    }
    //read input data. ESC or 'q' for quitting
    keyboard = 0;
    while( keyboard != 'q' && keyboard != 27 ){
        //read the current frame
        if(!capture.read(frame)) {
            cerr << "Unable to read next frame." << endl;
            cerr << "Exiting..." << endl;
            break;
        }
        if (iter_count==1){
            //intializing variable at first itereation.
            imageSize = frame.size(); //Mesuring size of image frames.
            //Creating image for holding output progress.
            combined = Mat(imageSize.height, 2*imageSize.width, CV_8UC3);
            //Intialing output image for holding collage.
            frame.copyTo(output);
            // Maintaining Forground objects history for avoiding
            // overlapping.
            previousMask = Mat(frame.size(), CV_8UC1, Scalar(0));}
        iter_count+=1;
        //update the background model
        pMOG2->apply(frame, fgMaskMOG2);
        if (iter_count%FRAME_DROP_RATIO != 0){
            //Performing samping
            continue;}
        //Cleaning foreground Mask, removing unwanted componenets.
        processForeground(fgMaskMOG2, cleanFgMask);
        //Itersection operation between new foreground and old foreground
        bitwise_and(previousMask, cleanFgMask, overlappingMask);
        //Checking overlapping between old and new foreground
        if (countNonZero(overlappingMask) == 0){
            // If new and old foreground do not overlap then consolidate
            // both foreground for creating collage.
            frame.copyTo(output, cleanFgMask);
            // Coverting 1 channel image into 3 channel image for
            // stitching image.
            vector<Mat> fakeChannels;
            for(int channel_index=0; channel_index<3; channel_index++)
                fakeChannels.push_back(cleanFgMask);
            //Merging image channels
            merge(fakeChannels, fake3CfgMaskMOG2);
            fake3CfgMaskMOG2.copyTo(combined(Rect(0,0, \
                            imageSize.width, imageSize.height)));
            //Updating collage.
            output.copyTo(combined(Rect(imageSize.width,0, imageSize.width,
                    imageSize.height)));
            //Show collage consolidation process.
            imshow("Process", combined);
            keyboard = (char)waitKey(WAIT);
        }
        bitwise_or(previousMask, cleanFgMask, previousMask);
    }
    //Writing final collage
    imwrite("./HW03_Sharma_Deepak_PartA_OUTPUT.jpg", output);
    //delete capture object
    capture.release();
}

/*
* @brief  clean foreground components by performing morphological and
* geometrical filtering.
* @param  fgMaskMOG2, foreground mask.
* @param  cleanFgMask, foreground mask containing only object of interest.
* Code Help/source: http://answers.opencv.org/\
* question/120698/drawning-labeling-components-in-a-image-opencv-c/
*/
void processForeground(Mat &fgMaskMOG2, Mat &cleanFgMask){
    Mat labelImage(fgMaskMOG2.size(), CV_32S);
    Mat stats, centroids, binaryImage;
    //Performed labeling on foreground image for generating connected
    //components.
    int nLabels = connectedComponentsWithStats(fgMaskMOG2, labelImage,
            stats, centroids, 8, CV_32S);
    std::vector<int> labels_finals;
    int largestLabel = 0;
    int maxArea = 0;
    int area = 0;
    float HWratio = 0;
    for (int label = 1; label < nLabels; ++label){
        area = stats.at<int>(label, CC_STAT_AREA) ;
        float HWratio = 1.0*stats.at<int>(label,\
                CC_STAT_HEIGHT)/stats.at<int>(label, CC_STAT_WIDTH);
        //float WHratio = 1.0/HWratio;
        //Performed geometrical filtering, removed connected components with
        //very small area, very large area, unwanted aspact ratio.
        //maintained the largest Connected components after geo metric
        //filtering.
        if (area > MIN_AREA  && area < MAX_AREA && area > maxArea &&\
                HWratio > ASPACT_RATIO ){
            largestLabel = label;
            maxArea = area;}}
    for (int rowCounter=0; rowCounter<labelImage.rows; rowCounter++){
        for(int colCounter=0; colCounter<labelImage.cols; colCounter++){
            int label = labelImage.at<int>(rowCounter, colCounter);
                if(label != largestLabel)
                    labelImage.at<int>(rowCounter, colCounter) = 0;
        }
    }
    labelImage.convertTo(labelImage, CV_8UC1);
    threshold(labelImage, binaryImage, 0, 1, 0);
    Mat element = getStructuringElement( MORPH_RECT,
            Size(2*MORPH_SIZE + 1, 2*MORPH_SIZE+1),
            Point(MORPH_SIZE, MORPH_SIZE));
    //Performed morphological closing operation for filling completly the
    //traget connected component
    morphologyEx(binaryImage, cleanFgMask, MORPH_CLOSE, element);
    cleanFgMask = 255*cleanFgMask;

}
