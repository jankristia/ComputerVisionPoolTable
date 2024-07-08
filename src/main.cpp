#include <opencv2/opencv.hpp>
#include <iostream>

#include "../include/TableDetector.hpp"
#include "../include/BallDetector.hpp"

int main(int argc, char** argv) {

    if(argc != 2) {
        std::cout << "Usage: " << argv[0] << " <imageDirectory path> " << std::endl;
        return -1;
    }

    std::string imageDirectory = argv[1];

    // Create a VideoCapture object and open the input file
    // If the input is the web camera, pass 0 instead of the video file name
    cv::VideoCapture cap(imageDirectory); 

    // Check if camera opened successfully
    if(!cap.isOpened()){
        std::cout << "Error opening video stream or file" << std::endl;
        return -1;
    }

    // Capture first frame
    cv::Mat frame;
    cap >> frame;

    // Detect the lines of the pool table in first frame
    TableDetector tableDetector;
    tableDetector.setTableLines(frame);
    std::vector<cv::Vec2f> detectedLines = tableDetector.detectedLines;
    tableDetector.setRoiTable(frame);
    
    // BallDetector ballDetector;
    // ballDetector.detectBalls(frame);



   
   
   
   
   

   
   
   
   
   
   
   
   
    // while(1){

    // cv::Mat frame;
    
    // // Capture frame-by-frame
    // cap >> frame;

    // // If the frame is empty, break immediately
    // if (frame.empty()) {
    //     break;
    // }
    // // Display the resulting frame
    // cv::imshow( "Frame", frame );

    // // Press  ESC on keyboard to exit
    // char c=(char)cv::waitKey(25);
    // if(c==27)
    //     break;
    // }

    // // When everything done, release the video capture object
    // cap.release();

    // // Closes all the frames
    // cv::destroyAllWindows();

    return 0;
  }