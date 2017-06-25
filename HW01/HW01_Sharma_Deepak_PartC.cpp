/*
* @file     HW01_Sharma_Deepak_PartC.cpp
* @author  Sharma, Deepak (ds5930@g.rit.edu)
* @date    6/16/2017
* @version 1.0
*
* @brief CSCI 731, Home Work 1, Part C
* Show color channels and Bit Plans for each channel for an input image.
*
* @section DESCRIPTION
*
* This program read input image, show color channels and bit plans for
* finding the hidden message.
*
* Use command line arguments to specify relative/absulute path of the input
* image.
* Program will exit in case of missing or invalid image path.
* C++ documentation has been learned from:
* http://csweb.cs.wfu.edu/~fulp/CSC112/codeStyle.html
*
*/

#include "opencv2/highgui/highgui.hpp"		// For OpenCV Windowing and display commands.
#include "opencv2/videoio.hpp"
#include "iostream" 				// For C++ IO
#include "stdexcept"                // For raising exceptions
#include <fstream>                  // For aserting file
#include <glob.h>
using namespace std;
using namespace cv;


const int DEFAULT_N_CHANNELS = 3; // Global Constant
const int DEFAULT_N_BIT_PLANS = 8; // Global Constant

/*
* @brief  Show an image stored in secondary memory locatation.
* @param  image object of Mat class containing image matrix
* @param  window_name object of namedWindow class, provide frame for showing
* the image.
* @return void
*/
void showImage(Mat &image, string &window_name)
    {
    namedWindow(window_name, WINDOW_AUTOSIZE);  // For showing image creating an image holder window
    imshow( window_name, image);                // Showing image in the image holder window
    waitKey(0);                                 // Waiting until user does't hit any key
    destroyWindow(window_name);                 // Destorying image holder window
    }

/*
* @brief  Return bit value stored at specific location in given byte variable.
* @param  byte, input byte of type unsigned char
* @param  location, an intiger[1-7] variable specify location of the required bit,
* @return An unsigned char value of bit 0 or 1
* Code Source/help: http://www.cplusplus.com/forum/general/97378/
*/
unsigned char getBit(unsigned char byte, int position)
{
        // Performing right shift and logical and operation with mask
        // for accessing required bit
        bool bitBool = (byte >> position) & 0x1;
        // Converting bool type varibale to unsinged char tyep
        unsigned char bit = bitBool?1:0;
        return bit;
}

/*
* @brief  Extract Image channels from the input image.
* @param  image, an Mat object containing data of input image
* @return An vector of Mat objects, containing channels(BGR) of input image.
* Code source/Help: http://answers.opencv.org/question/59529/how-do-i-/
* separate-the-channels-of-an-rgb-image-and-save-each-one-using-\
* the-249-version-of-opencv/
*/
vector<Mat> getImageChannels(Mat &image)
    {
    //Mat image = imread(imagePath);
    int nChannels = image.channels(); //For storing number of channels
    int nRows = image.rows;           //For storing number of rows
    //int nColsOneChannel = image.cols;
    int nCols = image.cols;           //For storing number of cols
    vector<Mat> channels;             // For storing channels
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
        return channels;
    }

/*
* @brief  Extract Bit plans from the input image.
* @param  channels, An vector of Mat objects, containing channels(BGR)
* @return An vector of Mat objects, containing 8 bit plans for each channel
* Coding help/source: https://stackoverflow.com/questions/8480640/\
* how-to-throw-a-c-exception
*/
vector<Mat> getBitPlans(vector<Mat> &channels){
    int nChannels = channels.size();           //Acessing number of channels
    if (nChannels < 0)
        // Throwing invalide input error if no channel image received
        throw invalid_argument("received no channel");
    int nBitPlans = DEFAULT_N_BIT_PLANS*nChannels; //For number of bit plans
    int unsignedCharLimit = 255;                   // For storing highest possible pixel value
    int nRows = channels[0].rows;                  // Accessing number of row in an input channel
    int nCols = channels[0].cols;                  // Accessing number of col in an input channel
    int channelCounter = 0;                        // Store channel couter
    unsigned char  bitVal;
    vector<Mat> bitPlans;                          // For storing bit plans
    for(int bitPlanCounter=0; bitPlanCounter < nBitPlans; bitPlanCounter++){
        //Intialzing bit plans matrixies with 0 value
        bitPlans.push_back(Mat(nRows, nCols, CV_8UC1, Scalar(0)));}

    // Iterating over pixel locations for accessing intensity of channels
    for (int rowCounter=0; rowCounter<nRows; rowCounter++){
        for(int colCounter=0; colCounter<nCols; colCounter++){
            for(int bitPlanCounter=0; bitPlanCounter < nBitPlans;\
                    bitPlanCounter++){
                channelCounter = bitPlanCounter/DEFAULT_N_BIT_PLANS;
                // Acessing intensity of channel at specific location
                Scalar val = channels[channelCounter].at<uchar>(rowCounter, \
                        colCounter);
                // Acessing bit value at specific bit location from the
                // intensity value corresponding to specific spatial location.
                bitVal = getBit(val[0], bitPlanCounter - \
                        DEFAULT_N_BIT_PLANS*channelCounter);
                // Assigning the bit value to bit plan image
                bitPlans[bitPlanCounter].at<uchar>(rowCounter, colCounter) =\
                    unsignedCharLimit*bitVal;}
        }
    }
    return bitPlans;}


/*
* @brief  Check validity of input file path.
* @param  file Path, string containg file path.
* @return boolen True or False
* code source/help: https://stackoverflow.com/questions/12774207/\
* fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
*/

bool is_file_exist(const char *filePath)
    //https://stackoverflow.com/questions/12774207/fastest-way-to-check
    //-if-a-file-exist-using-standard-c-c11-c
    {
        ifstream ifStreamObj(filePath);
        return ifStreamObj.good(); //Check source of input stream is okay

    }

/*
* @brief  Main method, invoke functionalities for showing input image,
* image channels, channels bit plans and input video.
* @param  argc, an integer variable containing number of arguments.
* @param  argv, an char array containing command line arguments.
* @return program status, an integer number.
* code source/help: https://stackoverflow.com/questions/36057028/\
* throw-exception-if-\ the-file-does-not-exist-in-constructor-and\
* -try-catch-it-when
*  http://docs.opencv.org/2.4/doc/tutorials/introduction/\
*  load_save_image/load_save_image.html
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
        if (!is_file_exist(argv[1]))
            {cerr<<"Image file Does not exist"<<endl;
            throw invalid_argument("Invalid file path");}
        string inputImage = argv[1];
        Mat image = imread(inputImage);     // Reading/loading image in Mat object
        string imageWindowName = "Input Image";    // For storing frame window name
        // Invoking show image method
        showImage(image, imageWindowName);
        // Getting image channels for the input image
        vector<Mat> channels = getImageChannels(image);
        // Showing Color channels for the input image
        cout<<"Now Showing color channels!!"<<endl;
        int nChannels = channels.size();
        // Storings names of the frame window
        string channelWindowNames[DEFAULT_N_CHANNELS] = {"Blue","Green", "Red"};
        int channelCounter;                         // For storing channel index
        for (channelCounter=0; channelCounter<nChannels; channelCounter++){
            // Invoking show image for each color channel
            showImage(channels[channelCounter],
                    channelWindowNames[channelCounter]);
            imwrite(inputImage+"_"+channelWindowNames[channelCounter]+".jpg",
                    channels[channelCounter]);
        }
        // Showing Bit Plans
        cout<<"Now Showing bit plan!!"<<endl;
        // Storing names of the bit plans
        string bitWindowNames[DEFAULT_N_BIT_PLANS] = \
        {"LeastSignificant","2", "3", "4", "5", "6", "7", "MostSignificant"};
        // Getting Bit plans
        vector<Mat> bitPlans = getBitPlans(channels);
        int nBitPlans = bitPlans.size();            //For storing number of bit plans
        string bitPlanWindowName;                   // For storing bit plan window name
        for(int bitPlanCounter=0; bitPlanCounter < nBitPlans; bitPlanCounter++){
            channelCounter =  bitPlanCounter/DEFAULT_N_BIT_PLANS;
            string bitPlanWindowName = bitWindowNames\
                                    [bitPlanCounter%DEFAULT_N_BIT_PLANS]+'_'+\
                                    channelWindowNames[channelCounter];
            //Invoking show image for each Bit plan
            showImage(bitPlans[bitPlanCounter], bitPlanWindowName);
            imwrite(inputImage+"_"+bitPlanWindowName+".jpg", bitPlans[bitPlanCounter]);

        }
    }
    catch(exception &ex){
        cerr<<"Open CV Error:\n "<<ex.what()<<endl;
        status = 1;}
    return status;
}
