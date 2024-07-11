#ifndef BALLDETECTOR_HPP
#define BALLDETECTOR_HPP

#include <opencv2/opencv.hpp>
#include <map>
#include "ColorEnum.hpp"

class BallDetector {
    public:
        BallDetector();
        std::vector<cv::Vec3f> detectedBalls;
        std::vector<cv::Vec2f> tableBorders;
        cv::Vec3f tableColor;
        std::map<Color, int> colorToHueMap;

        void setTableColor(cv::Mat frame);
        bool isInRange(cv::Vec3f testColor, cv::Vec3f refrenceColor, int threshold);
        bool isPixelColor(cv::Vec3b pixel, cv::Vec3f color, int threshold); // Next to implement

        void segmentBallColors(cv::Mat frame);
};

#endif