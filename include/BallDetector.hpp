// Main author: Jan Kristian Alstergren
#ifndef BALLDETECTOR_HPP
#define BALLDETECTOR_HPP

#include <opencv2/opencv.hpp>
#include <map>
#include "ColorEnum.hpp"
#include "BoundingBox.hpp"

class BallDetector {
    // // To detect balls, run the following code in main.cpp:
    // // tableDetector.roiTable is the detected table in the TableDetector class
    // BallDetector ballDetector;
    // ballDetector.detectBalls(tableDetector.roiTable); 

    public:
        BallDetector();
        std::vector<BoundingBox> segmentedBalls; 
        std::vector<cv::Vec3f> detectedBalls;
        std::vector<cv::Vec2f> tableBorders;
        cv::Vec3f tableColor;
        std::map<Color, int> colorToHueMap;
        
        void detectBalls(cv::Mat frame);

        void setTableColor(cv::Mat frame);
        bool isInRange(cv::Vec3f testColor, cv::Vec3f refrenceColor, int threshold, int indexToCheck = 0);

        int detectWhiteBallIndex(cv::Mat frame);
        int detectBlackBallIndex(cv::Mat frame);
        void segmentBalls(cv::Mat frame);

};

#endif