#ifndef MEANAVERAGEPRECISION_HPP
#define MEANAVERAGEPRECISION_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>


#include "BallDetector.hpp"
#include "TableDetector.hpp"

struct BoundingBox {
    int x;
    int y;
    int width;
    int height;
    int ballType; // 0 = background 1 = white ball, 2 = black, 3 = solid, 4 = striped, 5 = playing field
};


class MeanAveragePrecision {
    public:
    std::vector<BoundingBox> groundTruth;
    std::vector<BoundingBox> detectedBoxes;
    std::vector<float> precisions;
    std::vector<float> recalls;

    void loadGroundTruth(const std::string& filepath);
    void setDetectedBoxes(std::vector<cv::Rect> detectedBoundingBoxes); // Untill we have working segmentation we set ballType to 0
    double calculateIoU(const BoundingBox& box1, const BoundingBox& box2);
    std::vector<bool> evaluateDetections(const std::vector<BoundingBox>& groundTruth, const std::vector<BoundingBox>& detectedBoxes, double threshold);
    void calculatePrecisionRecall();
    double calculateAveragePrecision();

    double averagePrecisionCalculation(cv::Mat frame, std::string groundTruthPath);

};

#endif