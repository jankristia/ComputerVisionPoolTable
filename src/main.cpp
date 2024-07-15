#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <iostream>

#include "../include/TableDetector.hpp"
#include "../include/BallDetector.hpp"
#include "../include/BallTracker.hpp"
#include "../include/MeanAveragePrecision.hpp"

void printBGR(int event, int x, int y, int flags, void* userdata);
void printMeanAroundClick(int event, int x, int y, int flags, void* userdata);

int main(int argc, char** argv) {

    if(argc != 3) {
        std::cout << "Usage: " << argv[0] << " <videoDirectory path> " << "<groundTruthPath>" << std::endl;
        return -1;
    }

    std::string videoDirectory = argv[1];
    std::string groundTruthPath = argv[2];

    cv::VideoCapture video(videoDirectory);

    if(!video.isOpened()){
        std::cout << "Error opening video stream or file" << std::endl;
        return -1;
    }

    cv::Mat frame;
    video.read(frame);

    TableDetector tableDetector;
    cv::Mat detectedTable = tableDetector.detectTable(frame);

    // Show the detected table
    cv::namedWindow("Detected table", cv::WINDOW_AUTOSIZE);
    cv::imshow("Detected table", detectedTable);

    BallDetector ballDetector;
    ballDetector.detectBalls(tableDetector.roiTable);

    // Draw the bounding boxes on the detected table
    cv::Mat detectedBalls = tableDetector.roiTable.clone();
    for(int i = 0; i < ballDetector.boundingBoxes.size(); i++) {
        cv::rectangle(detectedBalls, ballDetector.boundingBoxes[i], cv::Scalar(0, 255, 0), 2);
    }

    // Show the detected balls
    cv::namedWindow("Detected balls", cv::WINDOW_AUTOSIZE);
    cv::imshow("Detected balls", detectedBalls);

    MeanAveragePrecision map;
    double averagePrecision = map.averagePrecisionCalculation(frame, groundTruthPath);
    std::cout << "Average Precision: " << averagePrecision << std::endl;














// --------------------- TESTING ---------------------
    // Convert frame to HSV
    // cv::Mat hsvImage;
    // cv::cvtColor(frame, hsvImage, cv::COLOR_BGR2HSV);

    // cv::namedWindow("Normal image", cv::WINDOW_AUTOSIZE); 
	// cv::imshow("Normal image", frame);

    // cv::namedWindow("Click image", cv::WINDOW_AUTOSIZE); 
	// cv::imshow("Click image", hsvImage);

    // cv::setMouseCallback("Click image", printBGR, &hsvImage);
   
    cv::waitKey(0);


   


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