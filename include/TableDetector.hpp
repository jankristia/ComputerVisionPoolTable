#ifndef TABLEDETECTOR_HPP
#define TABLEDETECTOR_HPP

#include <opencv2/opencv.hpp>

class TableDetector {
    public:
        std::vector<cv::Vec2f> detectedLines;
        // make vector of circles
        std::vector<cv::Vec3f> detectedBalls;
        cv::Mat roiTable;
        
        void setTableLines(cv::Mat frame);
        void setRoiTable(cv::Mat frame);
};

#endif
