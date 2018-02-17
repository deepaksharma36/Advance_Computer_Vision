/*
* @file     HW05_Sharma_Deepak_PartA.cpp
* @author  Sharma, Deepak (ds5930@g.rit.edu),
* taka-no-me,Valery Tyumen,
* Steven Puttemans, Adil Ibragimo
*
* @date    7/24/2017
* @version 1.0
*
* @brief CSCI 731, Home Work 5 Part A,
* 1. Generate Camera Parameters
*
* @section DESCRIPTION
*
* This program perform camera calibration and generate a yml file
* containing Camera parameters: intrensic, extrensic, distortion.
* Use command line arguments to specify relative/absolute path of the
* dir containing Checker Board Images:
* CMD:
* ./HW05_Sharma_Deepak+PartA <Path to Dir Containing Checker Board Images>
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


//Calculate Checker Board Corners and stores in corners vector
static void calcChessboardCorners(Size boardSize, float squareSize,
        vector<Point3f>& corners)
{
        corners.resize(0);

        for( int row = 0; row < boardSize.height; row++ )
            for( int col = 0; col < boardSize.width; col++ )
                corners.push_back(Point3f(float(col*squareSize),
                                          float(row*squareSize), 0));
}

//Used for calulating reprojection error
static double computeReprojectionErrors(
        const vector<vector<Point3f> >& objectPoints,
        const vector<vector<Point2f> >& imagePoints,
        const vector<Mat>& rvecs, const vector<Mat>& tvecs,
        const Mat& cameraMatrix, const Mat& distCoeffs,
        vector<float>& perViewErrors )
{
    vector<Point2f> imagePoints2;
    int i, totalPoints = 0;
    double totalErr = 0, err;
    perViewErrors.resize(objectPoints.size());

    for( i = 0; i < (int)objectPoints.size(); i++ )
    {
       projectPoints(Mat(objectPoints[i]), rvecs[i], tvecs[i],
                      cameraMatrix, distCoeffs, imagePoints2);
        err = norm(Mat(imagePoints[i]), Mat(imagePoints2), NORM_L2);
        int n = (int)objectPoints[i].size();
        perViewErrors[i] = (float)std::sqrt(err*err/n);
        totalErr += err*err;
        totalPoints += n;
    }

    return std::sqrt(totalErr/totalPoints);
}

//Run camera Calibaration and set camera parameter values
static bool runCalibration( vector<vector<Point2f> > imagePoints,
                    Size imageSize, Size boardSize,
                    float squareSize, float aspectRatio,
                    int flags, Mat& cameraMatrix, Mat& distCoeffs,
                    vector<Mat>& rvecs, vector<Mat>& tvecs,
                    vector<float>& reprojErrs,
                    double& totalAvgErr)
{
    cameraMatrix = Mat::eye(3, 3, CV_64F);
    if( flags & CALIB_FIX_ASPECT_RATIO )
        cameraMatrix.at<double>(0,0) = aspectRatio;

    distCoeffs = Mat::zeros(8, 1, CV_64F);

    vector<vector<Point3f> > objectPoints(1);
    calcChessboardCorners(boardSize, squareSize, objectPoints[0]);

    objectPoints.resize(imagePoints.size(),objectPoints[0]);

    double rms = calibrateCamera(objectPoints,
            imagePoints, imageSize,
            cameraMatrix, distCoeffs,
            rvecs, tvecs, flags|CALIB_FIX_K4|CALIB_FIX_K5);
                    ///*|CALIB_FIX_K3*/|CALIB_FIX_K4|CALIB_FIX_K5);
    printf("RMS error reported by calibrateCamera: %g\n", rms);

    bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);

    totalAvgErr = computeReprojectionErrors(objectPoints, imagePoints,
                rvecs, tvecs, cameraMatrix, distCoeffs, reprojErrs);

    return ok;
}


//Save camera Parameters to a yml file
static void saveCameraParams( const string& filename,
                       Size imageSize, Size boardSize,
                       float squareSize, float aspectRatio, int flags,
                       const Mat& cameraMatrix, const Mat& distCoeffs,
                       const vector<Mat>& rvecs, const vector<Mat>& tvecs,
                       const vector<float>& reprojErrs,
                       const vector<vector<Point2f> >& imagePoints,
                       double totalAvgErr )
{
    FileStorage fs( filename, FileStorage::WRITE );

    time_t tt;
    time( &tt );
    struct tm *t2 = localtime( &tt );
    char buf[1024];
    strftime( buf, sizeof(buf)-1, "%c", t2 );

    fs << "calibration_time" << buf;

    if( !rvecs.empty() || !reprojErrs.empty() )
        fs << "nframes" << (int)std::max(rvecs.size(), reprojErrs.size());
    fs << "image_width" << imageSize.width;
    fs << "image_height" << imageSize.height;
    fs << "board_width" << boardSize.width;
    fs << "board_height" << boardSize.height;
    fs << "square_size" << squareSize;

    if( flags & CALIB_FIX_ASPECT_RATIO )
        fs << "aspectRatio" << aspectRatio;

    if( flags != 0 )
    {
        sprintf( buf, "flags: %s%s%s%s",
            flags & CALIB_USE_INTRINSIC_GUESS ? "+use_intrinsic_guess" : "",
            flags & CALIB_FIX_ASPECT_RATIO ? "+fix_aspectRatio" : "",
            flags & CALIB_FIX_PRINCIPAL_POINT ? "+fix_principal_point" : "",
            flags & CALIB_ZERO_TANGENT_DIST ? "+zero_tangent_dist" : "" );
        //cvWriteComment( *fs, buf, 0 );
    }

    fs << "flags" << flags;

    fs << "intrinsic_matrix" << cameraMatrix;
    fs << "distortion_coefficients" << distCoeffs;

    fs << "avg_reprojection_error" << totalAvgErr;
    if( !reprojErrs.empty() )
        fs << "per_view_reprojection_errors" << Mat(reprojErrs);

    if( !rvecs.empty() && !tvecs.empty() )
    {
        CV_Assert(rvecs[0].type() == tvecs[0].type());
        Mat bigmat((int)rvecs.size(), 6, rvecs[0].type());
        for( int i = 0; i < (int)rvecs.size(); i++ )
        {
            Mat r = bigmat(Range(i, i+1), Range(0,3));
            Mat t = bigmat(Range(i, i+1), Range(3,6));

            CV_Assert(rvecs[i].rows == 3 && rvecs[i].cols == 1);
            CV_Assert(tvecs[i].rows == 3 && tvecs[i].cols == 1);
            //*.t() is MatExpr (not Mat) so we can use assignment operator
            r = rvecs[i].t();
            t = tvecs[i].t();
        }
        //cvWriteComment( *fs, "a set of 6-tuples (rotation vector + translation vector) for each view", 0 );
        fs << "extrinsic_parameters" << bigmat;
    }

}

/*static bool readStringList( const string& filename, vector<string>& l )
{
    l.resize(0);
    FileStorage fs(filename, FileStorage::READ);
    if( !fs.isOpened() )
        return false;
    FileNode n = fs.getFirstTopLevelNode();
    if( n.type() != FileNode::SEQ )
        return false;
    FileNodeIterator it = n.begin(), it_end = n.end();
    for( ; it != it_end; ++it )
        l.push_back((string)*it);
    return true;
}*/


//Invoke functinality for camera calibration and save rendered parameters.
static bool runAndSave(const string& outputFilename,
                const vector<vector<Point2f> >& imagePoints,
                Size imageSize, Size boardSize, float squareSize,
                float aspectRatio, int flags, Mat& cameraMatrix,
                Mat& distCoeffs, bool writeExtrinsics, bool writePoints )
{
    cout<<"Saving Parameters Now"<<endl;
    vector<Mat> rvecs, tvecs;
    vector<float> reprojErrs;
    double totalAvgErr = 0;

    bool ok = runCalibration(imagePoints, imageSize, boardSize, squareSize,
                   aspectRatio, flags, cameraMatrix, distCoeffs,
                   rvecs, tvecs, reprojErrs, totalAvgErr);
    printf("%s. avg reprojection error = %.2f\n",
           ok ? "Calibration succeeded" : "Calibration failed",
           totalAvgErr);

    if( ok )
        saveCameraParams( outputFilename, imageSize,
                         boardSize, squareSize, aspectRatio,
                         flags, cameraMatrix, distCoeffs,
                         writeExtrinsics ? rvecs : vector<Mat>(),
                         writeExtrinsics ? tvecs : vector<Mat>(),
                         writeExtrinsics ? reprojErrs : vector<float>(),
                         writePoints ? imagePoints : vector<vector<Point2f> >(),
                         totalAvgErr );
    return ok;
}

//Main method
int main( int argc, char** argv )
{
    cout<<"Cmd for run is: ./HW05_Sharma_Deepak_PartA <Path to directory containing checkerboard images>"<<endl;
    Size boardSize, imageSize;
    float squareSize, aspectRatio;
    Mat cameraMatrix, distCoeffs; // Storing camera parameters
    string outputFilename; //File containing Parameters

    int checkerBoardImageCounter, nframes=10;
    bool writeExtrinsics, writePoints;
    bool undistortImage = false;
    int flags = 0;
    clock_t prevTimestamp = 0;
    int cameraId = 0;
    vector<vector<Point2f> > imagePoints;
    vector<String> imageList;
    const string imageDir = argv[1];

    boardSize.width = 8; //Number of inner corner in X direction
    boardSize.height = 6; //Number of inner corner in Y direction
    squareSize = 1; //Size of the suare
    aspectRatio = 1;
    writePoints = true;
    writeExtrinsics = true;
    //if (parser.has("a"))
    //    flags |= CALIB_FIX_ASPECT_RATIO;
    //if (parser.has("zt"))
    flags |= CALIB_ZERO_TANGENT_DIST;
    //if (parser.has("p"))
    flags |= CALIB_FIX_PRINCIPAL_POINT;
    //if (parser.has("o"))
    outputFilename = "HW05_Sharma_Deepak_Camera_Parameters.yml";
    // Name of the file containing Camera Parameters
    if ( squareSize <= 0 )
        return fprintf( stderr, "Invalid board square width\n" ), -1;
    if ( aspectRatio <= 0 )
        return printf( "Invalid aspect ratio\n" ), -1;
    if ( boardSize.width <= 0 )
        return fprintf( stderr, "Invalid board width\n" ), -1;
    if ( boardSize.height <= 0 )
        return fprintf( stderr, "Invalid board height\n" ), -1;

    if(argc > 1 )
    {
        glob(imageDir, imageList);
    }
    else

    if( imageList.empty() )
        return fprintf( stderr, "Didn't find input directory (%s) ",argv[1]), -2;

    if( !imageList.empty() )
        if ((int)imageList.size() < nframes)
            nframes=(int)imageList.size();

    if ( nframes <= 3 )
        return printf("Invalid number of images\n" ), -1;

    namedWindow( "Image View",WINDOW_NORMAL );

    for(checkerBoardImageCounter = 0;;checkerBoardImageCounter++)
    {
        Mat view, viewGray;

        if( checkerBoardImageCounter < (int)imageList.size() ){
            view = imread(imageList[checkerBoardImageCounter], 1);
        cout<<"If execution stuck, then try again after removing processing image from the folder"<<endl;
        cout<<"Processing"<<imageList[checkerBoardImageCounter]<<endl;

        }

        if(view.empty())
        {
            if( imagePoints.size() > 0 )
                runAndSave(outputFilename, imagePoints, imageSize,
                           boardSize, squareSize, aspectRatio,
                           flags, cameraMatrix, distCoeffs,
                           writeExtrinsics, writePoints);
            break;
        }

        imageSize = view.size();
        cout<<imageSize<<endl;


        vector<Point2f> pointbuf;
        cvtColor(view, viewGray, COLOR_BGR2GRAY);

        bool found;
        found = findChessboardCorners( view, boardSize, pointbuf,
            CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);


        if( found)
        {
            // improve the found corners' coordinate accuracy
            cornerSubPix( viewGray, pointbuf, Size(11,11),
            Size(-1,-1), TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 30, 0.1 ));
            imagePoints.push_back(pointbuf);

            drawChessboardCorners( view, boardSize, Mat(pointbuf), found );}

        int baseLine = 0;



        imageSize = view.size();
        imshow("Image View", view);
        waitKey(1);




        if( imagePoints.size() >= (unsigned)nframes )
        {
            runAndSave(outputFilename, imagePoints, imageSize,
                       boardSize, squareSize, aspectRatio,
                       flags, cameraMatrix, distCoeffs,
                       writeExtrinsics, writePoints);
        }
    }

    //if( dwshowUndistorted )

    return 0;
}
