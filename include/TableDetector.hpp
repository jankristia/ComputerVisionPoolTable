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
        cv::Mat getRoiTable() { return this->roiTable; }

        cv::Mat removeTableColor(cv::Mat hsvFrame);
        std::vector<cv::Vec2f> chooseFourLines(std::vector<cv::Vec2f> lines);
        void drawDetectedLines(cv::Mat frame);

        std::vector<cv::Point> findIntersections(cv::Mat frame);
        cv::Point computeCentroid(std::vector<cv::Point> intersections);
        std::vector<cv::Point> refineIntersections(std::vector<cv::Point> intersections, cv::Point centroid);

        cv::Mat detectTable(cv::Mat frame);
};

#endif
