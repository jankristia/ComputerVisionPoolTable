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

    BallDetector ballDetector;
    ballDetector.segmentBalls(tableDetector.roiTable);

    // MeanAveragePrecision map;
    // double averagePrecision = map.averagePrecisionCalculation(frame, groundTruthPath);
    // std::cout << "Average Precision: " << averagePrecision << std::endl;
   
    cv::waitKey(0);


   


    return 0;
}