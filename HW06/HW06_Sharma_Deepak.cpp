/*
* @file     HW06_Sharma_Deepak_PartA.cpp
* @author  Sharma, Deepak (ds5930@g.rit.edu), SATYA MALLICK
*
* @date    8/8/2017
* @version 1.0
*
* @brief CSCI 731, Home Work 6,
* 1. Tracking algorithms demo.
*
* @section DESCRIPTION
*
* This program run BOOSTING, KCF, TLD, MEDIANFLOW tracking algorithm
* implemented in OpenCV contrib lib.
* Use command line arguments to specify relative/absolute path of the video
* Program will exit, in case of missing or invalid video path
* argument C++ documentation has been learned from:
* http://csweb.cs.wfu.edu/~fulp/CSC112/codeStyle.html
* IMPORTANT MESSAGE: Many parts of the code is based on the demo code provided
* in tutorial:
* https://www.learnopencv.com/object-tracking-using-opencv-cpp-python/
*/
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
//#include "opencv2/tracker.hpp"
using namespace cv;
using namespace std;

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
* @brief  Main method,
* Ask user for selecting target object
* Intialize Tracking algorithms
* Peforming tracking and show Outputs
* @param  argc, an integer variable containing number of arguments.
* @param  argv, an char array containing command line arguments.
* @return program status, an integer number[0, 1].
* code source/Help: https://stackoverflow.com/questions/36057028/throw-exception-if-
* https://stackoverflow.com/questions/13133055/opencv-displaying-2-images-adjacently-in-the-same-window
* http://docs.opencv.org/3.0-beta/modules/imgproc/doc/drawing_functions.html#puttext
*/
int main(int argc, char **argv)
{
    // Seting up tracker for  BOOSTING, KCF, TLD, MEDIANFLOW

    cout<<"Provide first argument as a path video"<<endl;
    cout<<"Select a patch/target using mouse for tracking and press Enter"<<endl;
    if(argc < 2)
    {
        cout<<"Missing argument provide video path"<<endl;
        return 1;

    }
    const char* videoPath = argv[1];
    if (!is_file_exist(videoPath)){
        cout<<"File does't exist, invalid path"<<endl;
        return 1;}
    //Creating Trackers object
    Ptr<Tracker> trackerMIL = Tracker::create("MIL");
    Ptr<Tracker> trackerTLD = Tracker::create("TLD");
    Ptr<Tracker> trackerKCF = Tracker::create("KCF");
    Ptr<Tracker> trackerBOOSTING = Tracker::create("BOOSTING");
    Ptr<Tracker> trackerMEDIANFLOW = Tracker::create("MEDIANFLOW");
    // Read video
    VideoCapture video(argv[1]);
    // Check video is open or not
    if(!video.isOpened())
    {
        cout << "Could not read video file" << endl;
        return 1;
    }
    // Read first frame for taking user input.
    Mat frame;
    namedWindow("tracker", WINDOW_NORMAL);
    int fontFace = 1;//FONT_HERSHEY_SCRIPT_PLAIN;
    double fontScale = 10;
    int thickness = 15;
    int offset = 250;
    video.read(frame);
    // Define an initial bounding box
    Rect2d bbox;
    Point textmessage(100, 100);
    putText(frame, "Select a patch using mouse then press Enter", textmessage, 1, 5,
                    Scalar(0,0,255), 3, 8);
    bbox = selectROI("tracker",frame);
    Rect2d bboxMIL(bbox), bboxTLD(bbox), bboxBOOSTING(bbox), bboxKCF(bbox), bboxMEDIANFLOW(bbox);
    //rect2d bbox(287, 23, 86, 320);


    // Initialize tracker with first frame and bounding box
    trackerMIL->init(frame, bboxMIL);
    trackerTLD->init(frame, bboxTLD);
    trackerBOOSTING->init(frame, bboxBOOSTING);
    trackerKCF->init(frame, bboxKCF);
    trackerMEDIANFLOW->init(frame, bboxMEDIANFLOW);
    Mat frameTLD, frameMIL, frameBOOSTING, frameKCF, frameMEDIANFLOW;
    Size imageSize = frame.size(); //Mesuring size of image frames.
    //Creating image for holding output progress.
    Mat combined = Mat(2*imageSize.height, 3*imageSize.width, CV_8UC3);
    namedWindow("trackerResults", WINDOW_NORMAL);
    while(video.read(frame))
    {
        //Coping frames into duplicate frames which will show output for each
        //tracker
        frame.copyTo(frameTLD);
        frame.copyTo(frameMIL);
        frame.copyTo(frameKCF);
        frame.copyTo(frameBOOSTING);
        frame.copyTo(frameMEDIANFLOW);
        //Tracker updating the location of path/object in next frame
        trackerMIL->update(frameMIL, bboxMIL);
        trackerTLD->update(frameTLD, bboxTLD);
        trackerKCF->update(frameKCF, bboxKCF);
        trackerBOOSTING->update(frameBOOSTING, bboxBOOSTING);
        trackerMEDIANFLOW->update(frameMEDIANFLOW, bboxMEDIANFLOW);

        // Draw bounding box on the frame for showing updated location of
        // traget object
        rectangle(frameTLD, bboxMIL, Scalar( 255, 0, 0 ), 2, 1 );
        rectangle(frameMIL, bboxTLD, Scalar( 255, 0, 0 ), 2, 1 );
        rectangle(frameKCF, bboxKCF, Scalar( 255, 0, 0 ), 2, 1 );
        rectangle(frameBOOSTING, bboxBOOSTING, Scalar( 255, 0, 0 ), 2, 1 );
        rectangle(frameMEDIANFLOW, bboxMEDIANFLOW, Scalar( 255, 0, 0 ), 2, 1 );

        // Display results on a combined image by stiching frames together
            frame.copyTo(combined(Rect(0,0, \
                            imageSize.width, imageSize.height)));
            //Updating collage.
            frameTLD.copyTo(combined(Rect(imageSize.width, 0, imageSize.width,
                    imageSize.height)));
            frameKCF.copyTo(combined(Rect(2*imageSize.width, 0, imageSize.width,
                    imageSize.height)));
            frameBOOSTING.copyTo(combined(Rect(0,imageSize.height, imageSize.width,
                    imageSize.height)));
            frameMEDIANFLOW.copyTo(combined(Rect(1*imageSize.width,imageSize.height, imageSize.width,
                    imageSize.height)));
            frameMIL.copyTo(combined(Rect(2*imageSize.width,imageSize.height, imageSize.width,
                    imageSize.height)));
            Point textTLD(imageSize.width+offset, 0+offset);
            Point textKCF(2*imageSize.width+offset, 0+offset);
            Point textBOOSTING(offset, imageSize.height+offset);
            Point textMEDIANFLOW(1*imageSize.width+offset, imageSize.height+offset);
            Point textMIL(2*imageSize.width+offset, imageSize.height+offset);
            // Placing tracker names on each frame for user
            putText(combined, "TLD", textTLD, fontFace, fontScale,
                    Scalar(0,0,255), thickness, 8);
            putText(combined, "KCF", textKCF, fontFace, fontScale,
                    Scalar(0,0,255), thickness, 8);
            putText(combined, "BOOSTING", textBOOSTING, fontFace, fontScale,
                    Scalar(0,0,255), thickness, 8);
            putText(combined, "MEDIANFLOW", textMEDIANFLOW, fontFace, fontScale,
                    Scalar(0,0,255), thickness, 8);
            putText(combined, "MIL", textMIL, fontFace, fontScale,
                    Scalar(0,0,255), thickness, 8);
        imshow("trackerResults", combined);
        int key = waitKey(1);
        if(key == 27) break;
    }
    return 0;
}

