#include <opencv2/opencv.hpp>
#include <iostream>

#include "../include/TableDetector.hpp"
#include "../include/BallDetector.hpp"

void printBGR(int event, int x, int y, int flags, void* userdata);
void printMeanAroundClick(int event, int x, int y, int flags, void* userdata);

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
    // ballDetector.setTableColor(tableDetector.roiTable);
    // ballDetector.segmentBallColors(tableDetector.roiTable);



    cv::waitKey(0);




// --------------------- TESTING ---------------------
    // Convert frame to HSV
    cv::Mat hsvImage;
    cv::cvtColor(frame, hsvImage, cv::COLOR_BGR2HSV);

    cv::namedWindow("Normal image", cv::WINDOW_AUTOSIZE); 
	cv::imshow("Normal image", frame);

    cv::namedWindow("Click image", cv::WINDOW_AUTOSIZE); 
	cv::imshow("Click image", hsvImage);

    cv::setMouseCallback("Click image", printMeanAroundClick, &hsvImage);
   
    cv::waitKey(0);
   
   
   

   
   
   
   



   
   
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

    void printBGR(int event, int x, int y, int flags, void* userdata) {
    if(event == cv::EVENT_LBUTTONDOWN) {
        cv::Mat* image = static_cast<cv::Mat*>(userdata);

        if (image != nullptr) {
            cv::Vec3b pixel = image->at<cv::Vec3b>(y,x);

            std::cout << "Press detected at: x = " << x << " and y = " << y << "\n";
            std::cout << "HSV values at pixel: \n";
            std::cout << "H: " << (int)pixel[0] << "\n";
            std::cout << "S: " << (int)pixel[1] << "\n";
            std::cout << "V: " << (int)pixel[2] << "\n";
        } else {
            std::cout << "Something wrong with the image pointer... \n";
        }

    }
}


void printMeanAroundClick(int event, int x, int y, int flags, void* userdata) {
        if(event == cv::EVENT_LBUTTONDOWN) {
        cv::Mat* image = static_cast<cv::Mat*>(userdata);

        if (image != nullptr) {
            cv::Vec3b pixel = image->at<cv::Vec3b>(y,x);
            
            int rows = image->rows;
            int cols = image->cols;
            
            int gridSize = 7;
            int halfGridSize = gridSize/2;

            int sumB = 0;
            int sumG = 0;
            int sumR = 0;

            int meanB = 0;
            int meanG = 0;
            int meanR = 0;

            int validNeighbours = 0;

            for (int row = y - halfGridSize; row <= y + halfGridSize; row++) {
                for(int col = x - halfGridSize; col <= x +halfGridSize; col++) {
                    if( row  >= 0 && row < rows && col >= 0 && col < cols) {
                        sumB += (int)image->at<cv::Vec3b>(row,col) [0];
                        sumG += (int)image->at<cv::Vec3b>(row,col) [1];
                        sumR += (int)image->at<cv::Vec3b>(row,col) [2];

                        ++validNeighbours;
                    }
                }
            }

            meanB = sumB/validNeighbours;
            meanG = sumG/validNeighbours;
            meanR = sumR/validNeighbours;

            std::cout << "Press detected at: x = " << x << " and y = " << y << "\n";
            std::cout << "Mean BGR values around pixel: \n";
            std::cout << "B: " << meanB << "\n";
            std::cout << "G: " << meanG << "\n";
            std::cout << "R: " << meanR << "\n";
        } else {
            std::cout << "Something wrong with the image pointer... \n";
        }

    }
}