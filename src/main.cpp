// Main author: Jan Kristian Alstergren
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
        std::cout << "Usage: " << argv[0] << " <videoDirectory path> " << "<groundTruthBboxesPath>" << std::endl;
        return -1;
    }

    std::string videoDirectory = argv[1];
    std::string groundTruthPath = argv[2];

    cv::VideoCapture video(videoDirectory);

    if(!video.isOpened()){
        std::cout << "Error opening video stream or file" << std::endl;
        return -1;
    }

    // Read the first frame of the video
    cv::Mat frame;
    video.read(frame);

    TableDetector tableDetector;
    cv::Mat detectedTable = tableDetector.detectTable(frame);

    // Show the detected table
    cv::namedWindow("Detected table", cv::WINDOW_AUTOSIZE);
    cv::imshow("Detected table", detectedTable);

    BallDetector ballDetector;
    ballDetector.detectBalls(tableDetector.roiTable);
    ballDetector.detectBlackBall(tableDetector.roiTable);

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
   
    cv::waitKey(0);


   


    return 0;
}