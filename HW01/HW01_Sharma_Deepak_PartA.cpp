/*
* @file     HW01_Sharma_Deepak_PartA.cpp
* @author  Sharma, Deepak (ds5930@g.rit.edu)
* @date    6/16/2017
* @version 1.0
*
* @brief CSCI 731, Home Work 1 Part A,
* 1.Show Image
*
* @section DESCRIPTION
*
* This program read input image,
*
* Use command line arguments to specify relative/absolute path of the
* input image. Program will exit, in case of missing or invalid image
* path argument C++ documentation has been learned from:
* http://csweb.cs.wfu.edu/~fulp/CSC112/codeStyle.html
*
*/

#include "opencv2/highgui/highgui.hpp"		// For OpenCV Windowing and display commands.
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
void showImage(const char* &imagePath, string &window_name)
    {
    Mat image = imread(imagePath);     // Reading/loading image in Mat object
    namedWindow(window_name, WINDOW_AUTOSIZE);  // For showing image creating an image holder window
    imshow( window_name, image);                // Showing image in the image holder window
    waitKey(0);                                 // Waiting until user does't hit any key
    destroyWindow(window_name);                 // Destorying image holder window
    }

/*
* @brief  Main method, invoke functionalities for showing input image,
* @param  argc, an integer variable containing number of arguments.
* @param  argv, an char array containing command line arguments.
* @return program status, an integer number[0, 1].
* code source: https://stackoverflow.com/questions/36057028/throw-exception-if-
* the-file-does-not-exist-in-constructor-and-try-catch-it-when
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
        string imageWindowName = "Input Image";    // For storing frame window name
        // Invoking show image method
        showImage(imagePath, imageWindowName);
        return 0;
        }

    catch(exception &ex){
        cerr<<"Bellow error occured:\n "<<ex.what()<<endl;
        cerr<<"Exiting"<<endl;
        return 1;}
}
