
/*
* @file     HW01_Sharma_Deepak_PartB.cpp
* @author  Sharma, Deepak (ds5930@g.rit.edu)
* @date    6/16/2017
* @version 1.0
*
* @brief CSCI 731, Home Work 1, Part B
* 2. Video file
*
* @section DESCRIPTION
*
* Show input video.
*
* Use command line arguments to specify relative/absolute path of the
* input video. Parogram will exit in case of missing or invalid video path.
* C++ documentation has been learned from:
* http://csweb.cs.wfu.edu/~fulp/CSC112/codeStyle.html
*/

#include "opencv2/highgui/highgui.hpp"		// For OpenCV Windowing and display commands.
#include "opencv2/videoio.hpp"
#include "iostream" 				// For C++ IO
#include "stdexcept"                // For raising exceptions
#include <fstream>                  // For aserting file
using namespace std;
using namespace cv;

/*
* @brief  Show a video stored on secondary memory locatation or
* from camera device.
* @param  videoPath, string containing path to input video or
* Integer value specifing camera device ID.
* @return execution status, an Integer [0, 1]
*/
template<typename T>
int showVideo(T videoPath)
    {
        // reading video from input path or from camera device
        char* window_name = (char* )"Input Video"; // Name of video frame
        namedWindow(window_name, WINDOW_AUTOSIZE); // For holding video frame
        try{
            VideoCapture cap = VideoCapture(videoPath);
            Mat frame;                                 // For holding image object
            char keyPressed = -1;                      // For storing user input while showing video
            while(keyPressed < 0)                      //Stop showing if user pressed any key
                {
                cap >> frame;                          // Acessing frame from video iteratively
                if (frame.empty()) break;             // Exit if video capture does not render next frame
                imshow(window_name, frame);            // Showing video frame
                keyPressed = (char )waitKey(33);       // Waiting for 33 milisecond
                }
            destroyWindow(window_name);
           }
        catch(exception &ex){
            cerr<<"Open CV Error:\n "<<ex.what()<<endl;
            destroyWindow(window_name);
            return 1;
            }
        return 0;
    }


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
* @brief  Main method, invoke functionalities for showing input image,
* image channels, channels bit plans and input video.
* @param  argc, an integer variable containing number of arguments.
* @param  argv, an char array containing command line arguments.
* @return program status, an integer number[0. 1].
* https://stackoverflow.com/questions/36057028/throw-exception-if-
* the-file-does-not-exist-in-constructor-and-try-catch-it-when
*/
int main( int argc, char** argv )
{

    if ( argc < 2 )
    // Asserting number of command line arguments provided by user
    {
        cerr << "Error: Missing arguments for image and video path"<<endl;
        return 1; 			// Return an error code.
    }
    // Showing video corresponding to provided video path
    try {
        const char* videoPath = argv[1];
        if (!is_file_exist(videoPath))
            {cerr<<"Video file "<<videoPath<<" Does not exist"<<endl;
            throw invalid_argument("Invalid file path");}
        return showVideo(videoPath);
        }
    catch (exception &ex) {
        cerr<<"Error:\n "<<ex.what()<<endl;
        cerr<<"Exiting"<<endl;
        return 1;
        }
}
