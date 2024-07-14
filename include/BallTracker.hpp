#ifndef BALLTRACKER_HPP
#define BALLTRACKER_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include "TableDetector.hpp"
#include "BallDetector.hpp"

class BallTracker {
    public:
    std::vector<cv::Rect> boundingBoxes;

    std::vector<cv::Rect> expandBoundingBoxes(std::vector<cv::Rect> inputBoxes);

    void trackBalls(cv::VideoCapture video);
   
};



#endif