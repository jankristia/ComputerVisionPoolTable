#include "MeanAveragePrecision.hpp"

void MeanAveragePrecision::loadGroundTruth(const std::string& filepath) {
    std::ifstream file(filepath);
    std::string line;
    while(std::getline(file, line)) {
        std::istringstream iss(line);
        BoundingBox box;
        iss >> box.x >> box.y >> box.width >> box.height >> box.ballType;
        this->groundTruth.push_back(box);
    }
}

void MeanAveragePrecision::setDetectedBoxes(std::vector<cv::Rect> detectedBoundingBoxes) {
    for(const auto& box : detectedBoundingBoxes) {
        BoundingBox b;
        b.x = box.x;
        b.y = box.y;
        b.width = box.width;
        b.height = box.height;
        b.ballType = 0; // until we have working segmentation
        this->detectedBoxes.push_back(b);
    } 
}

double MeanAveragePrecision::calculateIoU(const BoundingBox& box1, const BoundingBox& box2) {
    int x1 = std::max(box1.x, box2.x);
    int y1 = std::max(box1.y, box2.y);
    int x2 = std::min(box1.x + box1.width, box2.x + box2.width);
    int y2 = std::min(box1.y + box1.height, box2.y + box2.height);

    int intersection = std::max(0, x2 - x1) * std::max(0, y2 - y1);
    int unionArea = box1.width * box1.height + box2.width * box2.height - intersection;

    return static_cast<double>(intersection) / static_cast<double>(unionArea);
}

std::vector<bool> MeanAveragePrecision::evaluateDetections(std::vector<BoundingBox> groundTruth, std::vector<BoundingBox> detectedBoxes, double threshold) {
    std::vector<bool> detections;
    for(const auto& detectedBox : detectedBoxes) {
        bool detected = false;
        for(const auto& gtBox : groundTruth) {
            double iou = this->calculateIoU(detectedBox, gtBox);
            if(iou > threshold) {
                detected = true;
                break;
            }
        }
        detections.push_back(detected);
    }
    return detections;
}

void MeanAveragePrecision::calculatePrecisionRecall() {
    int truePositives = 0;
    int falsePositives = 0;
    int falseNegatives = 0;
    for(const auto& detected : this->evaluateDetections(this->groundTruth, this->detectedBoxes, 0.5)) {
        if(detected) {
            truePositives++;
        } else {
            falsePositives++;
        }
    }
    falseNegatives = this->groundTruth.size() - truePositives;
    float precision = static_cast<float>(truePositives) / static_cast<float>(truePositives + falsePositives);
    float recall = static_cast<float>(truePositives) / static_cast<float>(truePositives + falseNegatives);
    this->precisions.push_back(precision);
    this->recalls.push_back(recall);
}

double MeanAveragePrecision::calculateMeanAveragePrecision() {
    double sum = 0;
    for(size_t i = 0; i < this->precisions.size(); i++) {
        sum += this->precisions[i];
    }
    return sum / this->precisions.size();
}

double MeanAveragePrecision::meanAveragePrecisionCalculation(cv::Mat frame, std::string groundTruthPath) {
    cv::Mat tableFrame;
    TableDetector tableDetector;
    tableFrame = tableDetector.detectTable(frame);

    // Detect the balls and make bounding boxes
    BallDetector ballDetector;
    ballDetector.detectBalls(tableFrame);
    ballDetector.setBoundingBoxes();


    MeanAveragePrecision map;
    map.loadGroundTruth(groundTruthPath);
    map.setDetectedBoxes(ballDetector.boundingBoxes);
    map.calculatePrecisionRecall();
    double meanAveragePrecision = map.calculateMeanAveragePrecision();
    return meanAveragePrecision;
}