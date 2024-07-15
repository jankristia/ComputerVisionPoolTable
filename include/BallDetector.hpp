#ifndef BALLDETECTOR_HPP
#define BALLDETECTOR_HPP

#include <opencv2/opencv.hpp>
#include <map>
#include "ColorEnum.hpp"

class BallDetector {
    public:
        BallDetector();
        std::vector<cv::Vec3f> detectedBalls;
        cv::Vec3f whiteBall;
        cv::Vec3f blackBall;
        std::vector<cv::Vec3f> stripedBalls;
        std::vector<cv::Vec3f> solidBalls;
        std::vector<cv::Vec2f> tableBorders;
        cv::Vec3f tableColor;
        std::map<Color, int> colorToHueMap;
        std::vector<cv::Rect> boundingBoxes; 
        
        // Run this with detected table frame as input to do whole ball detection
        void detectBalls(cv::Mat frame);

        void setTableColor(cv::Mat frame);
        bool isInRange(cv::Vec3f testColor, cv::Vec3f refrenceColor, int threshold, int indexToCheck = 0);
        void setBoundingBoxes();




        void detectWhiteBall(cv::Mat frame);
        void detectBlackBall(cv::Mat frame);
        void detectStripedBalls(cv::Mat frame);
        void detectSolidBalls(cv::Mat frame);

};

#endif