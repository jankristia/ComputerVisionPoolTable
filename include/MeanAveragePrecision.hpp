// Main author: Jan Kristian Alstergren
#ifndef MEANAVERAGEPRECISION_HPP
#define MEANAVERAGEPRECISION_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>


#include "BallDetector.hpp"
#include "TableDetector.hpp"
#include "BoundingBox.hpp"

class MeanAveragePrecision {
    // // To calculate the mean average precision, run the following code in main.cpp:
    // // frame is the current frame from the video
    // // groundTruthPath is the path to the ground truth bounding boxes
    // // All the ball table and ball detection is done within the class
    // MeanAveragePrecision map;
    // double averagePrecision = map.averagePrecisionCalculation(frame, groundTruthPath);
    // std::cout << "Average Precision: " << averagePrecision << std::endl;
    public:
    std::vector<BoundingBox> groundTruth;
    std::vector<BoundingBox> detectedBoxes;
    std::vector<float> precisions;
    std::vector<float> recalls;

    void loadGroundTruth(const std::string& filepath);
    void setDetectedBoxes(std::vector<BoundingBox> detectedBoundingBoxes);
    double calculateIoU(const BoundingBox& box1, const BoundingBox& box2);
    std::vector<bool> evaluateDetections(const std::vector<BoundingBox>& groundTruth, const std::vector<BoundingBox>& detectedBoxes, double threshold);
    void calculatePrecisionRecall();
    double calculateAveragePrecision();

    double averagePrecisionCalculation(cv::Mat frame, std::string groundTruthPath);
};

#endif